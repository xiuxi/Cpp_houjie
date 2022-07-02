// Filename:    stl_alloc.h  
  

  
// �ر�˵��: SGI STL��allocator���ҵı��뻷���²�ʹ���ڴ��  
//          �����ڴ�ز������ڴ��ͷŲ���, ���ͷ�ʱ��Ϊ�����˳�����stack unwinding  
//          �ɲ���ϵͳ��֤�ڴ�Ļ���  
  
/* 
 * Copyright (c) 1996-1997 
 * Silicon Graphics Computer Systems, Inc. 
 * 
 * Permission to use, copy, modify, distribute and sell this software 
 * and its documentation for any purpose is hereby granted without fee, 
 * provided that the above copyright notice appear in all copies and 
 * that both that copyright notice and this permission notice appear 
 * in supporting documentation.  Silicon Graphics makes no 
 * representations about the suitability of this software for any 
 * purpose.  It is provided "as is" without express or implied warranty. 
 */  
  
/* NOTE: This is an internal header file, included by other STL headers. 
 *   You should not attempt to use it directly. 
 */  
  
#ifndef __SGI_STL_INTERNAL_ALLOC_H  
#define __SGI_STL_INTERNAL_ALLOC_H  
  
#ifdef __SUNPRO_CC  
#  define __PRIVATE public  
// SUN��������private���ƹ���, ��Ҫ����Ȩ��  
#else  
#  define __PRIVATE private  
#endif  
  
// Ϊ�˱�֤������, ���ڲ�֧��ģ���ྲ̬��Ա�����, ʹ��malloc()�����ڴ����  
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG  
#  define __USE_MALLOC  
#endif  
  
// ʵ����һЩ��׼��node allocator  
// ���ǲ�ͬ��C++��׼����STLԭʼSTL��׼  
// ��Щallocatorû�з�װ��ָͬ������  
// ��ʵ�����Ǽٶ�ֻ��һ��ָ������  
// allocation primitives���ڷ��䲻����ԭʼSTL allocator����Ķ����Ķ���  
  
#if 0  
#   include <new>  
#   define __THROW_BAD_ALLOC throw bad_alloc  
#elif !defined(__THROW_BAD_ALLOC)  
#   include <iostream.h>  
#   define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1)  
#endif  
  
#ifndef __ALLOC  
#   define __ALLOC alloc  
#endif  
#ifdef __STL_WIN32THREADS  
#   include <windows.h>  
#endif  
  
#include <stddef.h>  
#include <stdlib.h>  
#include <string.h>  
#include <assert.h>  
#ifndef __RESTRICT  
#  define __RESTRICT  
#endif  
  
// ���߳�֧��  
// __STL_PTHREADS       // GCC������  
// _NOTHREADS           // ��֧�ֶ��߳�  
// __STL_SGI_THREADS    // SGI����ר��  
// __STL_WIN32THREADS   // MSVC������  
#if !defined(__STL_PTHREADS) && !defined(_NOTHREADS) \  
 && !defined(__STL_SGI_THREADS) && !defined(__STL_WIN32THREADS)  
#   define _NOTHREADS  
#endif  
  
# ifdef __STL_PTHREADS  
    // POSIX Threads  
    // This is dubious, since this is likely to be a high contention  
    // lock.   Performance may not be adequate.  
#   include <pthread.h>  
#   define __NODE_ALLOCATOR_LOCK \  
        if (threads) pthread_mutex_lock(&__node_allocator_lock)  
#   define __NODE_ALLOCATOR_UNLOCK \  
        if (threads) pthread_mutex_unlock(&__node_allocator_lock)  
#   define __NODE_ALLOCATOR_THREADS true  
#   define __VOLATILE volatile  // Needed at -O3 on SGI  
# endif  
# ifdef __STL_WIN32THREADS  
    // The lock needs to be initialized by constructing an allocator  
    // objects of the right type.  We do that here explicitly for alloc.  
