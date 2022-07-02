#ifndef MYSTL_ALLOC_H_
#define MYSTL_ALLOC_H_

//这个头文件包含一个类 alloc，用于分配和回收内存，以内存池的方式实现
//考虑到小型区块可能造成的内存碎片问题，设计双层级配置器

#include<new>			
#include<cstddef>		//定义了一些常用的常量、宏、类型和函数
#include<cstdio>		//提供基本的文字的输入输出流操作

namespace mystl {	//命名空间mystl
//无论是一级或二级配置器，都需要提供一个接口，使配置器能够符合STL规格
//以下是配置器的接口,
template<class T, class Alloc>class simple_alloc {
public:
		//使用这些成员函数将调用传递给配置器的成员函数，使配置器的配置单位从bytes转为个别元素的大小(sizeof(T))
	static T* allocate(size_t n) { return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T)); }
	static T* allocate(void) { return (T*)Alloc::allocate(sizeof(T)); }
	static void deallocate(T* p, size_t n) { if (0 != n) Alloc::deallocate(p, n * sizeof(T)); }
	static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }
	};

//以下是一级配置器,直接使用malloc()和free()
template<int inst>class _malloc_alloc_template {	//alloc不接受任何template型别参数
private:
//以下都是函数指针，所代表的函数将用来处理内存不足的情况
	static void* oom_malloc(size_t n);
	static void* oom_realloc(void*, size_t);
	static void* (*_malloc_alloc_oom_handler)();

public:
	static void* allocate(size_t n) {		//分配内存的函数
	void* result = malloc(n);			//第一级配置器直接使用malloc()
	if (0 == result)	result = oom_malloc(n);		//无法满足内存需求时，调用oom_malloc()
	return result;
}

static void deallocate(void* p, size_t n) {		//释放内存的函数
	free(p);							//第一级配置器直接使用free()
}

static void* reallocate(void* p, size_t old_sz, size_t new_sz) {	//重新调整已分配内存的函数
	void* result = realloc(p, new_sz);
		if (0 == result) result = oom_realloc(p, new_sz);	//无法满足内存需求时，调用oom_realloc()
		return result;
}

//以malloc而非::operator new来配置内存(原因是1.历史元素，2.C++未提供相应于realloc()的内存配置操作)
//所以不能直接使用C++的set_new_handler()，必须自己设定一个仿真的set_new_handler()
//以下是仿真C++的set_new_handler()，你可以通过它指定你自己的out-of-memory handler
	static void(*set_malloc_handler(void(*f)()))(){		//f指针指向new-handler
	void(*old)() = _malloc_alloc_oom_handler;		//卸载old-handler
	_malloc_alloc_oom_handler = f;					//安装new-handler
	return(old);
	}
};

//malloc_alloc out-of-memory handler,初值为0，有待客户端设定
template<int inst>void (*_malloc_alloc_template<inst>::_malloc_alloc_oom_handler)() = 0;

//以下是处理内存不足的函数定义
template <int inst>void* _malloc_alloc_template<inst>::oom_malloc(size_t n) {
	void(*my_malloc_handler)();
	void* result;
	for (;;) {						//不断尝试释放、配置、再释放、再配置...
		my_malloc_handler = _malloc_alloc_oom_handler;
		if (0 == my_malloc_handler) { __THROW_RAD_ALLOC; }
		(*my_malloc_handler)();		//调用处理例程，企图释放内存
		result = malloc(n);			//再次尝试配置内存
		if (result) return(result);
	}
}

template <int inst>void* _malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
	void(*my_malloc_handler)();
	void* result;
	for (;;) {						//不断尝试释放、配置、再释放、再配置...
		my_malloc_handler = _malloc_alloc_oom_handler;
		//如果客户端未设定内存不足处理例程，调用__THROW_RAD_ALLOC丢出bad_alloc异常信息
		if (0 == my_malloc_handler) { __THROW_RAD_ALLOC; }
		(*my_malloc_handler)();		//调用处理例程，企图释放内存
		result = malloc(n);			//再次尝试配置内存
		if (result) return(result);
	}
}

typedef _malloc_alloc_template<0> malloc_alloc;	//直接将参数inst指定为0

//二级配置器：如果区块超过128bytes，移交第一级配置器处理；如果区块小于128bytes，以内存池管理
//内存池每次配置一大块内存，维护相应的free-list，有内存需求就从free-list中取出；
//如果客户端释放内存块，就回收到free-list
//二级配置器将任何区块的内存需求上调至8的倍数，并将128bytes按每8bytes分成16个free-list


