#ifndef MYSTL_NUMERIC_H_
#define MYSTL_NUMERIC_H_

// 这个头文件包含了 mystl 的数值算法

#include "iterator.h"		//迭代器

namespace mystl {

/*****************************************************************************************/
// accumulate：用来计算init和[first,last)内所有元素的总和
// 版本1：以初值 init 对每个元素进行累加
// 版本2：以初值 init 对每个元素进行自定义的二元操作
//调用accumulate需要提供初值init，以保证当[first,last)为空时仍能获得一个明确定义的值
/*****************************************************************************************/
// 版本1
template <class InputIter, class T> T accumulate(InputIter first, InputIter last, T init) {
	for (; first != last; ++first) {
		init += *first;		//将每个元素值累加到初值init身上
	}
	return init;
}

//版本2
template <class InputIter, class T, class BinaryOp> T accumulate
(InputIter first, InputIter last, T init, BinaryOp binary_op) {
	for (; first != last; ++first) {
		init = binary_op(init, *first);	//对每个元素执行二元操作
	}
	return init;
}

/*****************************************************************************************/
// adjacent_difference：用来计算[first,last)中相邻元素的差额，就地运算方式
// 版本1：采用operator- 计算相邻元素的差值，结果保存到以 result 为起始的区间上
// 版本2：自定义相邻元素的二元操作
/*****************************************************************************************/
// 版本1
template <class InputIter, class OutputIter> OutputIter adjacent_difference
(InputIter first, InputIter last, OutputIter result) {
	if (first == last) return result;
	*result = *first;		// 记录第一个元素
	auto value = *first;	
	while (++first != last) {	//走过整个区间
		auto tmp = *first;	
		*++result = tmp - value;	//将相邻元素的差额(后-前)，赋值给目的端
		value = tmp;	//value每次进一
	}
	return ++result;	//返回每一个相邻元素的差额
}

//版本2
template <class InputIter, class OutputIter, class BinaryOp>OutputIter adjacent_difference
(InputIter first, InputIter last, OutputIter result, BinaryOp binary_op) {
	if (first == last) return result;
	*result = *first;		// 记录第一个元素
	auto value = *first;
	while (++first != last) {	//走过整个区间
		auto tmp = *first;
		*++result = binary_op(tmp, value);	//将相邻元素二元运算的结果，赋值给目的端
		value = tmp;		//value每次进一
	}	
	return ++result;		//返回每一个相邻元素的差额
}

/*****************************************************************************************/
// inner_product：计算[first,last)和[first2,first2+(last1-first1))的内积
// 版本1：以 init 为初值，计算两个区间的内积   
// 版本2：自定义 operator+ 和 operator*
//调用函数时，需要提供初值init
/*****************************************************************************************/
// 版本1
template <class InputIter1, class InputIter2, class T> T inner_product
(InputIter1 first1, InputIter1 last2, InputIter2 first2, T init) {
	for (; first1 != last1; ++first1,++first2) {	//以第一序列元素个数为据，将两个序列都走一遍
		init = init + (*first1 * first2);	//执行两个序列的一般内积
	}
	return init;
}

//版本2
template <class InputIter1, class InputIter2, class T, class BinaryOp1, class BinaryOp2> T inner_product
(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init, BinaryOp1 binary_op1, BinaryOp2 binary_op2) {
	for (; first1 != last1; ++first1, ++first2) {	//以第一序列元素个数为据，将两个序列都走一遍
		//以外界提供的仿函数来取代第一版本中的operator*和operator+
		init = binary_op1(init, binary_op2(*first1, *first2));	
	}
	return init;
}

/*****************************************************************************************/
// partial_sum：用来计算局部总和，就地运算的质变算法
// 版本1：计算局部累计求和，结果保存到以 result 为起始的区间上
// 版本2：进行局部进行自定义二元操作
/*****************************************************************************************/
//版本1
template <class InputIter, class OutputIter> OutputIter partial_sum
(InputIter first, InputIter last, OutputIter result) {
	if (first == last) return result;
	*result = *first;		// 记录第一个元素
	auto value = *first;
	while (++first != last) {	
		value = value + *first;		//前n个元素的总和
		*++result = *value;			//指定给目的端
	}
	return ++result;
}

//版本2
template <class InputIter, class OutputIter, class BinaryOp>OutputIter partial_sum
(InputIter first, InputIter last, OutputIter result, BinaryOp binary_op) {
	if (first == last) return result;
	*result = *first;	//记录第一个元素
	auto value = *first;
	while (++first != last) {
		value = binary_op(value, *first);	//前n个元素的总计
		*++result = *value;					//指定给目的端
	}
	return ++result;
}

///*****************************************************************************************/
////power：用来计算某数的n幂次方，即自己对自己进行某种运算，达n次
////运算类型由外界指定，如果指定为乘法，即乘幂
////版本1：乘幂
////版本2：幂次方。如果指定为乘法运算，则当n>0时返回x^n
///*****************************************************************************************/
////版本1
//template <class T, class Integer>inline T power(T x, Integer n) {
//	return power(x, n, multiplies<T>());
//}
//
////版本2：注意"MonoidOp"必须满足结合律，但不需满足交换律
//template <class T, class Integer, class MonoidOp> T powder(T x, Integer n, MonoidOp Monoid_op) {
//	if(n==0) return iden
//}

/*****************************************************************************************/
// iota：用来设定某个区间的内容，是一种质变算法
// 填充[first, last)，以 value 为初值开始递增
//即在[first,last)内填充value,value+1,value+2...
/*****************************************************************************************/
template <class ForwardIter, class T> void iota(ForwardIter first, ForwardIter last, T value) {	
	while (++first!=last)
	{
		*first = value;
		++value;	//递增value
	}
}

}	//命名空间结束符

#endif // !MYSTL_NUMERIC_H_