#   define __NODE_ALLOCATOR_LOCK \  
        EnterCriticalSection(&__node_allocator_lock)  
#   define __NODE_ALLOCATOR_UNLOCK \  
        LeaveCriticalSection(&__node_allocator_lock)  
#   define __NODE_ALLOCATOR_THREADS true  
#   define __VOLATILE volatile  // may not be needed  
# endif /* WIN32THREADS */  
# ifdef __STL_SGI_THREADS  
    // This should work without threads, with sproc threads, or with  
    // pthreads.  It is suboptimal in all cases.  
    // It is unlikely to even compile on nonSGI machines.  
  
    extern "C" {  
      extern int __us_rsthread_malloc;  
    }  
    // The above is copied from malloc.h.  Including <malloc.h>  
    // would be cleaner but fails with certain levels of standard  
    // conformance.  
#   define __NODE_ALLOCATOR_LOCK if (threads && __us_rsthread_malloc) \  
                { __lock(&__node_allocator_lock); }  
#   define __NODE_ALLOCATOR_UNLOCK if (threads && __us_rsthread_malloc) \  
                { __unlock(&__node_allocator_lock); }  
#   define __NODE_ALLOCATOR_THREADS true  
#   define __VOLATILE volatile  // Needed at -O3 on SGI  
# endif  
# ifdef _NOTHREADS  
//  Thread-unsafe  
#   define __NODE_ALLOCATOR_LOCK  
#   define __NODE_ALLOCATOR_UNLOCK  
#   define __NODE_ALLOCATOR_THREADS false  
#   define __VOLATILE  
# endif  
  
__STL_BEGIN_NAMESPACE  
  
#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)  
#pragma set woff 1174  
#endif  
  
// Malloc-based allocator.  Typically slower than default alloc below.  
// Typically thread-safe and more storage efficient.  
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG  
# ifdef __DECLARE_GLOBALS_HERE  
    void (* __malloc_alloc_oom_handler)() = 0;  
    // g++ 2.7.2 does not handle static template data members.  
# else  
    extern void (* __malloc_alloc_oom_handler)();  
# endif  
#endif  
  
// һ��������  
template <int inst>  
class __malloc_alloc_template  
{  
private:  
    // ������������__malloc_alloc_oom_handler�����ѭ�������ڴ�,  
    // ֱ���ɹ�����  
    static void *oom_malloc(size_t);  
    static void *oom_realloc(void *, size_t);  
  
    // ���������֧��ģ���ྲ̬��Ա, ��ʹ�ô�������, ����C++��set_new_handler()  
    // Ĭ��ֵΪ0, ���������, ���ڴ����ʧ��ʱֱ��__THROW_BAD_ALLOC  
#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG  
    static void (* __malloc_alloc_oom_handler)();  
#endif  
  
public:  
    // ����ָ����С���ڴ�(size_t n)�� �������ʧ��, �����ѭ������׶�  
    // ѭ������ǰ����Ҫ��֤��ȷ������__malloc_alloc_oom_handler  
    static void * allocate(size_t n)  
    {  
        void *result = malloc(n);  
        if (0 == result) result = oom_malloc(n);  
        return result;  
    }  
  
    // �����size_t��Ϊ�˼���operator delele  
    static void deallocate(void *p, size_t /* n */)  
    { free(p); }  
  
    // ���·����ڴ��С, �ڶ���������Ϊ�˼���operator new  
    static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)  
    {  
        void * result = realloc(p, new_sz);  
        if (0 == result) result = oom_realloc(p, new_sz);  
        return result;  
    }  
  
    // ���ô�������, ����ԭ���ĺ���ָ��  
    // ������C++��׼�涨�Ľӿ�  
    static void (* set_malloc_handler(void (*f)()))()  
    {  
        void (* old)() = __malloc_alloc_oom_handler;  
        __malloc_alloc_oom_handler = f;  
        return(old);  
    }  
};  
  
