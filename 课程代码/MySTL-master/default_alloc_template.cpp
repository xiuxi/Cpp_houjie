#include <cstddef>
#include "malloc_alloc_template.h"
enum{__ALIGN = 8};
enum{__MAX_BYTES = 128};
enum{__NFREELISTS = __MAX_BYTES/__ALIGN};

// 第二级配置器

template <bool threads, int inst>
class __default_alloc_template{
    private:
    // 将bytes上调至8的倍数
    static size_t ROUND_UP(size_t bytes)
    {
        return (bytes+__ALIGN-1)& ~(__ALIGN-1);// ~取反 &与操作
    }
    private:
    //freelist 声明
    union obj
    {
        union obj* free_list_link;// 指向另一个obj
        char client_data[1];// 指向实际区块？
    };
    private:
    // 此处violatile防止编译器对freelist进行优化
    // 如果未作此声明则如果freelist在程序逻辑中未被用到，指针则不会被申请。
    static obj* volatile free_list[__NFREELISTS];// 存放指向链表的指针数组
    static size_t FREELIST_INDEX(size_t bytes){
        // 根据区块大小，决定使用第n号free-list。n从1开始算
        return (((bytes)+ __ALIGN-1)/__ALIGN -1);
    }

    // 返回一个大小为n的对象
    // 并可能加入大小为n的其他区块到freelist中
    static void *refill(size_t n);

    // 配置一大块空间，可容纳nobjs个大小为size的区块
    static char *chunk_alloc(size_t size, int& nobjs);

    // chunk allocate state
    static char *start_free;// 内存池起始位置。只在chunk_alloc中变化
    static char *end_free;// 内存池结束位置。只在chunk_alloc中变化
    static size_t head_size;// 推测为池大小

    public:
    static void *allocate(size_t n);
    static void *deallocate(void *p, size_t n);
    static void *reallocate(void *p,size_t old_size, size_t new_size);
};
// 定义池的参数
template <bool threads, int inst>
static char* __default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
static char* __default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
static size_t __default_alloc_template<threads, inst>::head_size = 0;

// 定义freelist
template <bool threads, int inst>
static __default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads ,inst>::free_list[__NFREELISTS]
={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

template <bool threads, int inst>
void * __default_alloc_template<threads, inst>:: allocate(size_t n)
{
    obj* result;
    // 指向 存放指向链表的指针的数组 的指针
    obj* volatile * my_free_list;
    // 如果区块大于128Byte则转调用第一级配置器
    if(n>(size_t)__MAX_BYTES)return(__malloc_alloc_template::allocate(n));
    // 找到对应的链表指针
    my_free_list = free_list + FREELIST_INDEX(n);
    // 取链表
    result = *my_free_list;
    // 如果找不到表头则将n取到8的倍数重新找
    if(result == 0)return refill(ROUND_UP(n));
    *my_free_list = result->free_list_link;
    return result;
}

// 返回一个大小为n的对象，并且有时候会为freelist增加节点
// 假设n已经上调至8的倍数
template <bool threads, int inst>
void* __default_alloc_template<threads, inst> :: refill(size_t n)
{
    int njobs = 20;
    // 注意njobs是传引用
    char *chunk = chunk_alloc(n,njobs);
    obj* volatile * my_free_list;
    obj* result;
    obj* current_obj, * next_obj;
    int i;

    // 如果只获得一个区块，就分配给调用者，freelist无新增节点
    if(1 == njobs) return chunk;
    // 否则调整freelist，纳入新节点
    my_free_list = free_list + FREELIST_INDEX(n);

    result = (obj*)chunk;// 第0块作为返回结果分配给用户
    *my_free_list = next_obj  = (obj*)(chunk+n);//从第一块开始纳入freelist

    for(i=1 ; ; i++)
    {
        current_obj = next_obj;
        next_obj = (obj*)((char*)chunk+n);
        if(nobjs -1 == i){// 到链尾跳出
            current_obj -> free_list_link = 0;
            break;
        }
        else{
            current_obj->free_list_link = next_obj;
        }
    }
    return (result);
}

template <bool threads, int inst>
char* __default_alloc_template<threads, inst> :: chunk_alloc(size_t size, int& nobjs)
{
    char * result;// 结果指针
    size_t total_bytes = size*nobjs;// 总共需要的字节数
    size_t bytes_left = end_free - start_free;// 内存池剩余容量

    if(bytes_left >= total_bytes){
        // 内存剩余空间满足需求量
        result = start_free;
        start_free = start_free + total_bytes;
        return result;
    }
    else if(bytes_left >= size)// 不能完全满足需求量，但足够一块以上
    {
        nobjs = bytes_left/size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free = start_free + total_bytes;
        return result;
    }else{
        // 内存池剩余空间连一个区块的大小都无法提供
        // 新申请的空间大小为两倍的所需空间大小加上一个随着
        // 配置次数增加而越来越大的附加量
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size>>4);
        if(bytes_left > 0 )
        {
            // 内存池还有零头，先配给适当的freelist
            // 首先寻找适当的freelist
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            // 调整freelist,将残余空间编入
            // 以下两句不太理解
            ((obj*)start_free) -> free_list_link = *my_free_list;
            * my_free_list = (obj*) start_free;
        }
        start_free = (char*)malloc(bytes_to_get);
        if(0 == start_free)
        {
            // heap 空间不足，malloc失败
            int i;
            obj* volatile * my_free_list , *p;
            // 搜索尚未被用到的freelist，该freelist有尚未被用到的区块，
            // 且区块足够大。
            // 为了防止在多进程机器上出错，不向下检视较小的区块
            for(i = size; i<__MAX_BYTES; i+__ALIGN)
            {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if(0 != p)
                {
                    //freelist 内尚有未用之块
                    // 调整freelist以释放出未用区块

                    // 修正三个变量
                    *my_free_list = p->free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    // 递归调用自己，修正nobjs
                    return (chunk_alloc(size, nobjs));
                    // 注意，任何残余零头终将被编入适当的freelist备用
                }
            }
            end_free = 0;// 没有内存可用了
            // 调用第一级配置器，试图使用oom
            start_free = (char*)__malloc_alloc_template(bytes_to_get);
        }
        heap_size += bytes_to_get;// heapsize递增
        end_free = start_free + bytes_to_get;
        return (chunk_alloc(size, nobjs));
    }

}