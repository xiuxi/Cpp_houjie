#ifndef MYSTL_ALGOBASE_H_
#define MYSTL_ALGOBASE_H_

// 这个头文件包含了 mystl 的基本算法

#include <cstring>				//包含处理char*字符串的库函数

#include "iterator.h"			//迭代器
#include "util.h"				//通用工具

namespace mystl {

#ifdef max
//预处理指令：if not define 如果未定义则编译下面代码
#pragma message("#undefing marco max")
// 打印：取消宏定义
#undef max
#endif // max
//当忘记了某个关键的宏是否正确的设置了。通过if...判断->message "打印"
//参考:https://www.jianshu.com/p/e0caa3beb416
#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

/*****************************************************************************************/
// equal：如果两个序列在[first,last)内相等，返回true
// 比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
//如果第二序列的元素比较多，多出来的元素不予考虑
//如果希望保证两个序列完全相等，在调用equal()时必须先判断元素个数是否相同
/*****************************************************************************************/
//版本1
template <class InputIter1, class InputIter2>bool equal
(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
	for (; first1 != last1; ++first1, ++first2) {
		if (*first1 != *first2) {	//对应元素不相等
			return false;	//返回false
		}	
		return true;	//否则返回true
	}
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compared>bool equal
(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!comp(*first1, *first2))
			return false;
	}
	return true;
}

/*****************************************************************************************/
// fill_n：从 first 位置开始填充 n 个值，返回的迭代器指向被填入的最后一个元素的下一位置
/*****************************************************************************************/
//版本1：泛化
template <class OutputIter, class Size, class T>OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value) {
	for (; n > 0; --n, ++first) {	//经过n个元素
		*first = value;				//设定新值
	}
	return first;
}

//版本2：为 one-byte 类型提供特化版本
//std::enable_if：满足条件时类型有效，利用SFINAE根据类型特征有条件地从重载解析中移动函数，
// 并为不同的类型特征提供单独的函数重载和特化
template <class Tp,class Size,class Up>typename std::enable_if<	
	std::is_integral<Tp>::value && sizeof(Tp) == 1 &&	//std::is_integral检查T是否是整数类型
	!std::is_same<Tp, bool>::value &&					//std::is_same判断类型是否一致
	std::is_integral<Up>::value && sizeof(Up) == 1,
	Tp*>::type
	unchecked_fill_n(Tp* first, Size n, Up value) {
	if (n > 0) {
		std::memset(first, (unsigned char)value, (size_t)(n));	
		//std::memset：char型初始化函数
		//将第一参数所指向的某一块内存中的前n个字节的内容全部设置为第二参数指定的ASCII值， 
		//第一个值为指定的内存地址，第二参数为指定的值，块的大小由第三个参数指定，
		//这个函数通常为新申请的内存做初始化工作， 其返回值为指向第一参数的指针，
		//它是对较大的结构体或数组进行清零操作的一种最快方法
		//参考：https://www.cnblogs.com/Tang-tangt/p/9342103.html
	}
	return first + n;
}

//fill_n的接口
template <class OutputIter, class Size, class T>OutputIter fill_n(OutputIter first, Size n, const T& value) {
	return unchecked_fill_n(first, n, value);
}

/*****************************************************************************************/
// fill: 为 [first, last)区间内的所有元素填充新值
/*****************************************************************************************/
//forward_iterator版本
template <class ForwardIter, class T>void fill_cat
(ForwardIter first, ForwardIter last, const T& value, mystl::forward_iterator_tag) {
	for (; first != last; ++first) {
		*first = value;		//设定新值
	}
}

//random_iterator版本
template <class RandomIter, class T>void fill_cat
(RandomIter first, RandomIter last, const T& value, mystl::random_access_iterator_tag) {
	fill_n(first, last - first, value);	//调用fill_n()
}

//fill的接口
template <class ForwardIter, class T>void fill(ForwardIter first, ForwardIter last, const T& value) {
	fill_cat(first, last, value, iterator_category(first));	//判断迭代器类型，调用不同fill()实现
}

