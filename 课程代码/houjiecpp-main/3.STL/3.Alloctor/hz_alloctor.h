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

/* �����ǵ�һ����������ʹ�� */
#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#define __THROW_BAD_ALLOC            \
    cerr << "out of memery" << endl; \
    exit(1);
#endif

    // malloc_based allocator. ͨ�����Ժ���ܵ�default alloc�ٶ���
    // һ������� thread-safe�����Ҷ��ڿռ�����ñȽϸ�Ч(efficient)
    // һ���ǵ�һ��������
    // ע�� ��template�ͱ���� ���ڷ��ͱ���� inst������ȫû�������ó�
    template <int inst>
    class __malloc_alloc_template
    {
    private:
        // ���º������������ڴ治������
        // oom:out of memory
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *, size_t);
        static void (*__malloc_alloc_oom_handler)();

    public:
        static void *allocate(size_t n)
        {
            void *result = malloc(n); //��һ������ֱ��ʹ��malloc
            //�����޷������ʱ�򣬸���oom_malloc()
            if (0 == result)
                result = oom_malloc(n);
            return result;
        }
        static void deallocate(void *p, size_t /* n */)
        {
            free(p); //��һ��������ֱ��ʹ��free()
        }

        static void *reallcate(void *p, size_t /* old_sz */, size_t new_sz)
        {
            void *result = realloc(p, new_sz);
            //���������޷������ʱ�򣬸���oom_malloc()
            if (0 == result)
                result = oom_realloc(p, new_sz);
            return result;
        }

        // �����Ƿ���c++��set_new_handler(),���仰˵�������ͨ����ָ�����Լ���out_of_memory handler
        static void (*set_malloc_handler(void (*f)()))()
        {
            void (*old)() = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return (old);
        }
    };

    // malloc_alloc out-of-memory handling
    // ��ֵΪ0���д��ͻ����趨
    template <int inst>
    void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;
    template <int inst>
    void *__malloc_alloc_template<inst>::oom_malloc(size_t n)
    {
        void (*my_malloc_handler)();
        void *result;
        for (;;)
        { //���ϳ����ͷš����á����ͷš������á���
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (0 == my_malloc_handler)
            {
                __THROW_BAD_ALLOC;
            }
            (*my_malloc_handler)(); // ���ô������̣���ͼ�ͷ��ڴ�
            result = malloc(n);     // �ٴγ�ʶ�����ڴ�
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
        { //���ϳ����ͷš����á����ͷš������á���
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (0 == my_malloc_handler)
            {
                __THROW_BAD_ALLOC;
            }
            (*my_malloc_handler)(); //���ô������̣���ͼ�ͷ��ڴ�
            result = realloc(p, n); //�ٴγ��������ڴ�
            if (result)
                return (result);
        }
    }
    // ע�⣬����ֱ�ӽ�����instָ��Ϊ0
    typedef __malloc_alloc_template<0> malloc_alloc;

    /* �����ǵڶ����������Ĳ���ʵ������ */
    enum
    {
        __ALIGN = 8
    }; //С��������ϵ��߽�
    enum
    {
        __MAX_BYTES = 128
    }; //С�����������
    enum
    {
        __NFREELISTS = __MAX_BYTES / __ALIGN
    }; // free list �ĸ���

    //�����ǵڶ���������
    //ע�⣬�ޡ�template�ͱ���������ҵڶ�������ȫû�����ó�
    //��һ�������ڶ��̻߳����¡����鲻���۶��̻߳���
    template <bool threads, int inst>
    class __default_alloc_template
    {
    private:
        // ROUND_UP()    ��bytes�ϵ���8�ı���
        static size_t ROUND_UP(size_t bytes)
        {
            return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
        }

    private:
        union obj
        { // free-lists�Ľڵ㹹��
            union obj *free_list_link;
            char client_data[1]; /* The Client sees this */
        };

    private: // 16��free_lists
        // 16��free-lists
        static obj *volatile free_list[__NFREELISTS];
        //���º������������С������ʹ�õ�n��free-list��n��0����
        static size_t FREELIST_INDEX(size_t bytes)
        {
            return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
        }

        // ����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free list
        static void *refill(size_t n);

        // ����һ���ռ䣬������nobjs��СΪ��size��������
        // �������nobjs�������������䣬nobjs���ܻή��
        static char *chunk_alloc(size_t size, int &nobjs);

        // Chunk allocation state
        static char *start_free; // �ڴ�ص���ʼλ�ã�ֻ��chunk_alloc()�б仯
        static char *end_free;   // �ڴ�ؽ���λ�ã�ֻ��chunk_alloc()�б任
        static size_t heap_size;

    public:
        static void *allocate(size_t n);
        static void deallocate(void *p, size_t n);
        static void *reallocate(void *p, size_t old_sz, size_t new_sz);
    };

    // ������static data member�Ķ������ֵ�趨
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
        // ����128����õ�һ��������
        if (n > (size_t)__MAX_BYTES)
        {
            return (malloc_alloc::allocate(n));
        }
        // Ѱ��16��free_list���ʵ���һ��
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == 0)
        {
            // û���ҵ����õ�free list��׼���������free list
            void *R = refill(ROUND_UP(n)); // �½�����
            return r;
        }
        // ����free list
        *my_free_list = result->free_list_link;
        return (result);
    }

    // p ��������0
    template <bool threads, int inst>
    void __default_alloc_template<threads, inst>::deallocate(void *p, size_t n)
    {
        obj *q = (obj *)p;
        obj *volatile *my_free_list;

        // ����128��ʹ�õ�һ��������
        if (n > (size_t)__MAX_BYTES)
        {
            malloc_alloc::deallocate(p, n);
            return;
        }
        // Ѱ�Ҷ�Ӧ��free list
        my_free_list = free_list + FREELIST_INDEX(n);
        // ����free list,��������
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
}

#endif // _JJALLOC_