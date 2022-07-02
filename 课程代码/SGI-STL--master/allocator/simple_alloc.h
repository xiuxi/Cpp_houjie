#pragma once
#ifndef SIMPLE_ALLOC_H
#define SIMPLE_ALLOC_H

#ifndef __THROW_BAD_ALLOC
	#include <new>
	#define __THROW_BAD_ALLOC throw std::bad_alloc()
#endif // !__THROW_BAD_ALLOC

#include <stdlib.h>
#include <string.h>

template <int inst>
class __malloc_alloc_template
{
public:
	// 分配内存与释放内存的函数，在内存不足的情况下调用oom函数
	static void * allocate(size_t n)
	{
		void *ret = malloc(n);
		if (ret == 0) ret = oom_malloc(n);

		return ret;
	}
	static void deallocate(void *p, size_t) { free(p); }
	static void * reallocate(void *p, size_t, size_t new_sz)
	{
		void *ret = realloc(p, new_sz);
		if (ret == 0) ret = oom_realloc(new_sz);

		return ret;
	}

	// 用户提供内存不足时的处理函数
	static void(*set_malloc_handler(void(*f)()))()
	{
		void *old_handler = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return old_handler;
	}
private:
	// 处理内存不足的情况
	static void *oom_malloc(size_t);
	static void *oom_realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler) ();
};

template<int inst>
void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template<int inst>
inline void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
	void(*my_malloc_handler)();
	void *ret;

	for (;;)
	{
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (my_malloc_handler == 0) __THROW_BAD_ALLOC;
		(*my_malloc_handler)();
		ret = malloc(n);
		if (ret) return ret;
	}
}

template<int inst>
inline void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t new_sz)
{
	void(*my_malloc_handler)();
	void *ret;

	for (;;)
	{
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (my_malloc_handler == 0) __THROW_BAD_ALLOC;
		(*my_malloc_handler)();
		ret = realloc(p, new_sz);
		if (ret) return ret;
	}
}

typedef __malloc_alloc_template<0> malloc_alloc;

enum { __ALIGN = 8 };
enum { __MAX_BYTES = 128 };
enum { nFREELISTS = __MAX_BYTES/__ALIGN };

template <bool threads, int inst>
class __default_alloc_template
{
public:
	static void * allocate(size_t n);
	static void deallocate(void *p, size_t n);
	static void * reallocate(void *p, size_t old_sz, size_t new_sz);

private:
	// 将bytes上调至8的倍数
	static size_t ROUND_UP(size_t bytes)
	{
		return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
	}

	// 节点构造
	union obj
	{
		union obj * free_list_link;
		char client_data[1];
	};

	// free_list
	static obj * volatile free_list[nFREELISTS];

	static size_t FREELIST_INDEX(size_t bytes)
	{
		return ROUND_UP(bytes) / __ALIGN - 1;
	}

	static void *refill(size_t n);
	// 内存池分配
	static char *chunk_alloc(size_t size, int &nobjs);

	static char *start_free;
	static char *end_free;
	static size_t heap_size;
};

typedef __default_alloc_template<true, 0> alloc;
typedef __default_alloc_template<false, 0> single_client_alloc;

template<bool threads, int inst>
char * __default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char * __default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads, int inst>
__default_alloc_template<threads, inst>::template obj * volatile __default_alloc_template<threads, inst>::free_list[] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

template<bool threads, int inst>
inline void * __default_alloc_template<threads, inst>::allocate(size_t n)
{
	void *ret = 0;

	if (n > __MAX_BYTES)
		return (char *)malloc_alloc::allocate(n);
	else
	{
		obj * volatile * my_free_list = free_list + FREELIST_INDEX(n);
		obj * result = *my_free_list;
		if (result == 0)
			ret = refill(ROUND_UP(n));
		else
		{
			*my_free_list = result->free_list_link;
			ret = result;
		}
	}

	return ret;
}

template<bool threads, int inst>
inline void __default_alloc_template<threads, inst>::deallocate(void * p, size_t n)
{
	if (n > __MAX_BYTES)
		malloc_alloc::deallocate(p, n);
	else
	{
		obj * volatile * my_free_list = free_list + FREELIST_INDEX(n);
		obj * q = (obj *)p;

		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}
}