/*****************************************************************************************/
// iter_swap
// 将两个迭代器所指对象对调
/*****************************************************************************************/
template <class FIter1, class FIter2> void iter_swap(FIter1 lhs, FIter2 rhs) {
	mystl::swap(*lhs, *rhs);
}

/*****************************************************************************************/
// swap
// 用来交换两个对象的内容
/*****************************************************************************************/
template<class T>void swap(T& a, T& b) {
	T tmp = a;
	a = b;
	b = tmp;
}


/*****************************************************************************************/
// lexicographical_compare
// 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
// (1)如果第一序列的元素较小，返回 true ，否则返回 false
// (2)如果到达 last1 而尚未到达 last2 返回 true
// (3)如果到达 last2 而尚未到达 last1 返回 false
// (4)如果同时到达 last1 和 last2 返回 false，即第一序列以字典排列方式而言不小于第二序列
/*****************************************************************************************/
//版本1
template <class InputIter1, class InputIter2> bool lexicographical_compare
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2) {
	//以下，任何一个序列到达尾端，就结束。否则两序列就相应元素一一进行比对
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (*first1 < *first2)		//第一序列元素小于第二序列的相应元素值
			return true;
		if (*first2 < *first1)		//第二序列元素小于第一序列的相应元素值
			return false;
		//如果不符合以上两条件，表示两值相等，就进行下一组相应元素值的对比
	}
	//进行到这里，如果第一序列到达尾端而第二序列尚有余额，那么第一序列小于第二序列
	return first1 == last1 && first2 != last2;
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compred> bool lexicographical_compare
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, Compred comp) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (comp(*first1, *first2))
			return true;
		if (comp(*first2, *first1))
			return false;
	}
	return first1 == last1 && first2 != last2;
}

//版本3：针对 const unsigned char* 的特化版本
bool lexicographical_compare(const unsigned char* first1,
	const unsigned char* last1,
	const unsigned char* first2,
	const unsigned char* last2) {
	const auto len1 = last1 - first1;
	const auto len2 = last2 - first2;
	// 先比较相同长度的部分
	////std::memcmp是C标准函数，以unsigned char方式来比较两个序列中一一对应的每个bytes
	const auto result = std::memcmp(first1, first2, mystl::min(len1, len2));	
	// 若相等，长度较长的被视为比较大
	return result != 0 ? result < 0 : len1 < len2;
}

/*****************************************************************************************/
// max
// 取二者中的较大值，语义相等时保证返回第一个参数
/*****************************************************************************************/
//版本1
template <class T> const T& max(const T& lhs, const T& rhs) {
	return lhs < rhs ? rhs : lhs;
}

//版本2：使用函数对象 comp 代替比较操作
template <class T, class Compare>const T& max(const T& lhs, const T& rhs, Compare comp) {
	return comp(lhs, rhs) ? rhs : lhs;
}

/*****************************************************************************************/
// min 
// 取二者中的较小值，语义相等时保证返回第一个参数
/*****************************************************************************************/
//版本1
template <class T>const T& min(const T& lhs, const T& rhs) {
	return rhs < lhs ? rhs : lhs;
}

//版本2：使用函数对象 comp 代替比较操作
template <class T, class Compare>const T& min(const T& lhs, const T& rhs, Compare comp) {
	return comp(rhs, lhs) ? rhs : lhs;
}

/*****************************************************************************************/
// mismatch
// 平行比较两个序列，找到第一处不匹配的元素，返回一对迭代器，分别指向两个序列中不匹配的元素
/*****************************************************************************************/
//版本1
template <class InputIter1,class InputIter2>mtstl::pair<InputIter1, InputIter2>mismatch
(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
	//如果第一序列走完，就结束
	//如果第一序列和第二序列对应的元素相等，就结束
	while (first1!last1&&*first1==*first2)
	{
		++first1;
		++first2;
	}
	return mystl::pair<InputIter1, InputIter2>(first1, first2);
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1,class InputIter2,class Compare>mystl::pair<InputIter1,InputIter2>
	mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compare comp) {
		while (first1 != last1 && comp(*first1,*first2)) {
			++first1;
			++first2;
		}
		return mystl::pair<InputIter1, InputIter2>(first1, first2);
}

