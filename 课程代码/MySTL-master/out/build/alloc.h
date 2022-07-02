#ifndef MYSTL_ALLOC_H_
#define MYSTL_ALLOC_H_

//���ͷ�ļ�����һ���� alloc�����ڷ���ͻ����ڴ棬���ڴ�صķ�ʽʵ��
//���ǵ�С�����������ɵ��ڴ���Ƭ���⣬���˫�㼶������

#include<new>			
#include<cstddef>		//������һЩ���õĳ������ꡢ���ͺͺ���
#include<cstdio>		//�ṩ���������ֵ��������������

namespace mystl {	//�����ռ�mystl
//������һ�������������������Ҫ�ṩһ���ӿڣ�ʹ�������ܹ�����STL���
//�������������Ľӿ�,
template<class T, class Alloc>class simple_alloc {
public:
		//ʹ����Щ��Ա���������ô��ݸ��������ĳ�Ա������ʹ�����������õ�λ��bytesתΪ����Ԫ�صĴ�С(sizeof(T))
	static T* allocate(size_t n) { return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T)); }
	static T* allocate(void) { return (T*)Alloc::allocate(sizeof(T)); }
	static void deallocate(T* p, size_t n) { if (0 != n) Alloc::deallocate(p, n * sizeof(T)); }
	static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }
	};

//������һ��������,ֱ��ʹ��malloc()��free()
template<int inst>class _malloc_alloc_template {	//alloc�������κ�template�ͱ����
private:
//���¶��Ǻ���ָ�룬������ĺ��������������ڴ治������
	static void* oom_malloc(size_t n);
	static void* oom_realloc(void*, size_t);
	static void* (*_malloc_alloc_oom_handler)();

public:
	static void* allocate(size_t n) {		//�����ڴ�ĺ���
	void* result = malloc(n);			//��һ��������ֱ��ʹ��malloc()
	if (0 == result)	result = oom_malloc(n);		//�޷������ڴ�����ʱ������oom_malloc()
	return result;
}

static void deallocate(void* p, size_t n) {		//�ͷ��ڴ�ĺ���
	free(p);							//��һ��������ֱ��ʹ��free()
}

static void* reallocate(void* p, size_t old_sz, size_t new_sz) {	//���µ����ѷ����ڴ�ĺ���
	void* result = realloc(p, new_sz);
		if (0 == result) result = oom_realloc(p, new_sz);	//�޷������ڴ�����ʱ������oom_realloc()
		return result;
}

//��malloc����::operator new�������ڴ�(ԭ����1.��ʷԪ�أ�2.C++δ�ṩ��Ӧ��realloc()���ڴ����ò���)
//���Բ���ֱ��ʹ��C++��set_new_handler()�������Լ��趨һ�������set_new_handler()
//�����Ƿ���C++��set_new_handler()�������ͨ����ָ�����Լ���out-of-memory handler
	static void(*set_malloc_handler(void(*f)()))(){		//fָ��ָ��new-handler
	void(*old)() = _malloc_alloc_oom_handler;		//ж��old-handler
	_malloc_alloc_oom_handler = f;					//��װnew-handler
	return(old);
	}
};

//malloc_alloc out-of-memory handler,��ֵΪ0���д��ͻ����趨
template<int inst>void (*_malloc_alloc_template<inst>::_malloc_alloc_oom_handler)() = 0;

//�����Ǵ����ڴ治��ĺ�������
template <int inst>void* _malloc_alloc_template<inst>::oom_malloc(size_t n) {
	void(*my_malloc_handler)();
	void* result;
	for (;;) {						//���ϳ����ͷš����á����ͷš�������...
		my_malloc_handler = _malloc_alloc_oom_handler;
		if (0 == my_malloc_handler) { __THROW_RAD_ALLOC; }
		(*my_malloc_handler)();		//���ô������̣���ͼ�ͷ��ڴ�
		result = malloc(n);			//�ٴγ��������ڴ�
		if (result) return(result);
	}
}

template <int inst>void* _malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
	void(*my_malloc_handler)();
	void* result;
	for (;;) {						//���ϳ����ͷš����á����ͷš�������...
		my_malloc_handler = _malloc_alloc_oom_handler;
		//����ͻ���δ�趨�ڴ治�㴦�����̣�����__THROW_RAD_ALLOC����bad_alloc�쳣��Ϣ
		if (0 == my_malloc_handler) { __THROW_RAD_ALLOC; }
		(*my_malloc_handler)();		//���ô������̣���ͼ�ͷ��ڴ�
		result = malloc(n);			//�ٴγ��������ڴ�
		if (result) return(result);
	}
}

typedef _malloc_alloc_template<0> malloc_alloc;	//ֱ�ӽ�����instָ��Ϊ0

//������������������鳬��128bytes���ƽ���һ�������������������С��128bytes�����ڴ�ع���
//�ڴ��ÿ������һ����ڴ棬ά����Ӧ��free-list�����ڴ�����ʹ�free-list��ȡ����
//����ͻ����ͷ��ڴ�飬�ͻ��յ�free-list
//�������������κ�������ڴ������ϵ���8�ı���������128bytes��ÿ8bytes�ֳ�16��free-list