// malloc_alloc out-of-memory handling  
  
#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG  
template <int inst>  
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;  
#endif  
  
// ���������__malloc_alloc_oom_handler, ������ִ�д�������, Ȼ��ѭ������ֱ���ɹ�  
// ���δ����__malloc_alloc_oom_handler, __THROW_BAD_ALLOC  
template <int inst>  
void * __malloc_alloc_template<inst>::oom_malloc(size_t n)  
{  
    void (* my_malloc_handler)();  
    void *result;  
  
    for (;;) {  
        my_malloc_handler = __malloc_alloc_oom_handler;  
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }  
        (*my_malloc_handler)();  
        result = malloc(n);  
        if (result) return(result);  
    }  
}  
  
template <int inst>  
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)  
{  
    void (* my_malloc_handler)();  
    void *result;  
  
    for (;;) {  
        my_malloc_handler = __malloc_alloc_oom_handler;  
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }  
        (*my_malloc_handler)();  
        result = realloc(p, n);  
        if (result) return(result);  
    }  
}  
  
// ����汾��STL��û��ʹ��non-typeģ�����  
typedef __malloc_alloc_template<0> malloc_alloc;  
  
// ������еĽӿ���ʵ����STL��׼�е�allocator�Ľӿ�  
// ʵ�������е�SGI STL��ʹ����������ڴ�����  
// ����: stl_vector.h��  
// template <class T, class Alloc = alloc>  
// class vector  
// {  
//      ...  
// protected:  
//      typedef simple_alloc<value_type, Alloc> data_allocator;  
//      ...  
//};  
template<class T, class Alloc>  
class simple_alloc  
{  
public:  
    static T *allocate(size_t n)  
                { return 0 == n? 0 : (T*) Alloc::allocate(n * sizeof (T)); }  
    static T *allocate(void)  
                { return (T*) Alloc::allocate(sizeof (T)); }  
    static void deallocate(T *p, size_t n)  
                { if (0 != n) Alloc::deallocate(p, n * sizeof (T)); }  
    static void deallocate(T *p)  
                { Alloc::deallocate(p, sizeof (T)); }  
};  
  
// Allocator adaptor to check size arguments for debugging.  
// Reports errors using assert.  Checking can be disabled with  
// NDEBUG, but it's far better to just use the underlying allocator  
// instead when no checking is desired.  
// There is some evidence that this can confuse Purify.  
template <class Alloc>  
class debug_alloc  
{  
private:  
    enum {extra = 8};       // Size of space used to store size.  Note  
                            // that this must be large enough to preserve  
                            // alignment.  
  
public:  
  
    // extra ��֤�������Ϊ0���ڴ�ռ�, ����Ҫ��֤�ڴ����  
    // �ѷ����ڴ����ǰ�����ó�n�Ĵ�С, ���ں���У��  
    // �ڴ��������þ��Ǳ���ǰ��extra��С�����ݲ����޸�  
    static void * allocate(size_t n)  
    {  
        char *result = (char *)Alloc::allocate(n + extra);  
        *(size_t *)result = n;  
        return result + extra;  
    }  
  
    // ���*(size_t *)real_p != n��϶�������ǰԽ��  
    static void deallocate(void *p, size_t n)  
    {  
        char * real_p = (char *)p - extra;  
        assert(*(size_t *)real_p == n);  
        Alloc::deallocate(real_p, n + extra);  
    }  
  
    static void * reallocate(void *p, size_t old_sz, size_t new_sz)  
    {  
        char * real_p = (char *)p - extra;  
        assert(*(size_t *)real_p == old_sz);  
        char * result = (char *)  
                      Alloc::reallocate(real_p, old_sz + extra, new_sz + extra);  
        *(size_t *)result = new_sz;  
        return result + extra;  
    }  
};  
  
# ifdef __USE_MALLOC  
  
typedef malloc_alloc alloc;  
typedef malloc_alloc single_client_alloc;  
  
