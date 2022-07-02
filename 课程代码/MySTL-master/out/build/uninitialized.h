#ifndef MYSTL_UNINITALIZED_H_
#define MYSTL_UNINITALIZED_H_

// 这个头文件用于对未初始化空间构造元素

#include "algobase.h"			//基本算法
#include "construct.h"			//构造对象
#include "iterator.h"			//迭代器
#include "type_traits.h"		//萃取机
#include "util.h"				//通用工具

namespace mystl {	//命名空间mystl

/*****************************************************************************************/
// uninitialized_copy
// 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
//对应于高层次的copy函数
/*****************************************************************************************/

template <class InputIter, class ForwardIter>ForwardIter unchecked_uninit_copy
(InputIter first, InputIter last, ForwardIter result, std::true_type) {		
	return mystl::copy(first, last, result);	//true_type和false_type可根据类型决定不同模板特化
	//参考：https://stackoverflow.com/questions/58694521/what-is-stdfalse-type-or-stdtrue-type
}

template <class InputIter, class ForwardIter>ForwardIter unchecked_uninit_copy
(InputIter first, InputIter last, ForwardIter result, std::false_type) {
	auto cur = result;
	try {
		for (; first != last; ++first, ++cur) {
			mystl::construct(&*cur, *first);	//构造元素，&*cur指的是cur所指的对象的引用，by reference
		}
	}
	catch (...) {
		for (; result != cur; --cur)
			mystl::destroy(&*cur);				//配置失败就销毁元素
	}
	return cur;
}

//uninitialized_copy的接口
template <class InputIter, class ForwardIter>ForwardIter uninitialized_copy
(InputIter first, InputIter last, ForwardIter result) {
	return mystl::unchecked_uninit_copy(first, last, result,
		std::is_trivially_copy_assignable<
		typename iterator_traits<ForwardIter>::
		value_type>{});
}

/*****************************************************************************************/
// uninitialized_copy_n
// 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/*****************************************************************************************/
template <class InputIter, class size, class ForwardIter>ForwardIter unchecked_uninit_copy_n
(InputIter first, Size n, ForwardIter result, std::true_type) {
	return mystl::copy(first, n, result).second;	//.second表示保存第二数据
}

template <class InputIter, class Size, class ForwardIter>ForwardIter unchecked_uninit_copy_n
(InputIter first, Size n, ForwardIter result, std::false_type) {
	auto cur = result;
	try {
		for (; n > 0; --n, ++cur, ++first) {
			mystl::construct(&*cur, *first);
		}
	}
	catch (...) {
		for (; result != cur; --cur)
			mystl::destroy(&*cur);
	}
	return cur;
}

// uninitialized_copy_n的接口
template <class InputIter, class Size, class ForwardIter>ForwardIter uninitialized_copy_n
(InputIter first, Size n, ForwardIter result) {
	return mystl::unchecked_uninit_copy_n(first, n, result,
		std::is_trivially_copy_assignable<
		typename iterator_traits<InputIter>::
		value_type>{});
}

/*****************************************************************************************/
// uninitialized_fill
// 在 [first, last) 区间内填充元素值
/*****************************************************************************************/
template <class ForwardIter, class T>void unchecked_uninit_fill
(ForwardIter first, ForwardIter last, const T& value, std::true_type) {
	mystl::fill(first, last, value);
}

template <class ForwardIter, class T>void unchecked_uninit_fill
(ForwardIter first, ForwardIter last, const T& value, std::false_type) {
	auto cur = first;
	try {
		for (; cur != last; ++cur) {
			mystl::construct(&*cur, value);
		}
	}
	catch (...) {
		for (; first != cur; ++first)
			mystl::destroy(&*first);
	}
}

//uninitialized_fill的接口
template <class ForwardIter, class T>
void  uninitialized_fill(ForwardIter first, ForwardIter last, const T& value)
{
	mystl::unchecked_uninit_fill(first, last, value,
		std::is_trivially_copy_assignable<
		typename iterator_traits<ForwardIter>::
		value_type>{});
}

/*****************************************************************************************/
// uninitialized_fill_n
// 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
/*****************************************************************************************/
template <class ForwardIter, class Size, class T>ForwardIter unchecked_uninit_fill_n
(ForwardIter first, Size n, const T& value, std::true_type) {
	return mystl::fill(first, n, value);
}

template <class ForwardIter, class Size, class T>ForwardIter unchecked_uninit_fill_n
(ForwardIter first, Size n, const T& value, std::false_type) {
	auto cur = first;
	try {
		for (; n > 0; --n, ++cur) {
			mystl::construct(&*cur, value);
		}
	}
	catch (...) {
		for (; first != cur; ++first)
			mystl::destroy(&*first);
	}
	return cur;
}

//uninitialized_fill_n的接口
template <class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value)
{
	return mystl::unchecked_uninit_fill_n(first, n, value,
		std::is_trivially_copy_assignable<
		typename iterator_traits<ForwardIter>::
		value_type>{});
}

/*****************************************************************************************/
// uninitialized_move
// 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*****************************************************************************************/
template <class InputIter, class ForwardIter>ForwardIter unchecked_uninit_move
(InputIter first, InputIter last, ForwardIter result, std::true_type) {
	return mystl::move(first, last, result);
}

template <class InputIter, class ForwardIter>ForwardIter unchecked_uninit_move
(InputIter first, InputIter last, ForwardIter result, std::false_type) {
	ForwardIter cur = result;
	try {
		for (; first != last; ++first, ++cur) {
			mystl::construct(&*cur, mystl::move(*first));
		}
	}
	catch (...) {
		mystl::destroy(result, cur);
	}
	return cur;
}

//uninitialized_move的接口
template <class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
{
	return mystl::unchecked_uninit_move(first, last, result,
		std::is_trivially_move_assignable<
		typename iterator_traits<InputIter>::
		value_type>{});
}

/*****************************************************************************************/
// uninitialized_move_n
// 把[first, first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*****************************************************************************************/
template <class InputIter, class Size, class ForwardIter>ForwardIter unchecked_uninit_move_n
(InputIter first,Size n,ForwardIter result,std::true_type){
	return mystl::move(first, first + n, result);
}

template <class InputIter, class Size, class ForwardIter>ForwardIter unchecked_uninit_move_n
(InputIter first, Size n, ForwardIter result, std::false_type) {
	auto cur = result;
	try{
		for (; n > 0; --n,++first,++cur) {
			mystl::construct(&*cur, std:; move(*first));
		}
	}
	catch (...) {
		for (; result != cur; ++result)
			mystl::destroy(&*result);
		throw;
	}
	return cur;
}

//uninitialized_move_n的接口
template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
{
	return mystl::unchecked_uninit_move_n(first, n, result,
		std::is_trivially_move_assignable<
		typename iterator_traits<InputIter>::
		value_type>{});
}

}		//结束命名空间mystl

#endif // !MYSTL_UNINITALIZED_H_