enum { _ALIGN = 8 };	//С��������ϵ��߽�,ÿ�����8bytes
enum { _MAX_BYTES = 128 };	//С�����������
enum { _NFREELISTS = _MAX_BYTES / _ALIGN };	//free-list�ĸ���

//�����Ƕ���������
template<bool threads, int inst>class _default_alloc_template {
private:
	static size_t ROUND_UP(size_t bytes) {		//ROUND_UP��bytes�ϵ���8�ı���
		return (((bytes)+_ALIGN - 1) & ~(_ALIGN - 1));
	}

	//free-list�Ľڵ�ṹ
	union obj {				//union�ؼ��ֶ��干���壬�ڴ�ɱ���������obj�ɱ�����һ��ָ�룬ָ����ͬ��ʽ����һ��obj
		union obj* free_list_link;	//ָ����һ������
		char client_data[1];		//�洢�����ڴ���׵�ַ
	};

	//volatile �ؼ�����һ���������η����������������ͱ�����ʾ���Ա�ĳЩ������δ֪�����ظ��ġ� 
	//���磺����ϵͳ��Ӳ�����������̵߳ȡ���������ؼ��������ı������������Է��ʸñ����Ĵ���Ͳ��ٽ����Ż���
	//�Ӷ������ṩ�������ַ���ȶ�����
	static obj* volatile free_list[_NFREELISTS];	//16����������

	static size_t FREELIST_INDEX(size_t bytes) {	//���������С������ʹ�õ�n��free-list��n��1����
		return (((bytes)+_ALIGN - 1) / _ALIGN - 1);	
	}

	//���ش�СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free-list
	static void* refill(size_t n);	

	//����һ���ռ䣬������nobjs����СΪsize������
	//�������nobjs���������㣬nobjs���ܻή��
	static char* chunk_alloc(size_t size, int& nobjs);

	//Chunk allocation state
	static char* start_free;	//�ڴ���ʼλ�ã�ֻ��Chunk_alloc()�б仯
	static char* end_free;		//�ڴ����λ�ã�ֻ��Chunk_alloc()�б仯
	static size_t heap_size;	//�ѵĴ�С����ϵͳ������ռ�

public:
	static void* allocate(size_t n);	//�ռ����õĺ���
	static void* deallocate(void* p, size_t n);		//�ռ��ͷŵĺ���
	static void* reallocate(void* p, size_t old_sz, size_t new_sz);		//���·����ڴ�ĺ���

};	//class������

//������static data member�Ķ������ֵ�趨
template<bool threads, int inst>char* _default_alloc_template<threads, inst>::start_free = 0;
template<bool threads, int inst>char* _default_alloc_template<threads, inst>::end_free = 0;
template<bool threads, int inst>size_t* _default_alloc_template<threads, inst>::heap_size = 0;
template<bool threads, int inst> _default_alloc_template<threads, inst>::obj* volatile
_default_alloc_template<threads, inst>::free_list[_NFREELISTS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };

//�����Ƕ�����������allocate()������ʵ��
template<bool threads, int inst>void* _default_alloc_template::allocate(size_t n) {		//n����Ϊ0
	obj* volatile* my_free_list;
	obj* result;
	//���ж������С������128bytes�͵���һ��������
	if (n > (size_t)_MAX_BYTES) {
		return (malloc_alloc::allocate(n));
	}
	//С��128bytes��Ѱ��16��free-list���ʵ���һ��
	my_free_list = free_list + _FREELIST_INDEX(n);	//Ѱ��free-list���ʵ�������
	result = *my_free_list;			//�γ��ҵ�������
	if (result == 0) {		//���û�ҵ�����free-list
		void* r = refill(ROUND_UP(n));	//�������С�ϵ���8�����߽磬Ȼ�����refill()�������free-list
		return r;
	}
	*my_free_list = result->free_list_link;		//����free-list��ָ��ָ�򱻰γ����������һ������׵�ַ
	return(result);
}

//�����Ƕ�����������deallocate()������ʵ��
template<bool threads, int inst>void* _default_alloc_template::deallocate(void* p, size_t n) {	//p����Ϊ0
	obj* q = (obj*)p;		//��ָ���ָ��ĵ�ǰ�����ַ������Ҫ���յ�������׵�ַ
	obj* volatile* my_free_list;
	//���ж������С������128bytes�͵���һ��������
	if (n > (size_t)_MAX_BYTES) {
		malloc_alloc::deallocate(p,n);
		return; 
	}
	//С��128bytes���ҳ���Ӧ��free-list
	my_free_list = free_list + _FREELIST_INDEX(n);	
	//����free-list����������
	q->free_list_link = *my_free_list;
	*my_free_list = q;
}

