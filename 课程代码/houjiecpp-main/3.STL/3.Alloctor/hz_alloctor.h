#ifndef _HZ_ALLOCTOR_H
#define _HZ_ALLOCTOR_H

#include <new>      // for placement new
#include <cstddef>  // for ptrdiff_t, size_t
#include <cstdlib>  // for exit()
#include <climits>  // for UNIT_MAX
#include <iostream> // for cerr
#include <vector>
#include <map>

using namespace std;

namespace HZ
{

    template <typename T>
    inline T *_allocate(ptrdiff_t size, T *)
    {
        set_new_handler(0);
        T *tmp = (T *)(::operator new((size_t)(size * sizeof(T))));
        if (tmp == 0)
        {
            cerr << "out of memory" << endl;
            exit(1);
        }
        return tmp;
    }

    template <typename T>
    inline void _deallocate(T *buffer)
    {
        ::operator delete(buffer);
    }

    template <typename T1, typename T2>
    inline void _construct(T1 *p, const T2 &value)
    {
        new (p) T1(value); // placement new. invoke ctor of T1
    }

    template <typename T>
    inline void _destroy(T *ptr)
    {
        ptr->~T();
    }

    template <typename T>
    class allocator
    {
    public:
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        // rebind allocator of type U
        template <class U>
        struct rebind
        {
            typedef allocator<U> other;
        };

        // hint used for locality. ref.[Austren],p198
        pointer allocate(size_type n, const void *hint = 0)
        {
            return _allocate((difference_type)n, (pointer)0);
        }

        void deallocate(pointer p, const T &value)
        {
            _construct(p, value);
        }

        void destroy(pointer p) { _destroy(p); }
        pointer address(reference x) { return (pointer)&x; }
        const_pointer const_address(const_reference x)
        {
            return (const_pointer)&x;
        }
        size_type max_size() const
        {
            return size_type(UINT_MAX / sizeof(T));
        }
    };

} // end of namespace HZ

namespace HZ_2
{

/* 下面是第一级适配器的使用 */
#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#define __THROW_BAD_ALLOC            \
    cerr << "out of memery" << endl; \
    exit(1);
#endif

    // malloc_based allocator. 通常比稍后介绍的default alloc速度慢
    // 一般而言是 thread-safe，并且对于空间的利用比较高效(efficient)
    // 一下是第一级适配器
    // 注意 无template型别参数 至于非型别参数 inst，则完全没有派上用场
    template <int inst>
    class __malloc_alloc_template
    {
    private:
        // 以下函数用来处理内存不足的情况
        // oom:out of memory
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *, size_t);
        static void (*__malloc_alloc_oom_handler)();

    public:
        static void *allocate(size_t n)
        {
            void *result = malloc(n); //第一级配置直接使用malloc
            //以下无法满足的时候，改用oom_malloc()
            if (0 == result)
                result = oom_malloc(n);
            return result;
        }
        static void deallocate(void *p, size_t /* n */)
        {
            free(p); //第一级配置器直接使用free()
        }

        static void *reallcate(void *p, size_t /* old_sz */, size_t new_sz)
        {
            void *result = realloc(p, new_sz);
            //以下条件无法满足的时候，改用oom_malloc()
            if (0 == result)
                result = oom_realloc(p, new_sz);
            return result;
        }