/*****************************************************************************************/
// copy
// 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
/*****************************************************************************************/
// input_iterator_tag 版本
template <class InputIter, class OutputIter>OutputIter unchecked_copy_cat
(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag) {
	//以迭代器等同与否，决定循环是否继续。速度慢
	for (; first != last; ++first, ++result) {
		*result = *first;
	}
	return result;
}

// ramdom_access_iterator_tag 版本
template <class RandomIter, class OutputIter> OutputIter unchecked_copy_cat
(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag) {
	//以n决定循环的执行次数。速度极快
	for (auto n = last - first; n > 0; --n, ++first, ++result) {
		*result = *first;
	}
	return result;
}

//unchecked_copy函数的接口
template <class InputIter, class OutputIter> OutputIter unchecked_copy
(InputIter first, InputIter last, OutputIter result) {
	return unchecked_copy_cat(first, last, result, mystl::iterator_category(first));	//判断迭代器类型
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>typename std::enable_if <
	std::is_same<typename std::remove_const<Tp>::type,Up>::value &&
	std::is_trivially_copy_assignable<Up>::value,
	Up*>::type
	unchecked_copy(Tp* first, Tp* last, Up* result) {
	const auto n = static_cast<size_t>(last - first);
	if (n != 0)
		std::memmove(result, first, n * sizeof(Up));	//std::memmove:最快速的内存对拷函数
	return result + n;
}

//copy的接口
template <class InputIter, class OutputIter>OutputIter copy(InputIter first, InputIter last, OutputIter result) {
	return unchecked_copy(first, last, result);
}

/*****************************************************************************************/
// copy_backward：以逆行方向拷贝
// 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
/*****************************************************************************************/
//bidirectional_iterator_tag 版本
template <class BidirectionalIter1,class BidirectionalIter2>BidirectionalIter2 
unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result, mystl::bidirectional_iterator_tag) {
	//以迭代器等同与否，决定循环是否继续。速度慢
	while (first!=last)
	{
		*--result = *--last;	//逆向拷贝
	}
	return result;
}

//random_access_iterator_tag 版本
template <class BidirectionalIter1, class BidirectionalIter2>BidirectionalIter2 
unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result, mystl::random_access_iterator_tag) {
	//以n决定循环的执行次数。速度极快
	for (auto n = last - firs; n > 0; --n)
		*--result = *--last;
	return result;
}

//unchecked_copy_backward的接口
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result)
{
	return unchecked_copy_backward_cat(first, last, result,
		iterator_category(first));		//判断迭代器类型
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if<
	std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
	std::is_trivially_copy_assignable<Up>::value,
	Up*>::type
	unchecked_copy_backward(Tp* first, Tp* last, Up* result) {
	const auto n = static_cast<size_t>(last - first);
	if (n != 0) {
		result -= n;	//逆向
		std::memmove(result, first, n * sizeof(Up));	//std::memmove:最快速的内存对拷函数
	}
	return result;
}

//copy_backward的接口
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
{
	return unchecked_copy_backward(first, last, result);
}


/*****************************************************************************************/
// copy_if
// 把[first, last)内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上
/*****************************************************************************************/
template <class InputIter,class OutputIter,class UnaryPredicate>
OutputIter copy_if(InputIter first, InputIter last, OutputIter result, UnaryPredicate unary_pred) {
	for (; first != last; ++first) {
		if (unary_pred(*first)) {
			*result++ = *first;		//拷贝给result后，result++
		}
	}
	return result;
}

/*****************************************************************************************/
// copy_n
// 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
// 返回一个 pair 分别指向拷贝结束的尾部
/*****************************************************************************************/
//input_iterator版本
template <class InputIter, class Size, class OutputIter>mystl::pair<InputIter, OutputIter> unchecked_copy_n
(InputIter first, Size n, OutputIter result, mystl::input_iterator_tag) {
	for (; n > 0; --n, ++first, ++result)
		*result = *first;
	return mystl::pair<InputIter, OutputIter>(first, result);
}

//random_iterator版本
template <class RandomIter, class Size, class OutputIter>mystl::pair<InputIter, OutputIter> unchecked_copy_n
(RandomIter first, Size n, OutputIter result, mystl::random_access_iterator_tag) {
	auto last = first + n;
	return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));	//使用copy函数
}