# else  
  
// Ĭ�ϵ�node allocator  
// ����к��ʵı�����, �ٶ�����ԭʼ��STL class-specific allocators���µȼ�  
// ���Ǿ��в��������ڴ���Ƭ���ŵ�  
// Default_alloc_template����������ʵ�����ʵ�, ��δ�����ܻ���ʧ  
// �ͻ�ֻ���ڵ���ʹ��alloc  
//  
// ��Ҫ��ʵ������:  
// 1. ����ͻ�����һ��size > __MAX_BYTE�Ķ���, ��ֱ��ʹ��malloc()����  
// 2. �������������, ���ǽ��������Ĵ�С�����ڴ������������ROUND_UP(requested_size)  
// TODO: ������  
// 2. In all other cases, we allocate an object of size exactly  
//    ROUND_UP(requested_size).  Thus the client has enough size  
//    information that we can return the object to the proper free list  
//    without permanently losing part of the object.  
//  
  
// ��һ��ģ�����ָ���Ƿ��ж���һ���߳�ʹ�ñ�allocator  
// ��һ��default_allocʵ���з������, ����һ��deallocateʵ�����ͷŶ���, �ǰ�ȫ��  
// ����Ч��ת��������Ȩ����һ������  
// ����ܵ��¶��������õ������������Ӱ��  
// �ڶ���ģ������������ڴ������default_allocʵ��  
// ��ͬ����ʹ�ò�ͬallocatorʵ��������nodeӵ�в�ͬ����, �������˴˷�����ͨ����  
  
// Sun C++ compiler��Ҫ�����ⶨ����Щö��  
#ifdef __SUNPRO_CC  
// breaks if we make these template class members:  
  enum {__ALIGN = 8};  
  enum {__MAX_BYTES = 128};  
  enum {__NFREELISTS = __MAX_BYTES/__ALIGN};  
#endif  
  
template <bool threads, int inst>  
class __default_alloc_template  
{  
private:  
  // Really we should use static const int x = N  
  // instead of enum { x = N }, but few compilers accept the former.  
# ifndef __SUNPRO_CC  
    enum {__ALIGN = 8};  
    enum {__MAX_BYTES = 128};  
    enum {__NFREELISTS = __MAX_BYTES/__ALIGN};  
# endif  
    // �����������  
    // ����һ��, __ALIGN - 1ָ������ʵ���ڴ���������  
    // ����__ALIGN = 8ʱ, ����ֻ��Ҫ7�Ϳ���ʵ�ʱ�ʾ8����(0~7)  
    // ��ô~(__ALIGN - 1)���ǽ������������  
    // ���ǽ�(bytes) + __ALIGN-1)�����Ƚ��н�λ, Ȼ��ض�  
    // ��ͱ�֤���������������  
    // ����byte = 100, __ALIGN = 8�����  
    // ~(__ALIGN - 1) = (1 000)B  
    // ((bytes) + __ALIGN-1) = (1 101 011)B  
    // (((bytes) + __ALIGN-1) & ~(__ALIGN - 1)) = (1 101 000 )B = (104)D  
    // 104 / 8 = 13, ���ʵ������������  
    // ����byte�պ������ڴ����������, �������byte��С����  
    // �ǵá�Hacker's Delight����������صļ���  
    // ������ʽ����������ĵȼ�  
    // ((((bytes) + _ALIGN - 1) * _ALIGN) / _ALIGN)  
    // ����SGI STLʹ�õķ���Ч�ʷǳ���  
    static size_t ROUND_UP(size_t bytes)  
    {  
        return (((bytes) + __ALIGN-1) & ~(__ALIGN - 1));  
    }  
__PRIVATE:  
    // �����ڴ�������  
    // Ϊ�˾������ܼ����ڴ��ʹ��, ����ʹ��һ��union  
    // ���ʹ�õ�һ����Ա, ��ָ����һ����ͬ��union obj  
    // �����ʹ�õڶ�����Ա, ��ָ��ʵ�ʵ��ڴ�����  
    // ������ʵ����������ֻʹ��һ��ָ��Ĵ�С�ռ�, ȴ��ͬʱ��������ָ���ڴ�����  
    // ��������Էǳ�ǿ, ֵ��ѧϰ  
    union obj  
    {  
        union obj * free_list_link;  
        char client_data[1];    /* The client sees this.        */  
    };  
private:  
# ifdef __SUNPRO_CC  
    static obj * __VOLATILE free_list[];  
        // Specifying a size results in duplicate def for 4.1  
# else  
    // ��������free_listΪ16  
    // ��Ӧ���ڴ��������ֱ�Ϊ8, 16, 32 ... 128  
    static obj * __VOLATILE free_list[__NFREELISTS];  
# endif  
    // ���ݴ�������Ŀռ��С, ��free_list��ѡ����ʵĴ�С  
    static  size_t FREELIST_INDEX(size_t bytes)  
    {  
        return (((bytes) + __ALIGN-1)/__ALIGN - 1);  
    }  
  