enum { _ALIGN = 8 };	//小型区块的上调边界,每块最多8bytes
enum { _MAX_BYTES = 128 };	//小型区块的上限
enum { _NFREELISTS = _MAX_BYTES / _ALIGN };	//free-list的个数

//以下是二级配置器
template<bool threads, int inst>class _default_alloc_template {
private:
	static size_t ROUND_UP(size_t bytes) {		//ROUND_UP将bytes上调至8的倍数
		return (((bytes)+_ALIGN - 1) & ~(_ALIGN - 1));
	}

	//free-list的节点结构
	union obj {				//union关键字定义共用体，内存可被共享，所以obj可被视作一个指针，指向相同形式的另一个obj
		union obj* free_list_link;	//指向下一个区块
		char client_data[1];		//存储本快内存的首地址
	};

	//volatile 关键字是一种类型修饰符，用它声明的类型变量表示可以被某些编译器未知的因素更改。 
	//比如：操作系统、硬件或者其它线程等。遇到这个关键字声明的变量，编译器对访问该变量的代码就不再进行优化，
	//从而可以提供对特殊地址的稳定访问
	static obj* volatile free_list[_NFREELISTS];	//16个自由链表

	static size_t FREELIST_INDEX(size_t bytes) {	//根据区块大小，决定使用第n号free-list。n从1起算
		return (((bytes)+_ALIGN - 1) / _ALIGN - 1);	
	}

	//返回大小为n的对象，并可能加入大小为n的其他区块到free-list
	static void* refill(size_t n);	

	//配置一大块空间，可容纳nobjs个大小为size的区块
	//如果配置nobjs个有所不便，nobjs可能会降低
	static char* chunk_alloc(size_t size, int& nobjs);

	//Chunk allocation state
	static char* start_free;	//内存起始位置，只在Chunk_alloc()中变化
	static char* end_free;		//内存结束位置，只在Chunk_alloc()中变化
	static size_t heap_size;	//堆的大小，向系统堆申请空间

public:
	static void* allocate(size_t n);	//空间配置的函数
	static void* deallocate(void* p, size_t n);		//空间释放的函数
	static void* reallocate(void* p, size_t old_sz, size_t new_sz);		//重新分配内存的函数

};	//class结束符

//以下是static data member的定义与初值设定
template<bool threads, int inst>char* _default_alloc_template<threads, inst>::start_free = 0;
template<bool threads, int inst>char* _default_alloc_template<threads, inst>::end_free = 0;
template<bool threads, int inst>size_t* _default_alloc_template<threads, inst>::heap_size = 0;
template<bool threads, int inst> _default_alloc_template<threads, inst>::obj* volatile
_default_alloc_template<threads, inst>::free_list[_NFREELISTS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };

//以下是二级配置器的allocate()函数的实现
template<bool threads, int inst>void* _default_alloc_template::allocate(size_t n) {		//n不可为0
	obj* volatile* my_free_list;
	obj* result;
	//先判断区块大小，大于128bytes就调用一级配置器
	if (n > (size_t)_MAX_BYTES) {
		return (malloc_alloc::allocate(n));
	}
	//小于128bytes就寻找16个free-list中适当的一个
	my_free_list = free_list + _FREELIST_INDEX(n);	//寻找free-list中适当的区块
	result = *my_free_list;			//拔出找到的区块
	if (result == 0) {		//如果没找到可用free-list
		void* r = refill(ROUND_UP(n));	//将区块大小上调至8倍数边界，然后调用refill()重新填充free-list
		return r;
	}
	*my_free_list = result->free_list_link;		//调整free-list，指针指向被拔出的区块的下一区块的首地址
	return(result);
}

//以下是二级配置器的deallocate()函数的实现
template<bool threads, int inst>void* _default_alloc_template::deallocate(void* p, size_t n) {	//p不可为0
	obj* q = (obj*)p;		//将指针从指向的当前区块地址调整到要回收的区块的首地址
	obj* volatile* my_free_list;
	//先判断区块大小，大于128bytes就调用一级配置器
	if (n > (size_t)_MAX_BYTES) {
		malloc_alloc::deallocate(p,n);
		return; 
	}
	//小于128bytes就找出对应的free-list
	my_free_list = free_list + _FREELIST_INDEX(n);	
	//调整free-list，回收区块
	q->free_list_link = *my_free_list;
	*my_free_list = q;
}

