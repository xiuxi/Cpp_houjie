#ifndef MYSTL_SET_ALGO_H_
#define MYSTL_SET_ALGO_H_
// 这个头文件包含 set 的四种算法: 并集union, 交集intersection, 差集difference, 对称差集symmetric_difference
// 所有函数都要求序列有序

#include "algobase.h"       //基本算法
#include "iterator.h"       //迭代器

namespace mystl {

//*****************************************************************************************/
// set_union：构造S1、S2的并集，集合内包含S1或S2内的每一个元素，稳定操作
// 计算 S1∪S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
/*****************************************************************************************/
//版本1
template<class InputIter1, class InputIter2, class OutputIter>OutputIter set_union
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
    //当两个区间都尚未到达尾端时，执行以下操作
	while (first1 != last1 && first2 != last2) {
        //在两区内分别移动迭代器，首先将元素较小者(假设为A区)记录于目标区
        //然后移动A区迭代器使之前进，同时间另一区迭代器不动。然后进行新一次的比大小、记录小值、迭代器移动
        //直到两区中有一区到达尾端。
        //如果元素相等，取S1者记录于目标区，并同时移动两个迭代器
		if (*first1 < *first2) {
			*result = *first1;
			++first1;
		}
		else if (*first2 < *first1) {
			*result = *first2;
			++first2;
		}
		else {
			*result = *first1;
			++first1;
			++first2;
		}
		++result;
	}
    //只要两区之中有一区到达尾端，就结束上述while循环
    // 以下将尚未到达尾端的区间的所有剩余元素拷贝到目的端
    //此刻的[first1,last1)和[first2,last2)之中有一个是空白区间
	return mystl::copy(first2, last2, mystl::copy(first1, last1, result));
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class OutputIter, class Compared>
OutputIter set_union(InputIter1 first1, InputIter1 last1,
    InputIter2 first2, InputIter2 last2,
    OutputIter result, Compared comp)
{
    while (first1 != last1 && first2 != last2)
    {
        if (comp(*first1, *first2))
        {
            *result = *first1;
            ++first1;
        }
        else if (comp(*first2, *first1))
        {
            *result = *first2;
            ++first2;
        }
        else
        {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    // 将剩余元素拷贝到 result
    return mystl::copy(first2, last2, mystl::copy(first1, last1, result));
}

/*****************************************************************************************/
// set_intersection：构造S1、S2之交集，此集合内含同时出现于S1和S2内的每一个元素
// 计算 S1∩S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
/*****************************************************************************************/
//版本1
template <class InputIter1, class InputIter2, class OutputIter>OutputIter set_intersection
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
    //当两个区间都尚未到达尾端时，执行以下操作...
    while (first1 != last1 && first2 != last2) {
        //在两区间内分别移动迭代器，直到遇有元素值相同，暂停，将该值记录于目标区
        //再继续移动迭代器，直到两区之中有一区到达尾端
        if (*first1 < *first2) {
            ++first1;
        }
        else if (*first2 < *first1) {
            ++first2;
        }
        else {
            *result = *first1;  //有相同元素时，优先取S1的元素
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1,class InputIter2,class OutputIter,class Compared>
OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
    InputIter2 first2, InputIter2 last2,
    OutputIter result, Compared comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            ++first1;
        }
        else if (comp(*first2, *first1)) {
            ++first2;
        }
        else {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
}

/*****************************************************************************************/
// set_difference:构造S1、S2之差集，此集合内含出现于S1但不出现于S2的每一个元素
// 计算 S1-S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
/*****************************************************************************************/
//版本1
template <class InputIter1, class InputIter2, class OutputIter>OutputIter set_difference
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
    //当两个区间都尚未到达尾端时，执行以下操作...
    while (first1 != last1 && first2 != last2) {
        //在两区内分别移动迭代器。
        //当第一区间的元素等于第二区间的元素(表示此值同时存在于两区间),就让两区间同时前进
        //当第一区间的元素大于第二区间的元素，就让第二区间前进
        //有了这两种处理，就保证当第一区间的元素小于第二区间的元素时，第一区间的元素只存在于第一区间，不存在第二区间
        //于是将它记录于目标区
        if (*first1 < first2) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (*first2 < *first1) {
            ++first2;
        }
        else
        {
            ++first1;
            ++first2;
        }
    }
    return mystl::copy(first1, last1, result);  //将结果拷贝到目标区
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class OutputIter, class Compared>
OutputIter set_difference(InputIter1 first1, InputIter1 last1,
    InputIter2 first2, InputIter2 last2,
    OutputIter result, Compared comp)
{ 
    while (first1 != last1 && first2 != last2)
    {
        if (comp(*first1, *first2))
        {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (comp(*first2, *first1))
        {
            ++first2;
        }
        else
        {
            ++first1;
            ++first2;
        }
    }
    return mystl::copy(first1, last1, result);  
}

/*****************************************************************************************/
// set_symmetric_difference：构造S1、S2之对称差集，此集合内含"出现于S1，但不出现于S2"以及"出现于S2但不出现于S1"的元素
// 计算 (S1-S2)∪(S2-S1) 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
/*****************************************************************************************/
//版本1
template <class InputIter1, class InputIter2, class OutputIter>OutputIter set_symmetric_difference
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
    //当两个区间都尚未到达尾端时，执行以下操作...
    while (first1 != last1 && first2 != last2) {
        //在两区间内分别移动迭代器，当两区间内的元素相等，就让两区间同时前进
        //在两区间内的元素不等，就记录较小值于目标区，并令较小值所在区间前进
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
            ++result;
        }
        else {
            ++first1;
            ++first2;
        }
    }
    return mystl::copy(first2, last2, mystl::copy(first1, last1, result));  //递归拷贝元素到目标区
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class OutputIter, class Compared>
OutputIter set_symmetric_difference(InputIter1 first1, InputIter1 last1,
    InputIter2 first2, InputIter2 last2,
    OutputIter result, Compared comp)
{
    while (first1 != last1 && first2 != last2)
    {
        if (comp(*first1, *first2))
        {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (comp(*first2, *first1))
        {
            *result = *first2;
            ++first2;
            ++result;
        }
        else
        {
            ++first1;
            ++first2;
        }
    }
    return mystl::copy(first2, last2, mystl::copy(first1, last1, result));
}

}	//命名空间结束符

#endif // !MYSTL_SET_ALGO_H_