  // Returns an object of size n, and optionally adds to size n free list.  
  static void *refill(size_t n);  
  // Allocates a chunk for nobjs of size "size".  nobjs may be reduced  
  // if it is inconvenient to allocate the requested number.  
  static char *chunk_alloc(size_t size, int &nobjs);  
  
  // �ڴ��  
  static char *start_free;      // �ڴ����ʼ��  
  static char *end_free;        // �ڴ�ؽ�����  
  static size_t heap_size;      // �Ѿ��ڶ��Ϸ���Ŀռ��С  
  
// ��������������������߳�������ʹ�õ����ṩ��Ҫ֧��  
# ifdef __STL_SGI_THREADS  
    static volatile unsigned long __node_allocator_lock;  
    static void __lock(volatile unsigned long *);  
    static inline void __unlock(volatile unsigned long *);  
# endif  
  
# ifdef __STL_PTHREADS  
    static pthread_mutex_t __node_allocator_lock;  
# endif  
  
# ifdef __STL_WIN32THREADS  
    static CRITICAL_SECTION __node_allocator_lock;  
    static bool __node_allocator_lock_initialized;  
  
  public:  
    __default_alloc_template() {  
    // This assumes the first constructor is called before threads  
    // are started.  
        if (!__node_allocator_lock_initialized) {  
            InitializeCriticalSection(&__node_allocator_lock);  
            __node_allocator_lock_initialized = true;  
        }  
    }  
  private:  
# endif  
  
    // ���ڶ��̻߳���������������  
    class lock  
    {  
    public:  
        lock() { __NODE_ALLOCATOR_LOCK; }  
        ~lock() { __NODE_ALLOCATOR_UNLOCK; }  
    };  
    friend class lock;  
  
public:  
  /* n must be > 0      */  
  static void * allocate(size_t n)  
  {  
    obj * __VOLATILE * my_free_list;  
    obj * __RESTRICT result;  
  
    // ���������������__MAX_BYTES, ʹ��һ������������  
    if (n > (size_t) __MAX_BYTES) {  
        return(malloc_alloc::allocate(n));  
    }  
    my_free_list = free_list + FREELIST_INDEX(n);  
    // Acquire the lock here with a constructor call.  
    // This ensures that it is released in exit or during stack  
    // unwinding.  
#       ifndef _NOTHREADS  
        /*REFERENCED*/  
        lock lock_instance;  
#       endif  
    result = *my_free_list;  
    // ����ǵ�һ��ʹ���������������, ������������Ҫ���ڴ�  
    // �������, ���ж��ڴ������, ���������  
    if (result == 0) {  
        void *r = refill(ROUND_UP(n));  
        return r;  
    }  
    *my_free_list = result -> free_list_link;  
    return (result);  
  };  
  