        // 以下是仿真c++的set_new_handler(),换句话说，你可以通过它指定你自己的out_of_memory handler
        static void (*set_malloc_handler(void (*f)()))()
        {
            void (*old)() = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return (old);
        }
    };

    // malloc_alloc out-of-memory handling
    // 初值为0，有待客户端设定
    template <int inst>
    void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;
    template <int inst>
    void *__malloc_alloc_template<inst>::oom_malloc(size_t n)
    {
        void (*my_malloc_handler)();
        void *result;
        for (;;)
        { //不断尝试释放、配置、再释放、再配置……
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (0 == my_malloc_handler)
            {
                __THROW_BAD_ALLOC;
            }
            (*my_malloc_handler)(); // 调用处理例程，企图释放内存
            result = malloc(n);     // 再次常识配置内存
            if (result)
                return (result);
        }
    }
    template <int inst>
    void *__malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
    {
        void (*my_malloc_handler)();
        void *result;
        for (;;)
        { //不断尝试释放、配置、再释放、再配置……
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (0 == my_malloc_handler)
            {
                __THROW_BAD_ALLOC;
            }
            (*my_malloc_handler)(); //调用处理例程，企图释放内存
            result = realloc(p, n); //再次尝试配置内存
            if (result)
                return (result);
        }
    }
    // 注意，以下直接将参数inst指定为0
    typedef __malloc_alloc_template<0> malloc_alloc;

    /* 下面是第二级适配器的部分实现内容 */
    enum
    {
        __ALIGN = 8
    }; //小型区块的上调边界
    enum
    {
        __MAX_BYTES = 128
    }; //小型区块的上限
    enum
    {
        __NFREELISTS = __MAX_BYTES / __ALIGN
    }; // free list 的个数

    //以下是第二级配置器
    //注意，无“template型别参数”，且第二参数完全没派上用场
    //第一参数用于多线程环境下。本书不讨论多线程环境
    template <bool threads, int inst>
    class __default_alloc_template
    {
    private:
        // ROUND_UP()    将bytes上调至8的倍数
        static size_t ROUND_UP(size_t bytes)
        {
            return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
        }

    private:
        union obj
        { // free-lists的节点构造
            union obj *free_list_link;
            char client_data[1]; /* The Client sees this */
        };

    private: // 16个free_lists
        // 16个free-lists
        static obj *volatile free_list[__NFREELISTS];
        //以下函数根据区块大小，决定使用第n号free-list。n从0算起
        static size_t FREELIST_INDEX(size_t bytes)
        {
            return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
        }

        // 返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
        static void *refill(size_t n);

        // 配置一大块空间，可容纳nobjs大小为“size”的区块
        // 如果配置nobjs个区块有所不变，nobjs可能会降低
        static char *chunk_alloc(size_t size, int &nobjs);

        // Chunk allocation state
        static char *start_free; // 内存池的起始位置，只在chunk_alloc()中变化
        static char *end_free;   // 内存池结束位置，只在chunk_alloc()中变换
        static size_t heap_size;

    public:
        static void *allocate(size_t n);
        static void deallocate(void *p, size_t n);
        static void *reallocate(void *p, size_t old_sz, size_t new_sz);
    };

    // 以下是static data member的定义与初值设定
    template <bool threads, int inst>
    char *__default_alloc_template<threads, inst>::start_free = 0;
    template <bool threads, int inst>
    char *__default_alloc_template<threads, inst>::end_free = 0;
    template <bool threads, int inst>
    size_t __default_alloc_template<threads, inst>::heap_size = 0;
    template <bool threads, int inst>
    typename __default_alloc_template<threads, inst>::obj *volatile __default_alloc_template<threads, inst>::free_list[__NFREELISTS] =
        {
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
    };

    // n must be > 0
    template <bool threads, int inst>
    void *__default_alloc_template<threads, inst>::allocate(size_t n)
    {
        obj *volatile *my_free_list;
        obj *result;
        // 大于128则调用第一级配置器
        if (n > (size_t)__MAX_BYTES)
        {
            return (malloc_alloc::allocate(n));
        }
        // 寻找16个free_list中适当的一个
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == 0)
        {
            // 没有找到可用的free list，准备重新填充free list
            void *R = refill(ROUND_UP(n)); // 下节详述
            return r;
        }
        // 调整free list
        *my_free_list = result->free_list_link;
        return (result);
    }

    // p 不可以是0
    template <bool threads, int inst>
    void __default_alloc_template<threads, inst>::deallocate(void *p, size_t n)
    {
        obj *q = (obj *)p;
        obj *volatile *my_free_list;

        // 大于128就使用第一级适配器
        if (n > (size_t)__MAX_BYTES)
        {
            malloc_alloc::deallocate(p, n);
            return;
        }
        // 寻找对应的free list
        my_free_list = free_list + FREELIST_INDEX(n);
        // 调整free list,回收区块
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
}

#endif // _JJALLOC_