// unchecked_copy的接口
template <class InputIter, class Size, class OutputIter>
mystl::pair<InputIter, OutputIter>
copy_n(InputIter first, Size n, OutputIter result) {
	return unchecked_copy_n(first, n, result, mystl::iterator_category(first));	//判断迭代器类型
}

/*****************************************************************************************/
// move
// 把 [first, last)区间内的元素移动到 [result, result + (last - first))内
/*****************************************************************************************/
// input_iterator_tag 版本
template <class InputIter, class OutputIter>OutputIter unchecked_move_cat
(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag) {
	//以迭代器等同与否，决定循环是否继续。速度慢
	for (; first != last; ++first, ++result) {
		*result = mystl::move(*first);
	}
	return result;
}

// random_access_iterator_tag 版本
template <class RandomIter, class OutputIter>OutputIter unchecked_move_cat
(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag) {
	//以n决定循环的执行次数。速度极快
	for (auto n = last - first; n > 0; --n, ++first, ++result) {
		*result = mystl::move(*first);
	}
	return result;
}

//unchecked_move的接口
template <class InputIter, class OutputIter>
OutputIter
unchecked_move(InputIter first, InputIter last, OutputIter result)
{
	return unchecked_move_cat(first, last, result, iterator_category(first));	//判断迭代器类型
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if<
	std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
	std::is_trivially_move_assignable<Up>::value,
	Up*>::type
	unchecked_move(Tp* first, Tp* last, Up* result) {
	const size_t n = static_cast<size_t>(last - first);
	if (n != 0) {
		std::memmove(result, first, n * sizeof(Up));	//使用memmove函数
	}
	return result + n;
}

//move的接口
template <class InputIter, class OutputIter>
OutputIter move(InputIter first, InputIter last, OutputIter result)
{
	return unchecked_move(first, last, result);
}

/*****************************************************************************************/
// move_backward
// 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
/*****************************************************************************************/
// bidirectional_iterator_tag 版本
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result, mystl::bidirectional_iterator_tag) {
	//以迭代器是否相等作为判断，速度慢
	while (first != last) {
		*--result = mystl::move(*--last);	//逆向移动
	}
	return result;
}

// random_access_iterator_tag 版本
template <class RandomIter1, class RandomIter2>
RandomIter2
unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
	RandomIter2 result, mystl::random_access_iterator_tag) {
	//以n决定循环的执行次数。速度极快
	for (auto n = last - first; n > 0; --n)
		*--result = mystl::move(*--last);
	return result;
}

//unchecked_move_backward接口
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result)
{
	return unchecked_move_backward_cat(first, last, result,
		iterator_category(first));	//判断迭代器类型
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if<
	std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
	std::is_trivially_move_assignable<Up>::value,
	Up*>::type
	unchecked_move_backward(Tp* first, Tp* last, Up* result) {
	const size_t n = static_cast<size_t>(last - first);
	if (n != 0) {
		result -= n;	//逆向，所以是-=
		std::memmove(result, first, n * sizeof(Up));	//使用memmove函数
	}
	return result;
}

//move_backward的接口
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
{
	return unchecked_move_backward(first, last, result);
}




}	//命名空间结束符


#endif // !MYSTL_ALGOBASE_H_