  /* p may not be 0 */  
  static void deallocate(void *p, size_t n)  
  {  
    obj *q = (obj *)p;  
    obj * __VOLATILE * my_free_list;  
  
    // ���ڴ���__MAX_BYTES�Ķ���, ��Ϊ���õ���һ������������, ����ͬ��ʹ��һ���������ͷ�  
    if (n > (size_t) __MAX_BYTES) {  
        malloc_alloc::deallocate(p, n);  
        return;  
    }  
    my_free_list = free_list + FREELIST_INDEX(n);  
    // acquire lock  
#       ifndef _NOTHREADS  
        /*REFERENCED*/  
        lock lock_instance;  
#       endif /* _NOTHREADS */  
    q -> free_list_link = *my_free_list;  
    *my_free_list = q;  
    // lock is released here  
  }  
  
  static void * reallocate(void *p, size_t old_sz, size_t new_sz);  
} ;  
  
typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> alloc;  
typedef __default_alloc_template<false, 0> single_client_alloc;  
  
// ÿ�η���һ����ڴ�, ��ֹ��η���С�ڴ��������ڴ���Ƭ  
// ���з������ʱ, ���ݾ��廷�������Ƿ����  
// ���Ǽٶ�Ҫ������ڴ������ڴ����Ҫ��  
template <bool threads, int inst>  
char*  
__default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs)  
{  
    char * result;  
    size_t total_bytes = size * nobjs;  
    size_t bytes_left = end_free - start_free;  // �����ڴ��ʣ������  
  
    // ����ڴ����ʣ���ڴ�>=��Ҫ��������ڴ�, ����start_freeָ����ڴ��,  
    // �������������ڴ����ʼ��  
    if (bytes_left >= total_bytes) {  
        result = start_free;  
        start_free += total_bytes;  
        return(result);  
    }  
    // ����ڴ����ʣ���������������, ���������ٷ���һ���ڵ�ʱ,  
    // �������ܷ�������Ľڵ�, ����start_freeָ����ڴ��  
    // �������������ڴ����ʼ��  
    else if (bytes_left >= size) {  
        nobjs = bytes_left/size;  
        total_bytes = size * nobjs;  
        result = start_free;  
        start_free += total_bytes;  
        return(result);  
    }  
    // �ڴ��ʣ���ڴ���һ���ڵ�Ҳ��������  
    else {  
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);  
        // ��ʣ����ڴ�����ָ����free_list[FREELIST_INDEX(bytes_left)]  
        if (bytes_left > 0) {  
            obj * __VOLATILE * my_free_list =  
                        free_list + FREELIST_INDEX(bytes_left);  
  
            ((obj *)start_free) -> free_list_link = *my_free_list;  
            *my_free_list = (obj *)start_free;  
        }  
        start_free = (char *)malloc(bytes_to_get);  
        // ����ʧ��, ����ԭ���Ѿ�������ڴ��, ���Ƿ��д��ڵ��ڵ�ǰ������ڴ��  
        if (0 == start_free) {  
            int i;  
            obj * __VOLATILE * my_free_list, *p;  
            // Try to make do with what we have.  That can't  
            // hurt.  We do not try smaller requests, since that tends  
            // to result in disaster on multi-process machines.  
            for (i = size; i <= __MAX_BYTES; i += __ALIGN) {  
                my_free_list = free_list + FREELIST_INDEX(i);  
                p = *my_free_list;  
                // �ҵ���һ��, ��������ڴ����  
                if (0 != p) {  
                    *my_free_list = p -> free_list_link;  
                    start_free = (char *)p;  
                    end_free = start_free + i;  
                    // �ڴ�ظ������, ���·�����Ҫ���ڴ�  
                    return(chunk_alloc(size, nobjs));  
                    // Any leftover piece will eventually make it to the  
                    // right free list.  
                }  
            }  
  
            // �ٴ�ʧ��, ֱ�ӵ���һ������������, �ڴ��쳣���������ṩ����  
            // �������ҿ���, �ڴ����ʧ�ܽ������������Ѿ�ûʲô������,  
            // ���ֱ��log, Ȼ���ó������  
        end_free = 0;   // In case of exception.  
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);  
        }  
        heap_size += bytes_to_get;  
        end_free = start_free + bytes_to_get;  
        // �ڴ�ظ������, ���·�����Ҫ���ڴ�  
        return(chunk_alloc(size, nobjs));  
    }  
}  
  
  
// ����һ����СΪn�Ķ���, ���Ҽ��뵽free_list[FREELIST_INDEX(n)]  
// ���з������ʱ, ���ݾ��廷�������Ƿ����  
// ���Ǽٶ�Ҫ������ڴ������ڴ����Ҫ��  
template <bool threads, int inst>  
void* __default_alloc_template<threads, inst>::refill(size_t n)  
{  
    int nobjs = 20;  
    char * chunk = chunk_alloc(n, nobjs);  
    obj * __VOLATILE * my_free_list;  
    obj * result;  
    obj * current_obj, * next_obj;  
    int i;  
  
    // ����ڴ�ؽ���ֻ������һ������Ŀռ�, ֱ�ӷ��ؼ���  
    if (1 == nobjs) return(chunk);  
  
    // �ڴ���ܷ������Ŀռ�  
    my_free_list = free_list + FREELIST_INDEX(n);  
  
    // ��chunk�Ŀռ��н���free_list  
      result = (obj *)chunk;  
      *my_free_list = next_obj = (obj *)(chunk + n);  
      for (i = 1; ; i++) {  
        current_obj = next_obj;  
        next_obj = (obj *)((char *)next_obj + n);  
        if (nobjs - 1 == i) {  
            current_obj -> free_list_link = 0;  
            break;  
        } else {  
            current_obj -> free_list_link = next_obj;  
        }  
      }  
    return(result);  
}  
  