//�����Ƕ�����������refill()��ʵ�֣���free list��û�п�������ʱ��refill()�������������ռ�
//�µĿռ�ȡ���ڴ��(����chunk_alloc()���)��ȱʡȡ��20�������飬���ڴ�ؿռ䲻��ʱ����õ�����������С��20
template<bool threads, int inst>void* _default_alloc_template::refill(size_t n) {	//����n�ѵ�����8�ı���
	int nobjs = 20;
	//����chunk_alloc()������ȡ��nobjs��������Ϊfree list���½ڵ�
	char* chunk = chunk_alloc(n, nobjs);	//ע��nobjs��pass by reference

	obj* volatile* my_free_list;
	obj* result;
	obj* current_obj, * next_obj;
	int i;
	
	if (1 = nobjs) return(chunk);	//���ֻ���һ�����飬���������������ã�free list���½ڵ�
	my_free_list =free_list+ _FREELIST_INDEX(n);	//����׼������free list�������½ڵ�

	//������chunk�ռ��ڽ���free list
	result = (obj*)chunk;	//��һ��׼�����ظ��ͻ���
	//��������free listָ�������õĿռ�(ȡ���ڴ��)
	*my_free_list = next_obj = (obj*)(chunk + n);
	//���½�free list�ĸ��ڵ㴮������
	for (i = 1;; i++) {		//��1��ʼ����Ϊ��0�������ظ��ͻ���
		current_obj = next_obj;
		next_obj = (obj*)((char*)next_obj + n));
		if (nobjs - 1 = i) {	//ֱ�����һ���ڵ㣬����ѭ��
			current_obj->free_list_link = 0;
			break;
		}
		else {
			current_obj->free_list_link = next_obj;	//һֱ������ȥ
		}
	}
	return(result);
}

//�����Ƕ�����������chunk_alloc()��ʵ�֣�������ڴ����ȡ���ռ��free listʹ�á�����size�ϵ���8�ı���
template<bool threads, int inst>char* _default_alloc_template::chunk_alloc(size_t size, int& nobjs) {
	char* result;
	size_t total_bytes = size * nobjs;				//�����ڴ�
	size_t bytes_left = end_free - start_free;		//�ڴ��ʣ��ռ�

	if (bytes_left >= total_bytes) {	//����ڴ��ʣ��ռ���ȫ����������
		result = start_free;			//�ڴ���ʼ��ַ����result
		start_free += total_bytes;		//����ʼ��ַ+ƫ�Ʒ���Ŀռ��С
		return (result);
	}
	else if (bytes_left >= size) {		//����ڴ��ʣ��ռ䲻����ȫ���������������㹻��Ӧһ��(��)���ϵ�����
		nobjs = bytes_left / size;		//����ɹ�Ӧ����������
		total_bytes = size * nobjs;		//�����ܹ���Ҫ������ٿռ�
		result = start_free;			//�ڴ���ʼ��ַ����result
		start_free += total_bytes;		//����ʼ��ַ+ƫ�Ʒ���Ŀռ��С
		return (result);
	}
	else {	//�ڴ��ʣ��ռ���һ������Ĵ�С�������ṩ
		//�µ��ڴ�ش�СΪ��������2����������һ���������ô������Ӷ���������ĸ�����
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		//�����������ڴ���еĲ�����ͷ�������ü�ֵ
		if (bytes_left > 0) {	//�ڴ���ڻ���һЩ��ͷ���ȷ�����ʵ���free list
			obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);	//Ѱ���ʵ���free list
			//����free list�����ڴ���еĲ���ռ����
			((obj*)start_free)->free_list_link = *my_free_list;	//����ʼ��ַ����һ����ָ��my_free_list
			*my_free_list = (obj*)start_free;
		}
		//��������heap�ռ䣬���������ڴ��
		start_free = (char*)malloc(bytes_to_get);	//�����ڴ�صĴ�С
		if (0 == start_free) {		//���heap�ռ䲻�㣬malloc()ʧ��
			int i;
			obj* volatile* my_free_list, * p;
			//������Ѱ�ʵ���free list��������δ�����飬�����鹻���free list
			for (i = size; i <= _MAX_BYTES; i += _ALIGN) {
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (0 != p) {	//���free list����δ������
					//����free list���ͷų�δ������
					*my_free_list = p - > free_list_link;
					start_free = (char*)p;
					end_free = start_free + i;
					//�ݹ�����Լ���Ϊ������nobjs
					return(chunk_alloc(size, nobjs));
					//ע�⣺�κβ�����ͷ�ս��������ʵ���free list�б���
				}
			}
			end_free = 0;	//���һ���ڴ涼û
			//���õ�һ��������������out-of-memory�����ܷ񾡵���
			//��ᵼ���׳�bad_alloc�쳣�����ڴ治�������õ�����
			start_free = (char*)malloc_alloc::allocate(bytes_to_get);
			
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		//�ݹ�����Լ���Ϊ������nobjs
		return(chunk_alloc(size, nobjs));
	}
}

}					//���������ռ�mystl


#endif // !MYSTL_ALLOC_H_