//以下是二级配置器的refill()的实现，当free list中没有可用区块时，refill()可用于重新填充空间
//新的空间取自内存池(经由chunk_alloc()完成)。缺省取得20个新区块，当内存池空间不足时，获得的区块数可能小于20
template<bool threads, int inst>void* _default_alloc_template::refill(size_t n) {	//假设n已调整至8的倍数
	int nobjs = 20;
	//调用chunk_alloc()，尝试取得nobjs个区块作为free list的新节点
	char* chunk = chunk_alloc(n, nobjs);	//注意nobjs是pass by reference

	obj* volatile* my_free_list;
	obj* result;
	obj* current_obj, * next_obj;
	int i;
	
	if (1 = nobjs) return(chunk);	//如果只获得一个区块，将其分配给调用者用，free list无新节点
	my_free_list =free_list+ _FREELIST_INDEX(n);	//否则准备调整free list，纳入新节点

	//以下在chunk空间内建立free list
	result = (obj*)chunk;	//这一块准备返回给客户端
	//以下引导free list指向新配置的空间(取自内存池)
	*my_free_list = next_obj = (obj*)(chunk + n);
	//以下将free list的各节点串联起来
	for (i = 1;; i++) {		//从1开始，因为第0个将返回给客户端
		current_obj = next_obj;
		next_obj = (obj*)((char*)next_obj + n));
		if (nobjs - 1 = i) {	//直到最后一个节点，跳出循环
			current_obj->free_list_link = 0;
			break;
		}
		else {
			current_obj->free_list_link = next_obj;	//一直串联下去
		}
	}
	return(result);
}

//以下是二级配置器的chunk_alloc()的实现，负责从内存池中取出空间给free list使用。假设size上调至8的倍数
template<bool threads, int inst>char* _default_alloc_template::chunk_alloc(size_t size, int& nobjs) {
	char* result;
	size_t total_bytes = size * nobjs;				//所需内存
	size_t bytes_left = end_free - start_free;		//内存池剩余空间

	if (bytes_left >= total_bytes) {	//如果内存池剩余空间完全满足需求量
		result = start_free;			//内存起始地址给到result
		start_free += total_bytes;		//从起始地址+偏移分配的空间大小
		return (result);
	}
	else if (bytes_left >= size) {		//如果内存池剩余空间不能完全满足需求量，但足够供应一个(含)以上的区块
		nobjs = bytes_left / size;		//求出可供应的区块数量
		total_bytes = size * nobjs;		//计算总共需要分配多少空间
		result = start_free;			//内存起始地址给到result
		start_free += total_bytes;		//从起始地址+偏移分配的空间大小
		return (result);
	}
	else {	//内存池剩余空间连一个区块的大小都不能提供
		//新的内存池大小为需求量的2倍，并加上一个随着配置次数增加而愈来愈大的附加量
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		//以下试着让内存池中的残余零头还有利用价值
		if (bytes_left > 0) {	//内存池内还有一些零头，先分配给适当的free list
			obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);	//寻找适当的free list
			//调整free list，将内存池中的残余空间编入
			((obj*)start_free)->free_list_link = *my_free_list;	//将起始地址的下一区块指向my_free_list
			*my_free_list = (obj*)start_free;
		}
		//以下配置heap空间，用来补充内存池
		start_free = (char*)malloc(bytes_to_get);	//分配内存池的大小
		if (0 == start_free) {		//如果heap空间不足，malloc()失败
			int i;
			obj* volatile* my_free_list, * p;
			//以下搜寻适当的free list，即尚有未用区块，且区块够大的free list
			for (i = size; i <= _MAX_BYTES; i += _ALIGN) {
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (0 != p) {	//如果free list尚有未用区块
					//调整free list以释放出未用区块
					*my_free_list = p - > free_list_link;
					start_free = (char*)p;
					end_free = start_free + i;
					//递归调用自己，为了修正nobjs
					return(chunk_alloc(size, nobjs));
					//注意：任何残余零头终将被编入适当的free list中备用
				}
			}
			end_free = 0;	//如果一点内存都没
			//调用第一级配置器，看看out-of-memory机制能否尽点力
			//这会导致抛出bad_alloc异常，或内存不足的情况得到改善
			start_free = (char*)malloc_alloc::allocate(bytes_to_get);
			
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		//递归调用自己，为了修正nobjs
		return(chunk_alloc(size, nobjs));
	}
}

}					//结束命名空间mystl


#endif // !MYSTL_ALLOC_H_