template <bool threads, int inst>  
void*  
__default_alloc_template<threads, inst>::reallocate(void *p,  
                                                    size_t old_sz,  
                                                    size_t new_sz)  
{  
    void * result;  
    size_t copy_sz;  
  
    // ���old_size��new_size������__MAX_BYTES, ��ֱ�ӵ���realloc()  
    // ��Ϊ�ⲿ���ڴ治�Ǿ����ڴ�ط����  
    if (old_sz > (size_t) __MAX_BYTES && new_sz > (size_t) __MAX_BYTES) {  
        return(realloc(p, new_sz));  
    }  
    // ���ROUND_UP(old_sz) == ROUND_UP(new_sz), �ڴ��Сû�仯, ���������·���  
    if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return(p);  
    // �������·��䲢��������  
    result = allocate(new_sz);  
    copy_sz = new_sz > old_sz? old_sz : new_sz;  
    memcpy(result, p, copy_sz);  
    deallocate(p, old_sz);  
    return(result);  
}  
  
#ifdef __STL_PTHREADS  
    template <bool threads, int inst>  
    pthread_mutex_t  
    __default_alloc_template<threads, inst>::__node_allocator_lock  
        = PTHREAD_MUTEX_INITIALIZER;  
#endif  
  
#ifdef __STL_WIN32THREADS  
    template <bool threads, int inst> CRITICAL_SECTION  
    __default_alloc_template<threads, inst>::__node_allocator_lock;  
  
    template <bool threads, int inst> bool  
    __default_alloc_template<threads, inst>::__node_allocator_lock_initialized  
    = false;  
#endif  
  
#ifdef __STL_SGI_THREADS  
__STL_END_NAMESPACE  
#include <mutex.h>  
#include <time.h>  
__STL_BEGIN_NAMESPACE  
// Somewhat generic lock implementations.  We need only test-and-set  
// and some way to sleep.  These should work with both SGI pthreads  
// and sproc threads.  They may be useful on other systems.  
template <bool threads, int inst>  
volatile unsigned long  
__default_alloc_template<threads, inst>::__node_allocator_lock = 0;  
  
