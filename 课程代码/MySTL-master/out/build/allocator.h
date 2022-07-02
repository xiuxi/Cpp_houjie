#ifdef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_
//这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构
#include<new>		//for placement new
//头文件<stddef>、<cstdlib>、<cstring>与其C对应版本兼容，在C++程序中经常用到。它们是C头文件<stddef.h>、<stdlib.h>、
//<string.h>的较新版本，定义了一些常用的常量、宏、类型和函数
#include<cstddef>	//for ptrdiff_f、size_t
#include<cstdlib>	//for exit()
#include<climits>	//for UINT_MAX
#include<iostream>	//for cerr

#include "construct.h"		//负责对象的构造
#include "util.h"			//通用工具

namespace mystl		//命名空间mystl
{
//模板类：allocator
// 模板函数代表数据类型
template <class T> class allocator {		//class T相当于typename T，为避免旧式编译器不支持typename T，使用class T
	//根据STL规范写出的allocator的必要接口
public:
	//以下是各种type的设计
	typedef T	value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T&	reference;
	typedef const T& const_reference;
	typedef size_t	size_type;
	typedef ptrdiff_t	difference_type;

public:
	//new算式分为两阶段：(1)分配内存；(2)构造对象
	//delete算式分为两阶段：(1)析构对象；(2)释放内存
	static T*	allocate();	//内存分配函数
	static T* allocate(size_type n);

	static void deallocate(T* ptr);	//内存释放函数
	static void deallocate(T* ptr, size_type n);
	//构造函数可同时存在多个版本
	static void construct(T* ptr);	//对象内容的构造函数
	static void construct(T* ptr, const T& value);	//拷贝构造函数
	static void construct(T* ptr, const T&& value);	//移动构造函数
	template<class...Args> static void construct(T* ptr, Arg&& ...args); //可变参数模板版本
	
	static void destroy(T* ptr);	//对象内容的析构函数
	static void destroy(T* first, T* last);
	};
	
template<class T> T* allocator<T>::allocate() {	
	return static_cast<T*>(::operator new(sizeof(T)));	//调用::operator new分配内存
	}

template<class T>T* allocator<T>::allocate(size_type n) {
	if (n == 0)
		return nullptr;
	return static_cast<T*>(::operaotr new(sizeof(T) * n));	//调用::operator new分配内存
}

template<class T>void allocator<T>::deallocate(T* ptr) {
	if (ptr == nullptr)
		return ;
	::operator delete(ptr);			//调用::operator delete释放内存
}

template<class T>void allocator<T>::deallocate(T* ptr, size_type n) {
	if (ptr == nullptr)
		return;
	::operator delete(ptr);			//调用::operator delete释放内存
}

template<class T>void allocator::construct(T* ptr) {
	mystl::construct(ptr);			//调用construct.h的构造函数去构造对象
}

template<class T>void allocator::construct(T* prt, const T& value) {
	mystl::construct(ptr, value);	//调用construct.h的拷贝构造函数去构造对象
}

template<class T>void allocator::construct(T* ptr, const T&& value) {
	mystl::construct(ptr, mystl::move(value));	//调用construct.h的移动构造函数去构造对象
}

template<class T>void allocator::construct(T* ptr, Arg&& ...args) {
	//使用forword函数实现参数的完美转发，因为函数参数会在编译中自动变为左值，需要用forward保持参数的右值属性
	mystl::construct(ptr, mystl::forword<Args>(args)...);	
}

template<class T>void allocator::destroy(T* ptr) {
	mystl::destroy(ptr);			//调用construct.h的析构函数去析构对象
}

template<class T>void allocator::destroy(T* first, T* last) {
	mystl::destroy(first, last);	//调用construct.h的析构函数去析构对象
}

}	//命名空间结束

#endif


