#ifdef MYSTL_CONSTRUCT_H_
#define MYSTL_CONSTRUCT_H_

//这个头文件包含两个函数 construct，destroy
//construct：负责对象的构造	destroy：负责对象的析构

#include<new>	//包含placement new的头文件

#include "type_traits.h"		//萃取机
#include "iterator.h"			//迭代器

//以下是关闭编译头文件时产生的警告
#ifdef _MSC_VER					//MSCV是微软的VC运行库，即编译器
#pragma warning(push)		   //#pragma指令可以对指定的区域的代码关闭指定的warning
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER

namespace mystl {	//命名空间mystl

//construct构造对象
template<class T>void construct(T* prt) {
	::new((void*)ptr) T();		//placement new,调用T::T()进行默认构造
}

template<class T1, class T2>void construct(T1* ptr,const T2& value){
	::new((void*)ptr) T1(value);	//placement new,调用T::T(value)将初值设定到指针所指的空间上
}

template<class T, class...Args>void construct(T* ptr, Args&&...args) {	//可变参数模板设定右值参数
	::new((void*)ptr) T(mystl::forward<Args>(args)...);					//forward函数保证参数的完美转发
}

//destroy将对象析构
//首先要判断元素的数值型别(value type)，再利用_type_traits<T>判断该型别是否有trivial destructor(无关痛痒的析构函数)
//若是(_true_type_)，则什么也不做就结束
//若否(_false_type_)，以循环方式巡访整个范围，并在循环中每经历一个对象就调用接受指针版本的析构函数

//接受指针版本
//如果元素的数值型别(value type)有trivial destructor
template <class T>void destroy_one(T*, std::true_type) {}		

//如果元素的数值型别(value type)有non-trivial destructor
template <class T>void destroy_one(T* pointer, std::false_type){
	if (pointer != nullptr)
	{
		pointer->~T();
	}
}

//接受迭代器版本
//如果元素的数值型别(value type)有trivial destructor
template <class ForwardIter>void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

//如果元素的数值型别(value type)有non-trivial destructor
template <class ForwardIter>void destroy_cat(ForwardIter first, ForwardIter last, std::false_type){
	for (; first != last; ++first)
		destroy(&*first);
}

//destroy()的第一版本，接受一个指针.此函数设法找出元素的数值型别，进而利用_type_traits<>求取最适当措施
template<class T>void destroy(T* pointer) {			
	destroy_one(pointer, std::is_trivially_destructible<T>{});
}
//以下是destroy的第二版本，接受两个迭代器。此函数设法找出元素的数值型别，进而利用_type_traits<>求取最适当措施
template<class ForwardIter>void destroy(ForwordIter first, ForwordIter last) {	
	destory_cat(first, last, std::is_trivially_destructible <typename iterator_traits<ForwardIter>::value_type>{});
}

}	//结束命名空间mystl

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // MYSTL_CONSTRUCT_H_