#if __mips < 3 || !(defined (_ABIN32) || defined(_ABI64)) || defined(__GNUC__)  
#   define __test_and_set(l,v) test_and_set(l,v)  
#endif  
  
template <bool threads, int inst>  
void  
__default_alloc_template<threads, inst>::__lock(volatile unsigned long *lock)  
{  
    const unsigned low_spin_max = 30;  // spin cycles if we suspect uniprocessor  
    const unsigned high_spin_max = 1000; // spin cycles for multiprocessor  
    static unsigned spin_max = low_spin_max;  
    unsigned my_spin_max;  
    static unsigned last_spins = 0;  
    unsigned my_last_spins;  
    static struct timespec ts = {0, 1000};  
    unsigned junk;  
#   define __ALLOC_PAUSE junk *= junk; junk *= junk; junk *= junk; junk *= junk  
    int i;  
  
    if (!__test_and_set((unsigned long *)lock, 1)) {  
        return;  
    }  
    my_spin_max = spin_max;  
    my_last_spins = last_spins;  
    for (i = 0; i < my_spin_max; i++) {  
        if (i < my_last_spins/2 || *lock) {  
            __ALLOC_PAUSE;  
            continue;  
        }  
        if (!__test_and_set((unsigned long *)lock, 1)) {  
            // got it!  
            // Spinning worked.  Thus we're probably not being scheduled  
            // against the other process with which we were contending.  
            // Thus it makes sense to spin longer the next time.  
            last_spins = i;  
            spin_max = high_spin_max;  
            return;  
        }  
    }  
    // We are probably being scheduled against the other process.  Sleep.  
    spin_max = low_spin_max;  
    for (;;) {  
        if (!__test_and_set((unsigned long *)lock, 1)) {  
            return;  
        }  
        nanosleep(&ts, 0);  
    }  
}  
  
template <bool threads, int inst>  
inline void  
__default_alloc_template<threads, inst>::__unlock(volatile unsigned long *lock)  
{  
#   if defined(__GNUC__) && __mips >= 3  
        asm("sync");  
        *lock = 0;  
#   elif __mips >= 3 && (defined (_ABIN32) || defined(_ABI64))  
        __lock_release(lock);  
#   else  
        *lock = 0;  
        // This is not sufficient on many multiprocessors, since  
        // writes to protected variables and the lock may be reordered.  
#   endif  
}  
#endif  
  
// �ڴ����ʼλ��  
template <bool threads, int inst>  
char *__default_alloc_template<threads, inst>::start_free = 0;  
// �ڴ�ؽ���λ��  
template <bool threads, int inst>  
char *__default_alloc_template<threads, inst>::end_free = 0;  
  
template <bool threads, int inst>  
size_t __default_alloc_template<threads, inst>::heap_size = 0;  
// �ڴ��������������  
template <bool threads, int inst>  
__default_alloc_template<threads, inst>::obj * __VOLATILE  
__default_alloc_template<threads, inst> ::free_list[  
# ifdef __SUNPRO_CC  
    __NFREELISTS  
# else  
    __default_alloc_template<threads, inst>::__NFREELISTS  
# endif  
] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };  
// The 16 zeros are necessary to make version 4.1 of the SunPro  
// compiler happy.  Otherwise it appears to allocate too little  
// space for the array.  
  
# ifdef __STL_WIN32THREADS  
  // Create one to get critical section initialized.  
  // We do this onece per file, but only the first constructor  
  // does anything.  
  static alloc __node_allocator_dummy_instance;  
# endif  
  
#endif /* ! __USE_MALLOC */  
  
#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)  
#pragma reset woff 1174  
#endif  
  
__STL_END_NAMESPACE  
  
#undef __PRIVATE  
  
#endif /* __SGI_STL_INTERNAL_ALLOC_H */  
  
// Local Variables:  
// mode:C++  
// End:  