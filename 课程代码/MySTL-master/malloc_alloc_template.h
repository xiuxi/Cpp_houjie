// 这一段宏定义什么意思
// 0下面的内容必定无法执行

#if 0
# include <new>
# define _THROW_BAD_ALLOC throw bad_alloc
#elif !defined(_THROW_BAD_ALLOC)
#include <iostream>
# define _THROW_BAD_ALLOC cerr<<"out of memory"<<endl;exit(1)
#endif

template<int inst>
class __malloc_alloc_template{
    // 第一级配置器
    private:
    // 以下都是函数指针，用来处理内存不足的情况
    // oom means "out of memory"
    // 第一：声明可以不给出形参名，只给出形参类型,在定义时给出具体形参名
    // 第二：在定义时只有类型没有参数名的形参也叫哑元，一般是作兼容使用，
    // 比如说某个函数的形参已经不再需要了，但是提供的对外的接口变量数不能改变，
    // 不然对其他模块的影响比较大，这种情况下可以使用哑元

    static void *oom_malloc(size_t);
    static void *oom_realloc(void*, size_t);
    static void (* __malloc_alloc_oom_handler)();

    public:
    static void* allocate(size_t n)
    {
        void *result = malloc(n);
        if(result == 0)
        result = oom_malloc(n);// 溢出分配
        return result;
    }
    static void deallocate(void *p, size_t /* n*/)
    {
        free(p);
    }
    static void* reallocate(void *p, size_t /* old_sz */, size_t new_sz)
    {
        realloc(p, new_sz);
        if(p == 0)
        {
            oom_realloc(p, new_sz);
        }
        return p;
    }
    // 仿真set_new_handler();
    static void(* set_malloc_handler(void (*f))) ()
    {
        // 不太明白这个函数
        void (* old) () = __malloc_alloc_oom_handler; 
        __malloc_alloc_oom_handler = f;
        return old;
    }
};

// 客端处理内存的函数，默认为缺省。
// 设计“内存不足的处理例程”是客端的责任！
// C++的new handler机制就是内存不足时在抛出异常之前允许客端调用handler函数，该函数一般为客端的内存处理函数
template <int inst>
void (* __malloc_alloc_template<inst>:: __malloc_alloc_oom_handler)() = 0;

template <int inst>
void *__malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void* handler = __malloc_alloc_oom_handler;
    while(1)
    {
        if(handler == 0){_THROW_BAD_ALLOC;}// 缺省内存方法，则抛出错误
        *handler();// 运行处理内存的方法
        void *p = malloc(n);// 试图分配内存
        if(p)return p;
    }
}

template <int inst>
void *__malloc_alloc_template<inst>::oom_realloc(void *p , size_t n)
{
    void* handler = __malloc_alloc_oom_handler;
    while(1)
    {
        if(handler == 0){_THROW_BAD_ALLOC;}
        *handler();
        void *p = realloc(p, n);
        if(p) return p;
    }
}