template<bool threads, int inst>
inline void * __default_alloc_template<threads, inst>::reallocate(void * p, size_t old_sz, size_t new_sz)
{
	void *ret = 0;

	if (old_sz > __MAX_BYTES && new_sz > __MAX_BYTES)
		return malloc_alloc::reallocate(p, old_sz, new_sz);

	if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;

	ret = allocate(new_sz);
	size_t copy_sz = new_sz > old_sz ? old_sz : new_sz;
	memcpy(new_sz, old_sz, copy_sz);
	deallocate(p, old_sz);

	return ret;
}

template<bool threads, int inst>
inline void * __default_alloc_template<threads, inst>::refill(size_t n)
{
	int nobjs = 20;
	char *chunk = chunk_alloc(n, nobjs);
	obj * volatile * my_free_list = 0;
	obj * ret = 0;
	obj * current_obj = 0;
	obj * next_obj = 0;
	int i = 0;

	if (nobjs == 1) return chunk;

	my_free_list = free_list + FREELIST_INDEX(n);

	ret = (obj *)chunk;
	*my_free_list = next_obj = (obj *)(chunk + n);

	for (i = 1; ; ++i)
	{
		current_obj = next_obj;
		next_obj = (obj *)((char *)next_obj + n);
		if (nobjs - 1 == i)
		{
			current_obj->free_list_link = 0;
			break;
		}
		else
		{
			current_obj->free_list_link = next_obj;
		}
	}

	return ret;
}

template<bool threads, int inst>
inline char * __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int & nobjs)
{
	char * ret;
	size_t total_bytes = size * nobjs;
	size_t bytes_left = end_free - start_free;

	if (bytes_left >= total_bytes)
	{
		ret = start_free;
		start_free += total_bytes;
		return ret;
	}
	else if (bytes_left >= size)
	{
		nobjs = (int)(bytes_left / size);
		total_bytes = nobjs * size;
		ret = start_free;
		start_free += total_bytes;
		return ret;
	}
	else
	{
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

		if (bytes_left > 0)
		{
			obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
			((obj *)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj *)start_free;
		}
		start_free = (char *)malloc(bytes_to_get);
		if (start_free == 0)
		{
			size_t i;
			obj * volatile * my_free_list;
			obj * p;

			for (i = size; i < __MAX_BYTES; i += __ALIGN)
			{
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (p == 0)
				{
					*my_free_list = p->free_list_link;
					start_free = (char *)p;
					end_free = start_free + i;

					return chunk_alloc(size, nobjs);
				}
			}

			end_free = 0;
			start_free = (char *)malloc_alloc::allocate(bytes_to_get);
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;

		return chunk_alloc(size, nobjs);
	}
}

template<class _Tp, class _Alloc>
class simple_alloc 
{
public:
	static _Tp* allocate(size_t n)
	{
		return 0 == n ? 0 : (_Tp*)_Alloc::allocate(n * sizeof(_Tp));
	}
	static _Tp* allocate()
	{
		return (_Tp*)_Alloc::allocate(sizeof(_Tp));
	}
	static void deallocate(_Tp* p, size_t n)
	{
		if (0 != n) _Alloc::deallocate(p, n * sizeof(_Tp));
	}
	static void deallocate(_Tp* p)
	{
		_Alloc::deallocate(p, sizeof(_Tp));
	}
};

template<class _Tp>
class allocator
{
	typedef alloc _Alloc;
public:
	typedef size_t     size_type;
	typedef ptrdiff_t  difference_type;
	typedef _Tp*       pointer;
	typedef const _Tp* const_pointer;
	typedef _Tp&       reference;
	typedef const _Tp& const_reference;
	typedef _Tp        value_type;

	template <class _Tp1> struct rebind {
		typedef allocator<_Tp1> other;
	};

	allocator() {}
	allocator(const allocator&) {}
	template <class _Tp1> allocator(const allocator<_Tp>&) {}
	~allocator() {}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }

	_Tp* allocate(size_type n, const void * = 0)
	{
		return n != 0 ? static_cast<_Tp*>(_Alloc::allocate(n * sizeof(_Tp))) : 0;
	}

	void deallocate(pointer p, size_type n)
	{
		_Alloc::deallocate(p, n * sizeof(_Tp));
	}

	size_type max_size() const { size_t(-1) / sizeof(_Tp); }

	void construct(pointer p, const _Tp& val) { new(p) _Tp(val); }
	void destory(pointer p) { p->~_Tp(); }
};

#endif // !1
