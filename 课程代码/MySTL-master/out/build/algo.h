#ifndef MYSTL_ALGO_H_
#define MYSTL_ALGO_H_

#ifdef _MSC_VER						//Virtual Stdio编译器
#pragma warning(push)				//保存当前的编译器警告状态，类似的，#pragma warning(pop)是恢复原先的警告状态。 
#pragma warning(disable : 4244)		//禁止编译器警告
#endif

// 这个头文件包含了 mystl 的一系列其他算法

#include <cstddef>				//定义了一些常用的常量、宏、类型和函数
#include <ctime>	//ctime功能是把日期和时间转换为字符串，而ctime类的对象表示的时间是基于格林威治标准时间（GMT）的

#include "algobase.h"			//基本算法
#include "memory.h"				//高级的动态内存管理
#include "heap_algo.h"			//堆算法
#include "functional.h"			//仿函数与哈希函数

namespace mystl {

/*****************************************************************************************/
// all_of
// 检查[first, last)内是否全部元素都满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/
template <class InputIter, class UnaryPrediacter>bool all_of
(InputIter first, InputIter last, UnaryPrediacter unary_pred) {
	for (; first != last; ++first) {
		if (!unary_pred(*first)) 	//如果不能返回一元操作
			return false;
	}
	return true;
}

/*****************************************************************************************/
// any_of
// 检查[first, last)内是否存在某个元素满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/
template <class InputIter, class UnaryPrediacter>bool any_of
(InputIter first, InputIter last, UnaryPrediacter unary_pred) {
	for (; first != last; ++first) {
		if (unary_pred(*first))
			return true;
	}
	return false;
}

/*****************************************************************************************/
// none_of
// 检查[first, last)内是否全部元素都不满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/
template <class InputIter, class UnaryPredicate> bool none_of
(InputIter first, InputIter last, UnaryPredicate unary_pred) {
	for (; first != last; ++first) {
		if (unary_pred(*first))
			return false;
	}
	return true;
}

/*****************************************************************************************/
// count
// 对[first, last)区间内的元素与给定值进行比较，缺省使用 operator==，返回元素相等的个数
/*****************************************************************************************/
template <class InputIter, class T>size_t count(InputIter first, InputIter last, const T& value) {
	size_t n = 0;
	for (; first != last; ++first) {	//遍历区间
		//相等就累计加一
		if (*first == value)
			++n;
	}
	return n;
}

/*****************************************************************************************/
// count_if
// 对[first, last)区间内的每个元素都进行一元 unary_pred 操作，返回结果为 true 的个数
/*****************************************************************************************/
template <class InputIter, class UnaryPredicate>size_t count_if
(InputIter first, InputIter last, UnaryPredicate unary_pred) {
	for (; first != last; ++first) {	//遍历区间
		if (unary_pred(*first))			//如果元素进行一元操作的运算结果为true
			++n;						//计数器累加1
	}
	return n;
}

/*****************************************************************************************/
// find
// 在[first, last)区间内找到等于 value 的元素，返回一个InputIterator指向该元素
/*****************************************************************************************/
template <class InputIter, class T>InputIter find(InputIter first, InputIter last, const T& value) {
	while (first != last && *first != *value) {
		++first;
	}
	return first;
}

/*****************************************************************************************/
// find_if
// 在[first, last)区间内找到第一个令一元操作 unary_pred 为 true 的元素并返回指向该元素的迭代器
/*****************************************************************************************/
template <class InputIter, class UnaryPredicate>InputIter find_if
(InputIter first, InputIter last, UnaryPredicate unary_pred) {
	while (first != last && !unary_pred(*first))
		++first;
	return first;
}

/*****************************************************************************************/
// find_if_not
// 在[first, last)区间内找到第一个令一元操作 unary_pred 为 false 的元素并返回指向该元素的迭代器
/*****************************************************************************************/
template <class InputIter, class UnaryPredicate>InputIter find_if_not
(InputIter first, InputIter last, UnaryPredicate unary_pred) {
	while (first != last && unary_pred(*first))
		++first;
	return first;
}

/*****************************************************************************************/
// search
// 在[first1, last1)中查找[first2, last2)的首次出现点
//如果[first1,last1)中不存在与[first2,last2）完全匹配的子序列，便返回迭代器last
/*****************************************************************************************/
//版本1
template <class ForwardIter1, class ForwardIter2>ForwardIter1 search
(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2) {
	auto d1 = mystl::distance(first1, last1);
	auto d2 = mystl::distance(first2, last2);
	//如果第二序列大于第一序列，不可能成为其子序列
	if (d1 < d2)
		return last1;
	auto current1 = first1;
	auto current2 = first2;
	while (current2 != last2) {	//遍历整个第二序列
		if (*current1 == *current2) {	//如果这个元素相同
			++current1;					//调整，以便比对下一元素
			++current2;
		}
		else {					//如果这元素不等
			if (d1 == d2) {		//如果两个序列一样长
				return last1;	//表示不可能成功，返回last1
			}
			else {				//两个序列不一样长(至此肯定序列一大于序列二)
				current1 = ++first1;	//调整第一序列的标兵
				current2 = first2;		//准备在新起点上再找一次
				--d1;	//已经排除了序列一的一个元素，所以序列一的长度要减1
			}
		}
	}
	return first1;
}

//版本2：使用函数对象 comp 代替比较操作
template <class ForwardIter1, class ForwardIter2, class Compared>
ForwardIter1
search(ForwardIter1 first1, ForwardIter1 last1,
	ForwardIter2 first2, ForwardIter2 last2, Compared comp)
{
	auto d1 = mystl::distance(first1, last1);
	auto d2 = mystl::distance(first2, last2);
	if (d1 < d2)
		return last1;
	auto current1 = first1;
	auto current2 = first2;
	while (current2 != last2)
	{
		if (comp(*current1, *current2))		//使用函数对象comp
		{
			++current1;
			++current2;
		}
		else
		{
			if (d1 == d2)
			{
				return last1;
			}
			else
			{
				current1 = ++first1;
				current2 = first2;
				--d1;
			}
		}
	}
	return first1;
}

/*****************************************************************************************/
// search_n
// 在[first, last)中查找连续 n 个 value 所形成的子序列，返回一个迭代器指向该子序列的起始处
/*****************************************************************************************/
//版本1
template <class ForwardIter, class Size, class T>ForwardIter search_n
(ForwardIter first, ForwardIter last, Size n, const T& value) {
	if (n <= 0) {
		return first;
	}
	else {
		first = mystl::find(first, last, value);	//首先找出value第一次出现点
		while (first != last) {		//继续查找余下元素
			auto m = n - 1;			//value还应该出现n-1次
			auto i = first;			//从上次出现点接下去查找
			++i;					//往前查找
			while (i != last && m != 0 && *i == value) {	//下一个元素依旧与value相等
				++i;				//继续往下查找
				--m;				//而且"value还应出现次数"便可减1
			}						//回到内循环继续查找
			if (m == 0) {			//m=0表示找到了"元素值出现n次"的子序列，大功告成
				return first;
			}
			else {					//尚未完成所有查找
				first = mystl::find(i, last, value);	//找value的下一个出现点，并准备回到外循环
			}
		}
		return last;
	}
}

//版本2：使用函数对象 comp 代替比较操作
template <class ForwardIter, class Size, class T, class Compared>ForwardIter search_n
(ForwardIter first, ForwardIter last, Size n, const T& value, Compared comp) {
	if (n <= 0) {
		return first;
	}
	else {
		while (first != last) {
			if (comp(*first, value))	//首先找出第一个符合条件的元素
				break;					
			++first;					//找到就离开
		}
		while (first != last) {			//继续查找余下元素
			auto m = n - 1;				//还应有n-1个元素符合条件
			auto i = first;				//从上次出现点继续往下查找
			++i;
			while (i != last && m != 0 && comp(*i, value)) {	//循环确定接下来n-1个元素是否都符合条件
				++i;
				--m;					//符合条件就m-1
			}
			if (m = 0) {				//m=0表示找到了"n个符合条件"的子序列，大功告成
				return first;
			}
			else {						//尚未完成所有查找
				while (i != last) {
					if (comp(*i, value))	//找下一个符合条件的元素
						break;
					++i;
				}
				first = i;				//准备回到外循环
			}
		}
		return last;
	}	
}

/*****************************************************************************************/
// find_end
// 在[first1, last1)区间中查找[first2, last2)最后一次出现的地方，若不存在返回 last1
/*****************************************************************************************/
//版本1
// find_end_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter1, class ForwrdIter2>ForwardIter1 find_end_dispatch
(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwrdIter2 last2,
	forward_iterator_tag, forward_iterator_tag) {
	if (first2 == last2) {	//如果查找目标是空的
		return last1;		//返回last1，表示该"空子序列"的最后出现点
	}
	else {
		auto result = last1;
		while (true) {
			//利用search()查找某个子序列的首次出现点，找不到的话返回last1
			auto new_result = mystl::search(first1, last1, first2, last2);
			if (new_result == last1) {	//没找到
				return result;
			}
			else {
				result = new_result;	//调动以下标兵，准备下一个查找行动
				first1 = new_result;
				++first1;
			}
		}
	}
}

// find_end_dispatch 的 bidirectional_iterator_tag 版本(可以逆向查找)
template <class BidirectionalIter1, class BidirectionalIter2>BidirectionalIter1 find_end_dispatch
(BidirectionalIter1 first, BidirectionalIter1 last2, BidirectionalIter2 first2, BidirectionalIter2 last2
	forward_iterator_tag, forward_iterator_tag) {
	//由于查找的是"最后出现地点"，因此反向查找比较快。利用reverse_iterator
	typedef reverse_iterator<BidirectionalIter1> reviter1;
	typedef reverse_iterator<BidirectionalIter2> reviter2;
	reviter1 rlast1(first1);
	reviter2 rlast2(first2);
	//查找时，将序列一和序列二统统逆转方向
	reviter1 rresult = mystl::search(reviter1(last1), rlast1, reviter2(last2), rlast2);
	if (rresult == rlast1) {	//没找到
		return last1;
	}
	else {						//找到了
		auto result = rresult.base();	//转回正常(非逆向)迭代器
		mystl::advance(result, -mystl::distance(first2, last2));	//调整回到子序列的起头处
		return result;
	}
}

//find_end的接口
template <class ForwardIter1, class ForwardIter2>ForwardIter1 find_end
(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2) {
	typedef typename iterator_traits<ForwardIter1>::iterator_category Catagory1;
	typedef typename iterator_traits<ForwardIter2>::iterator_category Category2;
	//以下根据两个区间的类属，调用不同的下层函数
	return mystl::find_end_dispatch(first1, last1, first2, last2, Catagory1(), Category2());  //型别名称+()是临时对象
}

// 版本2：使用函数对象 comp 代替比较操作
// find_end_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter1, class ForwardIter2, class Compared>
ForwardIter1
find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
	ForwardIter2 first2, ForwardIter2 last2,
	forward_iterator_tag, forward_iterator_tag, Compared comp)
{
	if (first2 == last2)
	{
		return last1;
	}
	else
	{
		auto result = last1;
		while (true)
		{
			// 利用 search 查找某个子序列的首次出现点，找不到则返回 last1
			auto new_result = mystl::search(first1, last1, first2, last2, comp);
			if (new_result == last1)
			{
				return result;
			}
			else
			{
				result = new_result;
				first1 = new_result;
				++first1;
			}
		}
	}
}

// find_end_dispatch 的 bidirectional_iterator_tag 版本
template <class BidirectionalIter1, class BidirectionalIter2, class Compared>
BidirectionalIter1
find_end_dispatch(BidirectionalIter1 first1, BidirectionalIter1 last1,
	BidirectionalIter2 first2, BidirectionalIter2 last2,
	bidirectional_iterator_tag, bidirectional_iterator_tag, Compared comp)
{
	typedef reverse_iterator<BidirectionalIter1> reviter1;
	typedef reverse_iterator<BidirectionalIter2> reviter2;
	reviter1 rlast1(first1);
	reviter2 rlast2(first2);
	reviter1 rresult = mystl::search(reviter1(last1), rlast1, reviter2(last2), rlast2, comp);
	if (rresult == rlast1)
	{
		return last1;
	}
	else
	{
		auto result = rresult.base();
		mystl::advance(result, -mystl::distance(first2, last2));
		return result;
	}
}

//find_end的接口
template <class ForwardIter1, class ForwardIter2, class Compared>
ForwardIter1
find_end(ForwardIter1 first1, ForwardIter1 last1,
	ForwardIter2 first2, ForwardIter2 last2, Compared comp)
{
	typedef typename iterator_traits<ForwardIter1>::iterator_category Category1;
	typedef typename iterator_traits<ForwardIter2>::iterator_category Category2;
	return mystl::find_end_dispatch(first1, last1, first2, last2, Category1(), Category2(), comp);
}

/*****************************************************************************************/
// find_first_of
// 在[first1, last1)中查找[first2, last2)中的某些元素，返回指向第一次出现的元素的迭代器
/*****************************************************************************************/
//版本1
template <class InputIter, class ForwardIter>InputIter find_first_of
(InputIter first1, InputIter last1, ForwardIter first2, ForwardIter last2) {
	for (; first1 != last1; ++first1) {	//遍历序列一
		for (auto iter = first2; iter != last2; ++iter) {	//根据序列二的每个元素
			if (*first1 == *iter)	//如果序列一的元素等于序列二的元素
				return first1;		//找到就结束
		}	
	}
	return last1;
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter, class ForwardIter, class Compared>
InputIter
find_first_of(InputIter first1, InputIter last1,
	ForwardIter first2, ForwardIter last2, Compared comp)
{
	for (; first1 != last1; ++first1)
	{
		for (auto iter = first2; iter != last2; ++iter)
		{
			if (comp(*first1, *iter))	//如果序列一和序列二的元素满足comp条件
				return first1;	
		}
	}
	return last1;
}

/*****************************************************************************************/
// for_each
// 使用一个仿函数对象 f 对[first, last)区间内的每个元素执行一个 operator() 操作，但不能改变元素内容
// f() 可返回一个值，但该值会被忽略
/*****************************************************************************************/
template <class InputIter, class Function>Function for_each(InputIter first, InputIter last, Function f) {
	for (; first != last; ++first) {
		f(*first);		//调用仿函数f的function call操作符，返回值被忽略
	}
	return f;
}

/*****************************************************************************************/
// adjacent_find
// 找出第一对匹配的相邻元素，缺省使用 operator== 比较，如果找到返回一个迭代器，指向这对元素的第一个元素
/*****************************************************************************************/
//版本1：查找相邻的重复元素
template <class ForwardIter>ForwardIter adjacent_find(ForwardIter first, ForwardIter last) {
	if (first == last) return first;
	auto next = first;
	while (++next != last) {
		if (*first == *next)	//如果找到相邻的元素值相同
			return first;		//就返回指向这对元素的第一个元素的迭代器
		first = next;
	}
	return last;
}

//版本2：使用函数对象 comp 代替比较操作
template <class ForwardIter, class Compared>ForwardIter adjacent_find
(ForwardIter first, ForwardIter last, Compared comp) {
	if (first == last) return first;
	auto next = first;
	while (++next != last) {
		if (comp(*first, *next))	//如果找到相邻的元素符合外界指定的条件，就结束
			return first;
		first = next;
	}
	return last;
}

/**************************************************************************************************/
// lower_bound：使用二分查找
// 在已排序的[first, last)中查找第一个不小于 value 的元素，并返回指向它的迭代器，若没有则返回 last
//即返回值是"在不破坏排序状态的原则下，可插入value的第一个位置"
/**************************************************************************************************/
//版本1：使用operator<进行比较
// lbound_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter, class T>ForwardIter lbound_dispatch
(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag) {
	auto len = mystl::distance(first, last);	//求取整个区间的长度len
	auto half = len;
	ForwardIter middle;
	while (len > 0) {
		// >> 是右移操作符，len >> 1 表示将len转为二进制的8位数字后右移1位, 效果相当于除以2， 左移操作符 << 同理
		half = len >> 1;						//除以2
		middle = first;							//令middle指向first位置
		mystl::advance(middle, half);			//advance表示将middle右移half长度的位置，即middle指向中间位置
		if (*middle < value) {					//如果中间位置的元素小于value
			first = middle;						//将first指向middle
			++first;							//令first指向middle的下一位置
			len = len - half - 1;				//修正len，在右半部分进行循环查找
		}
		else {				
			len = half;							//修正len，在左半部分进行循环查找
		}
	}
	return first;
}

// lbound_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter, class T>RandomIter lbound_dispatch
(RandomIter first, RandomIter last, const T& value, random_access_iterator_tag) {
	auto len = last - first;			//求取整个区间的长度len,利用random iterator的特性
	auto half = len;
	RandomIter middle;
	while (len > 0) {
		half = len >> 1;				//除以2
		middle = first + half;			//middle指向中间位置，因为是random iterator,所以可直接使用"+"去移动half的长度
		if (*middle < value) {			//如果中间位置的元素小于value
			first = middle + 1;			//令first指向middle的下一位置，random iterator可直接middle+1
			len = len - half - 1;		//修正len，在右半部分进行循环查找
		}
		else {
			len = half;					//修正len，在右半部分进行循环查找
		}
	}
	return first;
}

//lower_bound的接口
template <class ForwardIter, class T>ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value) {
	return mystl::lbound_dispatch(first, last, value, iterator_category(first));	//判断迭代器类型
}

//版本2：使用函数对象 comp 代替比较操作
// lbound_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter, class T, class Compared>
ForwardIter
lbound_dispatch(ForwardIter first, ForwardIter last,
	const T& value, forward_iterator_tag, Compared comp)
{
	auto len = mystl::distance(first, last);
	auto half = len;
	ForwardIter middle;
	while (len > 0)
	{
		half = len >> 1;
		middle = first;
		mystl::advance(middle, half);
		if (comp(*middle, value))
		{
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
		{
			len = half;
		}
	}
	return first;
}

// lbound_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter, class T, class Compared>
RandomIter
lbound_dispatch(RandomIter first, RandomIter last,
	const T& value, random_access_iterator_tag, Compared comp)
{
	auto len = last - first;
	auto half = len;
	RandomIter middle;
	while (len > 0)
	{
		half = len >> 1;
		middle = first + half;
		if (comp(*middle, value))
		{
			first = middle + 1;
			len = len - half - 1;
		}
		else
		{
			len = half;
		}
	}
	return first;
}

//lower_bound的接口
template <class ForwardIter, class T, class Compared>
ForwardIter
lower_bound(ForwardIter first, ForwardIter last, const T& value, Compared comp)
{
	return mystl::lbound_dispatch(first, last, value, iterator_category(first), comp);
}

/***********************************************************************************************/
// upper_bound：使用二分查找
// 在已排序的[first, last)中查找第一个大于value 的元素，并返回指向它的迭代器，若没有则返回 last
//即返回值是"在不破坏顺序的情况下，可插入value的最后一个合适位置"
/***********************************************************************************************/
//版本1：使用operator<进行比较
// ubound_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter, class T>ForwardIter ubound_dispatch
(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag) {
	auto len = mystl::distance(first, last);	//求取整个区间的长度len
	auto half = len;
	ForwardIter middle;
	while (len > 0) {		
		half = len >> 1;						//除以2
		middle = first;
		mystl::advance(middle, half);			//令middle指向中间位置，advance将middle右移half个长度
		if (value < *middle) {					//如果中间位置的元素值大于value
			len = half;							//修正len，在左半部分进行循环查找
		}
		else {
			first = middle;						
			++first;							//将first指向middle的下一位置
			len = len - half - 1;				//修正len，在右半部分进行查找
		}
	}
	return first;
}

// ubound_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter, class T>RandomIter ubound_dispatch
(RandomIter first, RandomIter last, const T& value, random_access_iterator_tag) {
	auto len = last - first;					//求取整个区间的长度len
	auto half = len;
	RandomIter middle;
	while (len > 0) {
		half = len >> 1;						//除以2
		middle = first + half;					//令middle指向中间位置
		if (value < *middle) {					//如果中间位置的元素值大于value
			len = half;							//修正len，在左半部分进行循环查找
		}
		else {
			first = middle + 1;					//将first指向middle的下一位置
			len = len - half - 1;				//修正len，在右半部分进行查找
		}
	}
	return first;
}

//upper_bound的接口
template <class ForwardIter, class T>ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T& value) {
	return mystl::ubound_dispatch(first, last, value, iterator_category(first));	//判断迭代器类型
}

//版本2：使用函数对象 comp 代替比较操作
// ubound_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter, class T, class Compared>
ForwardIter
ubound_dispatch(ForwardIter first, ForwardIter last,
	const T& value, forward_iterator_tag, Compared comp)
{
	auto len = mystl::distance(first, last);
	auto half = len;
	ForwardIter middle;
	while (len > 0)
	{
		half = len >> 1;
		middle = first;
		mystl::advance(middle, half);
		if (comp(value, *middle))
		{
			len = half;
		}
		else
		{
			first = middle;
			++first;
			len = len - half - 1;
		}
	}
	return first;
}

// ubound_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter, class T, class Compared>
RandomIter
ubound_dispatch(RandomIter first, RandomIter last,
	const T& value, random_access_iterator_tag, Compared comp)
{
	auto len = last - first;
	auto half = len;
	RandomIter middle;
	while (len > 0)
	{
		half = len >> 1;
		middle = first + half;
		if (comp(value, *middle))
		{
			len = half;
		}
		else
		{
			first = middle + 1;
			len = len - half - 1;
		}
	}
	return first;
}

//upper_bound的接口
template <class ForwardIter, class T, class Compared>
ForwardIter
upper_bound(ForwardIter first, ForwardIter last, const T& value, Compared comp)
{
	return mystl::ubound_dispatch(first, last, value, iterator_category(first), comp);
}

/*****************************************************************************************/
// binary_search
// 二分查找，如果在[first, last)内有等同于 value 的元素，返回 true，否则返回 false
//利用lower_bound先找出"假设value存在的话，应该出现的位置"，再对比该位置上的值是否是要查找的目标，并返回对比结果
/*****************************************************************************************/
//版本1
template <class ForwardIter, class T>bool binary_search(ForwardIter first, ForwardIter last, const T& value) {
	//当且仅当[first,last)存在一个迭代器i使得"i<value和value<*i"皆不为真，就返回true
	auto i = mystl::lower_bound(first, last, value);	//value一定小于或等于lower_bound查找的*i
	return i != last && !(value < *i);					
}

//版本2：使用函数对象 comp 代替比较操作
template <class ForwardIter, class T, class Compared>bool binary_search
(ForwardIter first, ForwardIter last, const T& value, Compared comp) {
	auto i = mystl::lower_bound(first, last, value);
	return i != last && !comp(value, *i);
}

/*****************************************************************************************/
// equal_range
// 查找已排序的[first,last)区间中与 value 相等的元素所形成的区间，以pair返回一对迭代器指向区间首尾
// 第一个迭代器指向第一个不小于 value 的元素，第二个迭代器指向第一个大于 value 的元素
//i即lower_bound，j即upper_bound，[first,last)的每个元素都等于value，且是[first,last)中符合这一性质的最大子区间
/*****************************************************************************************/
//版本1：
// erange_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter, class T>mystl::pair<ForwardIter, ForwardIter> erange_dispatch
(ForwardIter first, ForwardIter last, const T& value, forward_iterator_tag) {
	auto len = mystl::distance(first, last);		//求取区间的长度
	auto half = len;
	ForwardIter middle, left, right;
	while (len > 0) {								//整个区间尚未遍历完毕
		half = len >> 1;							//找出中央位置
		middle = first;	
		mystl::advance(middle, half);				//令middle指向中间位置
		if (*middle < value) {						//如果中央元素小于指定值
			first = middle;		
			++first;								//令first指向middle的下一位置
			len = len - half - 1;					//在右半部分查找
		}
		else if (value < *middle) {					//如果中央元素大于指定值
			len = half;								//在左半部分查找
		}
		else {										//如果中央元素==指定值
			left = mystl::lower_bound(first, last, value);			//在前半段找lower_bound
			mystl::advance(first, len);				//令first移动len个长度
			right = mystl::upper_bound(++middle, first, value);		//在后半段找upper_bound
			return mystl::pair<ForwardIter, ForwardIter>(left, right);	//返回查找到的pair
		}
	}
	//整个区间都没有匹配的值，就返回一对迭代器，指向第一个大于value的元素
	return mystl::pair<ForwardIter, ForwardIter>(last, last);
}

// erange_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter, class T>mystl::pair<RandomIter, RandomIter>erange_dispatch
(RandomIter first, RandomIter last, const T& value, random_access_iterator_tag) {
	auto len = last - first;						//求取区间的长度
	auto half = len;
	RandomIter middle, left, right;
	while (len > 0) {								//整个区间尚未遍历完毕
		half = len >> 1;							//找出中央位置
		middle = first + half;						//令middle指向中间位置
		if (*middle < value) {						//如果中央元素小于指定值
			first = middle + 1;						//令first指向middle的下一位置
			len = len - half - 1;					//在右半部分查找
		}
		else if (value < *middle) {					//如果中央元素大于指定值
			len = half;								//在左半部分查找
		}
		else {										//如果中央元素==指定值
			left = mystl::lower_bound(first, middle, value);			//在前半段找lower_bound
			right = mystl::upper_bound(++middle, first + len, value);	//在后半段找upper_bound
			return mystl::pair<RandomIter, RandomIter>(left, right);	//返回查找到的pair
		}
	}
	//整个区间都没有匹配的值，就返回一对迭代器，指向第一个大于value的元素
	return mystl::pair<RandomIter, RandomIter>(last, last);
}

//equal_range的接口
template <class ForwardIter, class T>mystl::pair<ForwardIter, ForwardIter> equal_range
(ForwardIter first, ForwardIter last, const T& value) {
	return mystl::erange_dispatch(first, last, value, iterator_category(first));
}

//版本2：使用函数对象 comp 代替比较操作
// erange_dispatch 的 forward iterator 版本
template <class ForwardIter, class T, class Compared>
mystl::pair<ForwardIter, ForwardIter>
erange_dispatch(ForwardIter first, ForwardIter last,
	const T& value, forward_iterator_tag, Compared comp)
{
	auto len = mystl::distance(first, last);
	auto half = len;
	ForwardIter middle, left, right;
	while (len > 0)
	{
		half = len >> 1;
		middle = first;
		mystl::advance(middle, half);
		if (comp(*middle, value))
		{
			first = middle;
			++first;
			len = len - half - 1;
		}
		else if (comp(value, *middle))
		{
			len = half;
		}
		else
		{
			left = mystl::lower_bound(first, last, value, comp);
			mystl::advance(first, len);
			right = mystl::upper_bound(++middle, first, value, comp);
			return mystl::pair<ForwardIter, ForwardIter>(left, right);
		}
	}
	return mystl::pair<ForwardIter, ForwardIter>(last, last);
}

// erange_dispatch 的 random access iterator 版本
template <class RandomIter, class T, class Compared>
mystl::pair<RandomIter, RandomIter>
erange_dispatch(RandomIter first, RandomIter last,
	const T& value, random_access_iterator_tag, Compared comp)
{
	auto len = last - first;
	auto half = len;
	RandomIter middle, left, right;
	while (len > 0)
	{
		half = len >> 1;
		middle = first + half;
		if (comp(*middle, value))
		{
			first = middle + 1;
			len = len - half - 1;
		}
		else if (comp(value, *middle))
		{
			len = half;
		}
		else
		{
			left = mystl::lower_bound(first, middle, value, comp);
			right = mystl::upper_bound(++middle, first + len, value, comp);
			return mystl::pair<RandomIter, RandomIter>(left, right);
		}
	}
	return mystl::pair<RandomIter, RandomIter>(last, last);
}

//equal_range的接口
template <class ForwardIter, class T, class Compared>
mystl::pair<ForwardIter, ForwardIter>
equal_range(ForwardIter first, ForwardIter last, const T& value, Compared comp)
{
	return mystl::erange_dispatch(first, last, value, iterator_category(first), comp);
}

/*****************************************************************************************/
// generate
// 将仿函数对象 gen 的运算结果对[first, last)内的每个元素赋值
/*****************************************************************************************/
template <class ForwardIter, class Generator>void generate(ForwardIter first, ForwardIter last, Generator gen) {
	for (; first != last; ++first) {	//整个序列区间
		*first = gen();
	}
}


/*****************************************************************************************/
// generate_n
// 用仿函数对象 gen 连续对 n 个元素赋值
/*****************************************************************************************/
template <class ForwardIter, class Size, class Generator>void generate_n
(ForwardIter first,Size n, Generator gen) {
	for (; n > 0; --n, ++first) {		//只限n个元素
		*first = gen();
	}
}

/*****************************************************************************************/
// includes
// 判断序列一S1 是否包含序列二S2，即S2的每个元素都出现于S1
//S1和S2都必须是有序集合，其中的元素可重复
/*****************************************************************************************/
//版本1：
template <class InputIter1, class InputIter2>bool includes
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2) {
	while (first1 != last1 && first2 != last2) {	//两个区间都尚未走完
		if (*first2 < *first1) {					//因为元素有序，当存在序列二的元素小于序列一的元素时
			return false;							//"涵盖"的情况必然不成立，结束执行
		}
		else if (*first1 < *first2) {				//序列二的元素大于序列一的元素
			++first1;								//序列一前进1
		}
		else										//*first1==*first2
		{
			++first1, ++first2;						//两序列各自前进1
		}
	}
	return first2 == last2;						//有一个序列走完了，判断最后一关
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compared>bool includes
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, Compared comp) {
	while (first1!= last1 && first2 != last2) {
		if (comp(*first2, *first1)) {
			return false;
		}
		else if (comp(*first1, *first2)) {
			++first1;
		}
		else {
			++first1,++first2;
		}
	}
	return first2 == last2;
}

/*****************************************************************************************/
// is_heap
// 检查[first, last)内的元素是否为一个堆，如果是，则返回 true
/*****************************************************************************************/
//版本1：
template <class RandomIter>bool is_heap(RandomIter first, RandomIter last) {
	auto n = mystl::distance(first, last);
	auto parent = 0;
	for (auto child = 1; child < n; ++child) {
		if (first[parent] < first[child]) {			//如果父节点小于子节点
			return false;
		}
		//child & 1产生一个值为 1或 0 , 取决于 child 的最低有效位: 如果最后一位是 1 ,chlid & 1 的结果是 1 ;否则为0
		//这是一个二进制的按位与操作。
		if ((child & 1) == 0)
			++parent;
	}
	return true;
}

// 版本2：使用函数对象 comp 代替比较操作
template <class RandomIter, class Compared>
bool is_heap(RandomIter first, RandomIter last, Compared comp)
{
	auto n = mystl::distance(first, last);
	auto parent = 0;
	for (auto child = 1; child < n; ++child)
	{
		if (comp(first[parent], first[child]))
			return false;
		if ((child & 1) == 0)
			++parent;
	}
	return true;
}

/*****************************************************************************************/
// is_sorted
// 检查[first, last)内的元素是否升序，如果是升序，则返回 true
/*****************************************************************************************/
//版本1：
template <class ForwardIter>bool is_sorted(ForwardIter first, ForwardIter last) {
	if (first == last)
		return true;
	auto next = first;
	++next;
	for (; next != last; first = next, ++next) {	//遍历区间
		if (*next < *first) {
			return false;
		}
	}
	return true;
}

//版本2：使用函数对象 comp 代替比较操作
template <class ForwardIter, class Compared>
bool is_sorted(ForwardIter first, ForwardIter last, Compared comp)
{
	if (first == last)
		return true;
	auto next = first;
	++next;
	for (; next != last; first = next, ++next)
	{
		if (comp(*next, *first))
			return false;
	}
	return true;
}

/*****************************************************************************************/
// median
// 找出三个值的中间值
/*****************************************************************************************/
//版本1：
template <class T>const T& median(const T& left, const T& mid, const T& right) {
	if (left < mid) {
		if (mid < right)			// left < mid < right
			return mid;
		else if (left < right)		// left < right <= mid
			return right;
		else						// right <= left < mid
			return left;
	}
	else if (left < right)			// mid <= left < right
		return left;
	else if (mid < right)			// mid < right <= left
		return right;
	else							// right <= mid <= left
		return mid;
}
//版本2：使用函数对象 comp 代替比较操作
template <class T, class Compared>const T& median(const T& left, const T& media, const T& right, Compared comp) {
	if (comp(left, mid))
		if (comp(mid, right))
			return mid;
		else if (comp(left, right))
			return right;
		else
			return left;
	else if (comp(left, right))
		return left;
	else if (comp(mid, right))
		return right;
	else
		return mid;
}

/*****************************************************************************************/
// max_element
// 返回一个迭代器，指向序列中最大的元素
/*****************************************************************************************/
//版本1：
template <class ForwardIter>ForwardIter max_element(ForwardIter first, ForwardIter last) {
	if (first == last)
		return first;
	auto result = first;
	while (++first != last) {
		if (*result < *first) {
			result = first;			//如果目前元素比较大，就登记起来
		}
	}
	return result;
}

// 版本2：使用函数对象 comp 代替比较操作
template <class ForwardIter, class Compared>
ForwardIter max_element(ForwardIter first, ForwardIter last, Compared comp)
{
	if (first == last)
		return first;
	auto result = first;
	while (++first != last)
	{
		if (comp(*result, *first))
			result = first;
	}
	return result;
}

/*****************************************************************************************/
// min_element
// 返回一个迭代器，指向序列中最小的元素
/*****************************************************************************************/
template <class ForwardIter>ForwardIter min_element(ForwardIter first, ForwardIter last) {
	if (first == last)
		return first;
	auto result = first;
	while (++first != last) {
		if (*first < *result)
			result = first;
	}
	return result;
}

// 版本2：使用函数对象 comp 代替比较操作
template <class ForwardIter, class Compared>
ForwardIter min_elememt(ForwardIter first, ForwardIter last, Compared comp)
{
	if (first == last)
		return first;
	auto result = first;
	while (++first != last)
	{
		if (comp(*first, *result))
			result = first;
	}
	return result;
}

/*****************************************************************************************/
// swap_ranges
// 将[first1, last1)从 first2 开始，交换相同个数元素
// 这两个序列可位于同一容器，也可位于不同容器
// 交换的区间长度必须相同，两个序列不能互相重叠，返回一个迭代器指向序列二最后一个被交换元素的下一位置
/*****************************************************************************************/
template <class ForwardIter1, class ForwardIter2>ForwardIter2 swap_ranges
(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
	for (; first1 != last1; ++first1, ++first2) {
		mystl::iter_swap(first1, first2);
	}
	return first2;
}

/*****************************************************************************************/
// transform
// 第一个版本以函数对象 unary_op 作用于[first, last)中的每个元素并将结果保存至 result 中
// 第二个版本以函数对象 binary_op 作用于两个序列[first1, last1)、[first2, last2)的相同位置
//两个版本都把执行结果放进迭代器result所标示的容器中，返回值将指向结果序列的最后元素的下一位置
/*****************************************************************************************/
//版本1：使用一元操作符
template <class InputIter, class OutputIter, class UnaryOperation>OutputIter transform
(InputIter first, InputIter last, OutputIter result, UnaryOperation unary_op) {
	for (; first != last; ++first, ++result) {
		*result = unary_op(*first);
	}
	return result;
}

//版本2：使用二元操作符
template <class InputIter1, class InputIter2, class OutputIter, class BinaryOperation>OutputIter transform
(InputIter1 first1, InputIter1 last1, InputIter2 first2, OutputIter result, BinaryOperation binary_op) {
	for (; first1 != last1; ++first1, ++first2, ++result) {
		*result = binary_op(*first1,*first2);
	}
	return result;
}

/*****************************************************************************************/
// remove_copy
// 移除区间内与指定 value 相等的元素，并将结果复制到以 result 标示起始位置的容器上
//并不真正从容器中删除那些元素，返回值指向被复制的最后元素的下一位置
/*****************************************************************************************/
template <class InputIter, class OutputIter, class T>OutputIter remove_copy
(InputIter first, InputIter last, OutputIter result, const T& value) {
	for (; first != last; ++first) {
		if (*first != value) {			//如果不相等
			*result++ = *first;			//赋值给新容器，新容器前进一个位置
		}
	}
	return result;
}

/*****************************************************************************************/
// remove
// 移除所有与指定 value 相等的元素
// 并不从容器中删除这些元素，而是将每一个不与value相等的元素轮番赋值给first之后的空间
// 返回值标示出重新整理后的最后元素的下一位置
//因为array无法缩小尺寸，所以 remove 和 remove_if 不适用于 array
/*****************************************************************************************/
template <class ForwardIter, class T>ForwardIter remove(ForwardIter first, ForwardIter last, const T& value) {
	first = mystl::find(first, last, value);	//利用循序查找法找出第一个相等元素
	auto next = first;							//以next标示出来
	//利用"remove_copy()允许新旧容器重叠"的性质，进行移除操作
	//并将结果置于原容器中
	return first == last ? first : mystl::remove_copy(++next, last, first, value);
}

/*****************************************************************************************/
// remove_copy_if
// 移除区间内所有令一元操作 unary_pred 为 true 的元素，并将结果复制到以 result 为起始位置的容器上
/*****************************************************************************************/
template <class InputIter, class OutputIter, class UnaryPrediacte>OutputIter remove_copy_if
(InputIter first, InputIter last, OutputIter result, UnaryPrediacte unary_pred) {
	for (; first != last; ++first) {
		if (!unary_pred(*first)) {		//如果pred核定为flase
			*result = *first;			//就赋值给新容器(保留，不删除)
			++result;					//新容器前进一个位置
		}		
	}
	return result;
}

/*****************************************************************************************/
// remove_if
// 移除区间内所有令一元操作 unary_pred 为 true 的元素
// 并不从容器中删除这些元素，而是将每一个不与value相等的元素轮番赋值给first之后的空间
// 返回值标示出重新整理后的最后元素的下一位置
//因为array无法缩小尺寸，所以 remove 和 remove_if 不适用于 array
/*****************************************************************************************/
template <class ForwardIter, class UnaryPredicate>ForwardIter remove_if
(ForwardIter first, ForwardIter last, UnaryPredicate unary_pred) {
	first = mystl::find_if(first, last, unary_pred);	//利用循序查找法找出第一个相等元素
	auto next = first;									//以next标示出来
	//利用"remove_copy()允许新旧容器重叠"的性质，进行移除操作
	//并将结果置于原容器中
	return first == last ? first : mystl::remove_copy_if(++next, last, first, unary_pred);
}

/*****************************************************************************************/
// replace
// 将区间内所有的 old_value 都以 new_value 替代
/*****************************************************************************************/
template <class ForwardIter, class T>void replace
(ForwardIter first, ForwardIter last, const T& old_value, const T& new_value) {
	for (; first != last; ++first) {
		if (*first == old_value)
			*first = new_value;
	}
}

/*****************************************************************************************/
// replace_copy
// 行为与 replace 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
/*****************************************************************************************/
template <class ForwardIter, class OutputIter, class T>OutputIter replace_copy
(ForwardIter first, ForwardIter last, OutputIter result, const T& old_value, const T& new_value) {
	for (; first != last; ++first,++result) {
		//如果旧序列的元素等于old_value，就放new_value到新序列中
		//否则就将元素拷贝一份放进新序列中
		*result == *first == old_value ? new_value : *first;
	}
	return result;
}

/*****************************************************************************************/
// replace_copy_if
// 行为与 replace_if 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
/*****************************************************************************************/
template <class ForwardIter, class OutputIter, class UnaryPredicate, class T>OutputIter replace_copy_if
(ForwardIter first, ForwardIter last, OutputIter result, UnaryPredicate unary_pred, const T& new_value) {
	for (; first != last; ++first, ++result) {
		//如果旧序列的元素被unary_pred评估为true，就放new_value到新序列中
		//否则就将元素拷贝一份放进新序列中
		*result == unary_pred(*first) ? new_value : *first;
	}
	return result;
}

/*****************************************************************************************/
// replace_if
// 将区间内所有令一元操作 unary_pred 为 true 的元素都用 new_value 替代
/*****************************************************************************************/
template <class ForwardIter, class UnaryPrediacte, class T >void replace_if
(ForwardIter first, ForwardIter last, UnaryPrediacte unary_pred, const T& new_value) {
	for (; first != last; ++first) {
		if (unary_pred(*first))
			*first = *new_value;
	}
}

/*****************************************************************************************/
// reverse
// 将[first, last)区间内的元素在元容器中颠倒重排
/*****************************************************************************************/
// reverse_dispatch 的 bidirectional_iterator_tag 版本
template <class BidirectionalIter>void reverse_dispatch
(BidirectionalIter first, BidirectionalIter last, bidirectional_iterator_tag) {
	while (true) {
		if (first == last || first == -last)
			return;
		mystl::iter_swap(first++, last);	//头部元素不断换到last后面，并累进一个位置。两者交错时停止
	}
}

// reverse_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter>void reverse_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
	while (first < last)					//只有random iterators才能做first<last的判断
		mystl::iter_swap(first++, --last);	//头尾两两互换，然后头部累进一个位置，尾部累退一个位置。两者交错时停止
}

//reverse的接口
template <class BidrectionalIter>void reverse(BidrectionalIter first, BidrectionalIter last) {
	mystl::reverse_dispatch(first, last, iterator_category(first));		//判断迭代器类型
}

/*****************************************************************************************/
// reverse_copy
// 行为与 reverse 类似，不同的是将结果复制到 result 所指容器中
/*****************************************************************************************/
template <class BidirectionalIter, class OutputIter>OutputIter reverse_copy
(BidirectionalIter first, BidirectionalIter last, OutputIter result) {
	while (first != last) {		//整个序列走一遍
		--last;					
		*result = *last;		//将尾端所指元素复制到result所指位置
		++result;				
	}
	return result;
}

/*****************************************************************************************/
// random_shuffle
// 将[first, last)内的元素次序随机重排
// 即在N!种可能的元素排列顺序中随机选出一种，N为last-first
// 版本1使用内部随机数产生器
// 版本2使用一个产生随机数的函数对象 rand，传递方式是by reference
/*****************************************************************************************/
//版本1：
template <class RandomIter>void random_shuffle(RandomIter first, RandomIter last) {
	if (first == last) return;
	//以下是内部的随机产生器
	//srand() 给随机函数一个新的种子，通常随机数是通过取前一个数字（或种子）来计算的，
	//然后对该数字进行多次操作以生成下一个
	//time(0) 给出自 Unix 纪元以来的时间（以秒为单位），这是一个非常好的“不可预测”种子
	//参考：https://stackoverflow.com/questions/4736485/srandtime0-and-random-number-generation
	//		https://blog.csdn.net/kongfanyu/article/details/6387642
	srand((unsigned)time(0));
	for (auto i = first + 1; i != last; ++i) {
		mystl::iter_swap(i, first + (rand() % (i - first + 1)));	//rand()产生随机数
	}
}

//版本2：使用一个产生随机数的函数对象 rand
template <class RandomIter, class RandomNumberGenerator>void random_shuffle
(RandomIter first, RandomIter last, RandomNumberGenerator& rand) {	//rand对象是by reference
	if (first == last) return;
	auto len = mystl::distance(first, last);
	for (auto i = first + 1, i != last; ++i) {
		mystl::iter_swap(i, first + rand(i - first + 1) % len);
	}
}

/*****************************************************************************************/
// rotate
// 将[first, middle)内的元素和 [middle, last)内的元素互换，可以交换两个长度不同的区间
// middle所指的元素会成为容器的第一个元素，返回交换后 middle 的位置
/*****************************************************************************************/
// rotate_dispatch 的 forward_iterator_tag 版本
template <class ForwardIter> ForwardIter rotate_dispatch
(ForwardIter first, ForwardIter middle, ForwardIter last, forward_iterator_tag) {
	auto first2 = middle;					//取划分出的数量相等的两个段的后段，第一个元素为first2
	do {									//前段与后段哪个短，就以哪个段为依据
	mystl:; swap(*first++, *first2++);		//将前段、后段的元素一一交换
		if (first == middle)				//当first行进到middle位置就停止
			middle = first2;				//此时将first2赋给middle，即后半段移到前面
	} while (first2 != last);				// 遍历区间

	auto new_middle = first;				// 迭代器返回的位置
	first2 = middle;						//调整后段位置，此时middle为尚未进行交换的段的首个元素
	while (first2 != last) {				//后段不为空时
		mystl::swap(*first++, *first2++);	// 调整剩余元素
		//以下判断是前段[first,middle)先结束还是后段[middle,last)先结束
		if (first == middle) {				//前段结束了
			middle = first2;				//对新的前、后段再作交换
		}
		else if (first2 = last) {			//后段先结束
			first2 = middle;				//调整，准备对新的前、后段再作交换
		}
	}
	return new_middle;						//返回交换后 middle 的位置，此时middle为容器的第一个元素
}

// rotate_dispatch 的 bidirectional_iterator_tag 版本
template <class BidirectionalIter>BidirectionalIter rotate_dispatch
(BidirectionalIter first, BidirectionalIter middle, BidirectionalIter last, bidirectional_iterator_tag) {
	//以下进行分段逆转
	mystl::reverse_dispatch(first, middle, bidirectional_iterator_tag());		//前段进行逆转
	mystl::reverse_dispatch(middle, last, bidirectional_iterator_tag());		//后段进行逆转
	//以下当前、后段元素不为空时，将前段、后段整合起来进行逆转
	while (first != middle && middle != last)									
		mystl::swap(*first++, *--last);											//整段再逆转
	//以下处理剩余元素
	if (first == middle) {														//前段处理完毕时
		mystl::reverse_dispatch(middle, last, bidirectional_iterator_tag());	//后段再逆转
		return last;
	}
	else {																		//后段处理完毕时
		mystl::reverse_dispatch(first, middle, bidirectional_iterator_tag());	//前段再逆转
		return first;
	}
}

// 求最大公因子，利用辗转相除法
//rgcd()应用于 rotate_dispatch 的 random_access_iterator_tag 版本
template <class EuclideanRingElement>EuclideanRingElement rgcd(EuclideanRingElement m, EuclideanRingElement n) {
	while (n != 0) {
		auto t = m % n;		//求余
		m = n;
		n = t;
	}
	return m;
}

// rotate_dispatch 的 random_access_iterator_tag 版本
template <class RandomIter>RandomIter rotate_dispatch
(RandomIter first, RandomIter middle, RandomIter last, random_access_iterator_tag) {
	auto n = last - first;							//整段区间长度
	auto l = middle - first;						//前段区间长度
	auto r = n - l;									//后段区间长度
	auto result = first + (last - middle);			//跳跃到middle位置，作为调整后的容器的第一个元素
	if (l == r) {									//如果前、后段长度相等
		mystl::swap_ranges(first, middle, middle);	//一一对应交换元素
		return result;								//结束执行
	}
	auto cycle_times = rgcd(n, l);					//取全长和前段长度的最大公因子
	for (auto i = 0; i < cycle_times; ++i) {		//以长度的最大公因子为依据
		auto tmp = *first;
		auto p = first;
		if (l < r) {								//如果前段长度小于后段，元素进行交换
			for (auto j = 0; j < r / cycle_times; ++j) {
				if (p > first + r) {
					*p = *(p - r);
					p -= r;
				}
				*p = *(p + l);
				p += l;
			}
		}
		else {										//如果后段长度小于前段，元素进行交换
			for (auto j = 0; j < l / cycle_times - 1; ++j) {
				if (p < last - 1) {
					*p = *(p + l);
					p += l;
				}
				*p = *(p - r);
				p -= r;
			}
		}
		*p = tmp;									//回到循环，继续处理剩余元素
		++first;
	}
	return result;
}

template <class ForwardIter>ForwardIter rotate(ForwardIter first, ForwardIter middle, ForwardIter last) {
	if (first == middle)
		return last;
	if (middle == last)
		return first;
	return mystl::rotate_dispatch(first, middle, last, iterator_category());
}

/*****************************************************************************************/
// rotate_copy
// 行为与 rotate 类似，不同的是将结果复制到 result 所指的容器中
/*****************************************************************************************/
template <class ForwardIter, class OutputIter>ForwardIter rotate_copy
(ForwardIter first, ForwardIter middle, ForwardIter last, OutputIter result) {
	//先把后段复制到新容器的前端，再把前段接续复制到新容器
	//返回值middle成为新容器的第一个元素
	return mystl::copy(first, middle, mystl::copy(middle, last, result));
}

/*****************************************************************************************/
// is_permutation
// 判断[first1,last1)是否为[first2, last2)的排列组合
/*****************************************************************************************/
//辅助函数，使用自定义的二元操作符
template <class ForwardIter1, class ForwardIter2, class BinaryPred>bool is_permutation_aux
(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred) {
	//constexpr声明“常量表达式”，也就是在编译期可求值的表达式
	constexpr bool is_ra_it = mystl::is_random_access_iterator<ForwardIter1>::value
		&& mystl::is_random_access_iterator<ForwardIter2>::value;
	if (is_ra_it) {
		auto len1 = last1 - first1;			
		auto len2 = last2 - first2;
		if (len1 != len2)
			return false;
	}

	// 先找出相同的前缀段
	for (; first1 != last1 && first2 != last2; ++first1, (void)++first2) {
		if (!pred(*first1, *first2))
			break;
	}
	if (is_ra_it) {
		if (first1 == last1)
			return true;
	}
	else {
		auto len1 = mystl::distance(first1, last1);
		auto len2 = mystl::distance(first2, last2);
		if (len1 == 0 && len2 == 0)
			return true;
		if (len1 != len2)
			return false;
	}

	// 判断剩余部分
	for (auto i = first1; i != last1; ++i) {
		bool is_repeated = false;
		for (auto j = first1; j != i; ++j) {
			if (pred(*j, *i)) {
				is_repeated = true;
				break;
			}
		}

		if (!is_repeated) {
			// 计算 *i 在 [first2, last2) 的数目
			auto c2 = 0;
			for (auto j = first2; j != last2; ++j) {
				if (pred(*i, *j))
					++c2;
			}
			if (c2 == 0)
				return false;

			// 计算 *i 在 [first1, last1) 的数目
			auto c1 = 1;
			auto j = i;
			for (++j; j != last1; ++j) {
				if (pred(*i, *j))
					++c1;
			}
			if (c1 != c2)
				return false;
		}
	}
	return true;
}

//版本1：is_permutation的接口，使用自定义的二元操作符
template <class ForwardIter1, class ForwardIter2, class BinaryPred>bool is_permutation
(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred) {
	return is_permutation_aux(first1, last1, first2, last2, pred);
}

//版本2：使用函数对象
template <class ForwardIter1, class ForwardIter2>bool is_permutation
(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2) {
	//萃取型别定义
	typedef typename iterator_traits<ForwardIter1>::value_type v1;
	typedef typename iterator_traits<ForwardIter2>::value_type v2;
	//静态断言，is_same判断两者萃取型别是否相同
	static_assert(std::is_same<v1, v2>::value,
		"the type should be same in mystl::is_permutation");
	//使用mystl::equal_to()作为二元操作符
	return is_permutation_aux(first1, last1, first2, last2, mystl::equal_to(v1));
}

/*****************************************************************************************/
// next_permutation
// 取得[first, last)所标示序列的下一个排列组合，如果没有下一个排序组合，返回 false，否则返回 true
/*****************************************************************************************/
//版本1：使用operator<
template <class BidirectionalIter>bool next_permutation(BidirectionalIter first, BidirectionalIter last) {
	auto i = last;							//i指向尾端
	if (first == last || first == --i)		//空区间
		return false;
	for (;;) {
		auto ii = i;						//锁定一组(两个)相邻元素
		if (*--i < *ii) {					//如果前一个元素小于后一个元素
			auto j = last;					//令j指向尾端
			while(!(*i<*--j)){}				//由尾端往前找，直到遇到比 *i 大的元素
			mystl::iter_swap(i, j);			// 交换 i，j 所指元素
			mystl::reverse(ii, last);		// 将 ii 之后的所有元素全部逆向重排
			return true;
		}
		if (i == first) {					//行进至最前面
			mystl::reverse(first, last);	//全部逆向重排
			return false;
		}
	}
}

//版本2：使用函数对象 comp 代替比较操作
template <class BidirectionalIter, class Compared>
bool next_permutation(BidirectionalIter first, BidirectionalIter last, Compared comp)
{
	auto i = last;
	if (first == last || first == --i)
		return false;
	for (;;)
	{
		auto ii = i;
		if (comp(*--i, *ii))
		{
			auto j = last;
			while (!comp(*i, *--j)) {}
			mystl::iter_swap(i, j);       // 交换 i，j 所指元素
			mystl::reverse(ii, last);     // 将 ii 之后的所有元素反转
			return true;
		}
		if (i == first)
		{
			mystl::reverse(first, last);
			return false;
		}
	}
}

/*****************************************************************************************/
// prev_permutation
// 取得[first, last)所标示序列的上一个排列组合，如果没有上一个排序组合，返回 false，否则返回 true
/*****************************************************************************************/
//版本1：
template <class BidirectionalIter>bool prev_permutation(BidirectionalIter first, BidirectionalIter last) {
	auto i = last;								//i指向尾端
	if (first == last || first == --last)		//空区间
		return false;
	for (;;) {
		auto ii = i;							//锁定一组(两个)相邻元素
		if (*ii < *--i) {						//如果前一个元素大于后一个元素
			auto j = last;						//令j指向尾端
			while (!(*--j < *i)) {};			//由尾端往前找，直到遇到比 *i 小的元素
			mystl::iter_swap(i, j);				// 交换 i，j 所指元素
			mystl::reverse(ii, last);			// 将 ii 之后的所有元素全部逆向重排
			return true;
		}
		if (i == first) {						//行进至最前面
			mystl::reverse(first, last);		//全部逆向重排
			return false;
		}
	}
}

// 版本2：使用函数对象 comp 代替比较操作
template <class BidirectionalIter, class Compared>
bool prev_permutation(BidirectionalIter first, BidirectionalIter last, Compared comp)
{
	auto i = last;
	if (first == last || first == --i)
		return false;
	for (;;)
	{
		auto ii = i;
		if (comp(*ii, *--i))
		{
			auto j = last;
			while (!comp(*--j, *i)) {}
			mystl::iter_swap(i, j);       // 交换i，j
			mystl::reverse(ii, last);     // 将 ii 之后的所有元素反转
			return true;
		}
		if (i == first)
		{
			mystl::reverse(first, last);
			return false;
		}
	}
}

/*****************************************************************************************/
// merge
// 将两个经过排序的集合 S1 和 S2 合并起来置于另一段空间，返回一个迭代器指向最后一个元素的下一位置
/*****************************************************************************************/
//版本1：
template <class InputIter1, class InputIter2, class OutputIter>OutputIter merge
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
	while (first1!last1 && first2 != last2) {	//两个序列尚未走完
		if (*first2 < *first1) {				//序列二的元素比较小
			*result = *first2;					//登记序列二的元素
			++first2;							//序列二前进1
		}
		else {									//序列二的元素比较大
			*result = *first1;					//登记序列一的元素
			++first1;							//序列一前进1
		}
		++result;
	}
	//最后剩余元素以copy复制到目的端，以下两个序列一定至少有一个为空
	return mystl::copy(first2, last2, mystl::copy(first1, last1, result));
}

//版本2：使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class OutputIter, class Compared>
OutputIter
merge(InputIter1 first1, InputIter1 last1,
	InputIter2 first2, InputIter2 last2,
	OutputIter result, Compared comp)
{
	while (first1 != last1 && first2 != last2)
	{
		if (comp(*first2, *first1))				//使用comp进行比较
		{
			*result = *first2;
			++first2;
		}
		else
		{
			*result = *first1;
			++first1;
		}
		++result;
	}
	return mystl::copy(first2, last2, mystl::copy(first1, last1, result));
}

/*****************************************************************************************/
// inplace_merge 就地归并排序
// 把连接在一起的两个有序序列[first,middle)和[middle,last)结合成单一序列并保持有序
//稳定操作，如果两个子序列有相同元素，第一序列的元素会被排在第二序列之前
/*****************************************************************************************/
// 辅助函数，没有缓冲区的情况下的就地归并排序
template <class BidirectionalIter, class Distance>void merge_without_buffer
(BidirectionalIter first, BidirectionalIter middle, BidirectionalIter last, Distance len1, Distance len2) {
	if (len1 == 0 || len2 == 0) return;				//只要有一个序列为空，就结束执行
	if (len1 + len2 == 2) {							//如果两个序列各自只有一个元素
		if (*middle < *first)						//而且first大于middle
			mystl::iter_swap(first, middle);		//进行元素互换，使序列保持递增排列
		return;										//如果first不大于middle，结束执行
	}
	auto first_cut = first;
	auto second_cut = middle;
	Distance len11 = 0;
	Distance len22 = 0;
	if (len1 > len2) {								// 序列一较长
		len11 = len1 >> 1;							//除以2
		mystl::advance(first_cut, len11);			//找到序列一的中点
		second_cut = mystl::lower_bound(middle, last, *first_cut);	//在序列二寻找刚好大于或等于*first_cut的位置
		len22 = mystl::distance(middle, second_cut);	//求出len22的长度，等下进行递归调用
	}
	else {											// 序列二较长
		len22 = len2 >> 1;							//除以2
		mystl::advance(second_cut, len22);			//找到序列二的中点
		first_cut = mystl::upper_bound(first, middle, *second_cut);	//在序列一寻找刚好大于或等于*second_cut的位置
		len11 = mystl::distance(first, first_cut);		//求出len11的长度，等下进行递归调用
	}
	auto new_middle = mystl::rotate(first_cut, middle, second_cut);		//对first_cut和second_cut之间元素进行旋转操作
	mystl::merge_without_buffer(first, first_cut, new_middle, len11, len22);				//针对左段，递归调用
	mystl::merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22);	//针对右段，递归调用
}

//逆向归并排序，作为有缓冲区情况下的就地归并排序的辅助函数
template <class BidirectionalIter1, class BidirectionalIter2>BidirectionalIter1 merge_backward
(BidirectionalIter1 first1,BidirectionalIter1 last1,BidirectionalIter2 first2,BidirectionalIter2 last2,
	BidirectionalIter1 result){
	if (first1 == last1)										//序列一为空
		return mystl::copy_backward(first2, last2, result);		//逆向拷贝序列二
	if (first2 == last2)										//序列二为空
		return mystl::copy_backward(first1, last1, result);		//逆向拷贝序列一
	--last1;													//从尾端开始遍历
	--last2;
	while (true) {												//遍历区间
		if (*last2 < *last1) {									//序列二元素小于序列一元素
			*--result = *last1;									//将序列一元素登记
			if (first1 == last1)								//直到序列一为空
				return mystl::copy_backward(first2, ++last2, result);	//拷贝剩余的序列二元素到result
			--last1;											//序列一减1
		}
		else {													//序列一元素小于序列二元素
			*--result = *last2;									//将序列二元素登记
			if (first2 == last2)								//直到序列二为空
				return mystl::copy_backward(first1, ++last1, result);	//拷贝剩余的序列一元素到result
			--last2;											//序列二减1
		}
	}
}

//针对缓冲区优化的旋转函数，作为有缓冲区情况下的就地归并排序的辅助函数
template <class BidirectionalIter1, class BidirectionalIter2, class Distance>BidirectionalIter1 rotate_adaptive
(BidirectionalIter1 first, BidirectionalIter1 middle, BidirectionalIter1 last, Distance len1, Distance len2,
	BidirectionalIter2 buffer, Distance buffer_size) {
	BidirectionalIter2 buffer_end;
	if (len1 > len2 && len2 <= buffer_size) {					//缓冲区足够安置序列二(较短)
		buffer_end = mystl::copy(middle, last, buffer);			//序列二拷贝到缓冲区
		mystl::copy_backward(first, middle, last);				//逆向排列序列一
		return mystl::copy(buffer, buffer_end, first);			//将序列二元素(位于缓冲区)拷贝回去，放置在序列一之前
	}
	else if (len1 <= buffer_size) {								//缓冲区足够安置序列一(较短)
		buffer_end = mystl::copy(first, middle, buffer);		//序列一拷贝到缓冲区
		mystl::copy_backward(middle,last,first);				//逆向排列序列二
		return mystl::copy_backward(buffer, buffer_end, last);	//将序列一元素(位于缓冲区)拷贝回去，放置在序列二之后
	}
	else {
		return mystl::rotate(first, middle, last);				//缓冲区空间不足，改用rotate算法(不需缓冲区)
	}
}

// 辅助函数，有缓冲区的情况下就地合并排序
template <class BidrectionalIter, class Distance, class Pointer>void merge_adaptive
(BidrectionalIter first, BidrectionalIter middle, BidrectionalIter last, Distance len1, Distance len2,
	Pointer buffer, Distance buffer_size) {
	if (len1 <= len2 && len1 <= buffer_size) {							//缓冲区足够安置序列一(较短)
		Pointer buffer_end = mystl::copy(first, middle, buffer);		//拷贝序列一到缓冲区
		mystl::merge(buffer, buffer_end, middle, last, first);			//将缓冲区的序列一元素与序列二元素归并排序
	}
	else if (len2 <= buffer_size) {										//缓冲区足够安置序列二(较短)
		Pointer buffer_end = mystl::copy(middle, last, buffer);			//拷贝序列二到缓冲区
		mystl::merge_backward(first, middle, buffer, buffer_end, last);//将缓冲区的序列二元素与序列一元素逆向归并排序
	}
	else {										//缓冲区空间不足安置任何一个序列，进行分割递归处理
		auto first_cut = first;
		auto second_cut = middle;
		Distance len11 = 0;
		Distance len22 = 0;
		if (len1 > len2) {						//序列一比较长
			len11 = len1 >> 1;					
			mystl::advance(first_cut, len11);
			second_cut = mystl::lower_bound(middle, last, *first_cut);
			len22 = mystl::distance(middle, second_cut);
		}
		else {									//序列二比较长
			len22 = len2 >> 1;
			mystl::advance(second_cut, len22);
			first_cut = mystl::upper_bound(first, middle, *second_cut);
			len11 = mystl::distance(first, first_cut);
		}
		//此时左段长度与右段长度相等，针对分割出的剩余元素进行旋转操作
		auto new_middle = mystl::rotate_adaptive(first_cut, middle, second_cut, len1 - len11, len22, buffer, buffer_size);
		//针对左段，递归调用
		mystl::merge_adaptive(first, first_cut, new_middle, len11, len22, buffer, buffer_size);
		//针对右段，递归调用
		mystl::merge_adaptive(new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size);
	}
}

//辅助函数，就地归并排序的具体调用
template <class BidirectionalIter, class T>void inplace_merge_aux
(BidirectionalIter first, BidirectionalIter middle, BidirectionalIter last, T*) {
	auto len1 = mystl::distance(first, middle);							//序列一的长度
	auto len2 = mystl::distance(middle, last);							//序列二的长度
	temporary_buffer<BidirectionalIter, T> buf(first, last);			//使用临时缓冲区
	if (!buf.begin()) {													//内存配置失败
		mystl::merge_without_buffer(first, middle, last, len1, len2);	//调用无缓冲区情况下的归并排序
	}
	else {																//内存配置成功
		//调用有缓冲区情况下的归并排序
		mystl::merge_adaptive(first, middle, last, len1, len2, buf.begin(), buf.size()); 
	}
}

//inplace_merge()的接口
template <class BidirectionalIter>void inplace_merge
(BidirectionalIter first, BidirectionalIter middle, BidirectionalIter last) {
	if (first == middle || middle == last)
		return;
	mystl::inplace_merge_aux(first, middle, last, value_type(first));
}


//版本2：使用函数对象 comp 代替比较操作
// 没有缓冲区的情况下合并
template <class BidirectionalIter, class Distance, class Compared>
void merge_without_buffer(BidirectionalIter first, BidirectionalIter middle,
	BidirectionalIter last, Distance len1, Distance len2,
	Compared comp)
{
	if (len1 == 0 || len2 == 0)
		return;
	if (len1 + len2 == 2)
	{
		if (comp(*middle, *first))
			mystl::iter_swap(first, middle);
		return;
	}
	auto first_cut = first;
	auto second_cut = middle;
	Distance len11 = 0;
	Distance len22 = 0;
	if (len1 > len2)
	{
		len11 = len1 >> 1;
		mystl::advance(first_cut, len11);
		second_cut = mystl::lower_bound(middle, last, *first_cut, comp);
		len22 = mystl::distance(middle, second_cut);
	}
	else
	{
		len22 = len2 >> 1;
		mystl::advance(second_cut, len22);
		first_cut = mystl::upper_bound(first, middle, *second_cut, comp);
		len11 = mystl::distance(first, first_cut);
	}
	auto new_middle = mystl::rotate(first_cut, middle, second_cut);
	mystl::merge_without_buffer(first, first_cut, new_middle, len11, len22, comp);
	mystl::merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22, comp);
}

template <class BidirectionalIter1, class BidirectionalIter2, class Compared>
BidirectionalIter1
merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
	BidirectionalIter2 first2, BidirectionalIter2 last2,
	BidirectionalIter1 result, Compared comp)
{
	if (first1 == last1)
		return mystl::copy_backward(first2, last2, result);
	if (first2 == last2)
		return mystl::copy_backward(first1, last1, result);
	--last1;
	--last2;
	while (true)
	{
		if (comp(*last2, *last1))
		{
			*--result = *last1;
			if (first1 == last1)
				return mystl::copy_backward(first2, ++last2, result);
			--last1;
		}
		else
		{
			*--result = *last2;
			if (first2 == last2)
				return mystl::copy_backward(first1, ++last1, result);
			--last2;
		}
	}
}

// 有缓冲区的情况下合并
template <class BidirectionalIter, class Distance, class Pointer, class Compared>
void merge_adaptive(BidirectionalIter first, BidirectionalIter middle,
	BidirectionalIter last, Distance len1, Distance len2,
	Pointer buffer, Distance buffer_size, Compared comp)
{
	// 区间长度足够放进缓冲区
	if (len1 <= len2 && len1 <= buffer_size)
	{
		Pointer buffer_end = mystl::copy(first, middle, buffer);
		mystl::merge(buffer, buffer_end, middle, last, first, comp);
	}
	else if (len2 <= buffer_size)
	{
		Pointer buffer_end = mystl::copy(middle, last, buffer);
		mystl::merge_backward(first, middle, buffer, buffer_end, last, comp);
	}
	else
	{  // 区间长度太长，分割递归处理
		auto first_cut = first;
		auto second_cut = middle;
		Distance len11 = 0;
		Distance len22 = 0;
		if (len1 > len2)
		{
			len11 = len1 >> 1;
			mystl::advance(first_cut, len11);
			second_cut = mystl::lower_bound(middle, last, *first_cut, comp);
			len22 = mystl::distance(middle, second_cut);
		}
		else
		{
			len22 = len2 >> 1;
			mystl::advance(second_cut, len22);
			first_cut = mystl::upper_bound(first, middle, *second_cut, comp);
			len11 = mystl::distance(first, first_cut);
		}
		auto new_middle = mystl::rotate_adaptive(first_cut, middle, second_cut, len1 - len11,
			len22, buffer, buffer_size);
		mystl::merge_adaptive(first, first_cut, new_middle, len11,
			len22, buffer, buffer_size, comp);
		mystl::merge_adaptive(new_middle, second_cut, last, len1 - len11,
			len2 - len22, buffer, buffer_size, comp);
	}
}

template <class BidirectionalIter, class T, class Compared>
void
inplace_merge_aux(BidirectionalIter first, BidirectionalIter middle,
	BidirectionalIter last, T*, Compared comp)
{
	auto len1 = mystl::distance(first, middle);
	auto len2 = mystl::distance(middle, last);
	temporary_buffer<BidirectionalIter, T> buf(first, last);
	if (!buf.begin())
	{
		mystl::merge_without_buffer(first, middle, last, len1, len2, comp);
	}
	else
	{
		mystl::merge_adaptive(first, middle, last, len1, len2, buf.begin(), buf.size(), comp);
	}
}

template <class BidirectionalIter, class Compared>
void
inplace_merge(BidirectionalIter first, BidirectionalIter middle,
	BidirectionalIter last, Compared comp)
{
	if (first == middle || middle == last)
		return;
	mystl::inplace_merge_aux(first, middle, last, value_type(first), comp);
}


/*****************************************************************************************/
// partial_sort
// 对整个序列做部分排序，保证较小的 N 个元素以递增顺序置于[first, first + N)中
//接受一个middle迭代器，使序列中的middle-first个最小元素以递增顺序排序，置于[first,middle)内
//其余last-middle个元素安置于[middle,last)中，不保证有任何特定顺序
//算法内部使用HeapSort堆排序来完成
/*****************************************************************************************/
template <class RandomIter>void partial_sort(RandomIter first, RandomIter middle, RandomIter last) {
	mystl::make_heap(first, middle);			//界定[first,middle)去构造大根堆
	for (auto i = middle; i < last; ++i) {		//i<last 判断操作只适合Random Iterator
		if (*i < *first) {						//将[middle,last)的每一个元素与大根堆的最大值比较
			//如果小于该最大值，就互换位置并重新保持大根堆的状态
			mystl::pop_heap_aux(first, middle, i, *i, distance_type(first));
		}
	}
	//以上循环走完时，较大的元素都已经被抽离出[first,middle)
	//再以堆排序将[first,middle)做一次排序，保证较小的 N 个元素以递增顺序置于[first, first + N)中
	mystl::sort_heap(first, middle);
}

// 版本2：使用函数对象 comp 代替比较操作
template <class RandomIter, class Compared>
void partial_sort(RandomIter first, RandomIter middle,
	RandomIter last, Compared comp)
{
	mystl::make_heap(first, middle, comp);
	for (auto i = middle; i < last; ++i)
	{
		if (comp(*i, *first))
		{
			mystl::pop_heap_aux(first, middle, i, *i, distance_type(first), comp);
		}
	}
	mystl::sort_heap(first, middle, comp);
}

/*****************************************************************************************/
// partial_sort_copy
// 行为与 partial_sort 类似，不同的是把排序结果复制到 result 容器中
/*****************************************************************************************/
//版本1：
//辅助函数
template <class InputIter, class RandomIter, class Distance>RandomIter psort_copy_aux
(InputIter first, InputIter last, RandomIter result_first, RandomIter result_last, Distance*) {
	if (result_first == result_last)
		return result_last;
	auto result_iter = result_first;
	while (first != last && result_iter != result_last) {	//遍历区间，将元素拷贝到result
		*result_iter = *first;
		++result_iter;
		++first;
	}
	mystl::make_heap(result_first, result_iter);			//界定[result_first,result_iter)去构造大根堆
	while (first != last) {
		if (*first < *result_first) {						//将[first,last)的每一个元素与大根堆的最大值比较
			//如果小于该最大值，就互换位置并重新保持大根堆的状态
			mystl::adjust_heap(result_first, static_cast<Distance>(0), result_iter - result_first, *first);
		}
		++first;
	}
	//以上循环走完时，较大的元素都已经被抽离出result
	//再以堆排序将result做一次排序
	mystl::sort_heap(result_first, result_iter);
	return result_iter;
}

//partial_sort_copy的接口
template <class InputIter, class RandomIter>RandomIter partial_sort_copy
(InputIter first, InputIter last, RandomIter result_first, RandomIter result_last) {
	return mystl::psort_copy_aux(first, last, result_first, result_last, distance_type(result_first));
}

// 重载版本使用函数对象 comp 代替比较操作
template <class InputIter, class RandomIter, class Distance, class Compared>
RandomIter
psort_copy_aux(InputIter first, InputIter last,
	RandomIter result_first, RandomIter result_last,
	Distance*, Compared comp)
{
	if (result_first == result_last)
		return result_last;
	auto result_iter = result_first;
	while (first != last && result_iter != result_last)
	{
		*result_iter = *first;
		++result_iter;
		++first;
	}
	mystl::make_heap(result_first, result_iter, comp);
	while (first != last)
	{
		if (comp(*first, *result_first))
		{
			mystl::adjust_heap(result_first, static_cast<Distance>(0),
				result_iter - result_first, *first, comp);
		}
		++first;
	}
	mystl::sort_heap(result_first, result_iter, comp);
	return result_iter;
}

template <class InputIter, class RandomIter, class Compared>
RandomIter
partial_sort_copy(InputIter first, InputIter last,
	RandomIter result_first, RandomIter result_last,
	Compared comp)
{
	return mystl::psort_copy_aux(first, last, result_first, result_last,
		distance_type(result_first), comp);
}


/*****************************************************************************************/
// partition 分割
// 对区间内的元素重排，被一元条件运算判定为 true 的元素会放到区间的前段
// 将整个序列分割为两部分，前半部分为被被一元条件运算判定为 true 的元素，后半段为被判定为false的元素
// 该函数不保证元素的原始相对位置
/*****************************************************************************************/
template <class BidirectionalIter, class UnaryPredicate>BidirectionalIter pratition
(BidirectionalIter first, BidirectionalIter last, UnaryPredicate unary_pred) {
	while (true) {
		while (first != last && unary_pred(*first)) {	//first找到不符合一元条件运算符的元素的就停下来
			++first;									//否则就不断向尾部移动
		}	
		if (first == last)								//first到达末端就结束执行
			break;
		--last;											//调整last的位置，向中央逼近，继续循环
		while (first != last && !unary_pred(*last)) {	//last找到符合一元条件运算符的元素就停下来
			--last;										//否则就继续向头部移动
		}
		if (first == last)								//first到达末端就结束执行
			break;
		mystl::iter_swap(first, last);		//当first与last交错时，如果first仍然在左，last仍然在右，就将两者元素互换
		++first;										//调整first的位置，向中央逼近，继续循环
	}
	return first;		//直到两者交错，即last在左，first在右，就结束循环，以此时的first为轴，将序列切割为左右两段	
}

/*****************************************************************************************/
// partition_copy
// 行为与 partition 类似，不同的是，将被一元操作符判定为 true 的放到 result_true 的输出区间
// 其余放到 result_false 的输出区间，并返回一个 mystl::pair 指向这两个区间的尾部
/*****************************************************************************************/
template <class InputIter,class OutputIter1,class OutputIter2,class UnaryPredicate>
mystl::pair<OutputIter1, OutputIter2> partition_copy
(InputIter first, InputIter last, OutputIter1 result_true, OutputIter2 result_false, UnaryPredicate unary_pred) {
	for (; first != last; ++first) {
		if (unary_pred(*first)) {
			result_true++ = *first;
		}
		else {
			*result_false++ = *first;
		}
	}
	return mystl::pair<OutputIter1, OutputIter2>(result_true, result_false);
}


/*****************************************************************************************/
// sort
// 将[first, last)内的元素以递增的方式排序
//此算法使用RandomAccess Iterator，因为迭代器必须能随机定位，为了快速取出QuickSort的中央元素
//容器中只有属于RandomAccess Iterators的vector和deque适合使用sort算法
//其他容器各有各不适合使用sort算法的原因
/*****************************************************************************************/
constexpr static size_t kSmallSectionSize = 128;	// 全局常数，表示小型区间的大小，在这个大小内采用插入排序

//slg2() 用于控制分割恶化的情况
//找出2^k<=n 的最大值
template <class Size>Size slg2(Size n) {
	Size k = 0;
	for (; n > 1; n >> 1)		//n>1时，不断除以2
		++k;					//求出k的最大值
	return k;
}

//版本1：
// 分割函数 unchecked_partition
template <class RandomIter, class T>RandomIter unchecked_partition
(RandomIter first, RandomIter last, const T& pivot) {	//以pivot为切分序列的枢轴元素
	while (true) {
		while (*first < pivot)				//first找到>=pivot的元素就停下来
			++first;						//否则就不断向尾部移动
		--last;								//调整last的位置，向中央逼近，继续循环
		while (pivot < *last)				//last找到<=pivot的元素就停下来
			--last;							//否则就继续向头部移动
		if (!first < last)					//两个迭代器交错，表示整个序列调整完毕，结束循环
			return first;					//以此时的first为轴，将序列切割为左右两段
		mystl::iter_swap(first, last);		//当first与last交错时，如果first仍然在左，last仍然在右，就将两者元素互换
		++first;							//调整first的位置，向中央逼近，继续循环
	}
}

// introSort 内省式排序，先进行 quick sort，当分割行为有恶化倾向时，改用 heap sort
//本函数内的许多迭代器运算操作，都只适合Random Iterators
template <class RandomIter, class Size>void intro_sort
(RandomIter first, RandomIter last, Size depth_limit) {		//depth_limit表示允许分割序列数目的最大值
	while (static_cast<size_t>(last - first) > kSmallSectionSize) {	// 判断序列大小，即如果元素个数>128bytes(16字节)
		if (depth_limit == 0) {								 // 检查分割层次，如果到达最大分割深度限制
			mystl::partial_sort(first, last, last);			 //改用堆排序
			return;
		}
		--depth_limit;
		//在first、middle、last之间三点取中值，选择一个好的枢轴并决定分割点
		auto mid = mystl::median(*(first), *(first + (last - first) / 2), *(last - 1));
		//分割点落在迭代器cut身上
		auto cut = mystl::unchecked_partition(first, last, mid);
		//对右半段递归进行排序
		mystl::intro_sort(cut, last, depth_limit);
		last = cut;
		//现在回到while循环，准备对左半段递归进行排序
	}
}

// 插入排序辅助函数 unchecked_linear_insert
template <class RandomIter, class T>void unchecked_linear_insert(RandomIter last, const T& value) {
	auto next = last;			//记录尾元素
	--next;
	//insertion sort的内循环
	//注意：一旦不再出现逆转对，循环就可以结束
	while (value < *next) {		//逆转对存在
		*last = *next;			//调整
		last = next;			//调整迭代器
		--next;					//左移一个位置
	}
	*last = value;				//value的正确落脚处
}

// 插入排序函数 unchecked_insertion_sort
template <class RandomIter> void unchecked_insertion_sort(RandomIter first, RandomIter last) {
	for (auto i = first; i != last; ++i) {
		mystl::unchecked_linear_insert(i, *i);	//调用unchecked_linear_insert
	}
}

// 插入排序函数 insertion_sort
template <class RandomIter>void insertion_sort(RandomIter first, RandomIter last) {
	if (first == last)
		return;
	for (auto i = first + 1; i != last; ++i) {
		auto value = *i;
		if (value < *first) {						//尾比头还小(注意：头端必为最小元素)
			mystl::copy_backward(first, i, i + 1);	//将整个区间向右递移一个位置
			*first = value;							//令头元素等于原先的尾元素值
		}
		else {										//尾不小于头
			mystl::unchecked_linear_insert(i, value);
		}
	}
}

// 最终插入排序函数 final_insertion_sort
template <class RandomIter>void final_insertion_sort(RandomIter first, RandomIter last) {
	if (static_cast<size_t>(last - first) > kSmallSectionSize) {	//元素个数>128bytes，即>16字节
		//将[first,last)分割为长度为16字节的一段子序列，和另一段剩余子序列
		//针对两个子序列分别调用insertion_sort和unchecked_insertion_sort
		mystl::insertion_sort(first, first + kSmallSectionSize);	
		mystl::unchecked_insertion_sort(first + kSmallSectionSize, last);
	}
	else															//元素个数<128
	{
		mystl::insertion_sort(first, last);							//调用insetion_sort进行处理
	}
}

template <class RandomIter>void sort(RandomIter first, RandomIter last) {
	if (first != last) {
		// 内省式排序，将区间分为一个个小区间，然后对整体进行插入排序
		mystl::intro_sort(first, last, slg2(last - first) * 2);
		mystl::final_insertion_sort(first, last);
	}
}

// 版本2：使用函数对象 comp 代替比较操作
// 分割函数 unchecked_partition
template <class RandomIter, class T, class Compared>
RandomIter
unchecked_partition(RandomIter first, RandomIter last,
	const T& pivot, Compared comp)
{
	while (true)
	{
		while (comp(*first, pivot))
			++first;
		--last;
		while (comp(pivot, *last))
			--last;
		if (!(first < last))
			return first;
		mystl::iter_swap(first, last);
		++first;
	}
}

// 内省式排序，先进行 quick sort，当分割行为有恶化倾向时，改用 heap sort
template <class RandomIter, class Size, class Compared>
void intro_sort(RandomIter first, RandomIter last,
	Size depth_limit, Compared comp)
{
	while (static_cast<size_t>(last - first) > kSmallSectionSize)
	{
		if (depth_limit == 0)
		{                            // 到达最大分割深度限制
			mystl::partial_sort(first, last, last, comp);  // 改用 heap_sort
			return;
		}
		--depth_limit;
		auto mid = mystl::median(*(first), *(first + (last - first) / 2), *(last - 1));
		auto cut = mystl::unchecked_partition(first, last, mid, comp);
		mystl::intro_sort(cut, last, depth_limit, comp);
		last = cut;
	}
}

// 插入排序辅助函数 unchecked_linear_insert
template <class RandomIter, class T, class Compared>
void unchecked_linear_insert(RandomIter last, const T& value, Compared comp)
{
	auto next = last;
	--next;
	while (comp(value, *next))
	{  // 从尾部开始寻找第一个可插入位置
		*last = *next;
		last = next;
		--next;
	}
	*last = value;
}

// 插入排序函数 unchecked_insertion_sort
template <class RandomIter, class Compared>
void unchecked_insertion_sort(RandomIter first, RandomIter last,
	Compared comp)
{
	for (auto i = first; i != last; ++i)
	{
		mystl::unchecked_linear_insert(i, *i, comp);
	}
}

// 插入排序函数 insertion_sort
template <class RandomIter, class Compared>
void insertion_sort(RandomIter first, RandomIter last, Compared comp)
{
	if (first == last)
		return;
	for (auto i = first + 1; i != last; ++i)
	{
		auto value = *i;
		if (comp(value, *first))
		{
			mystl::copy_backward(first, i, i + 1);
			*first = value;
		}
		else
		{
			mystl::unchecked_linear_insert(i, value, comp);
		}
	}
}

// 最终插入排序函数 final_insertion_sort
template <class RandomIter, class Compared>
void final_insertion_sort(RandomIter first, RandomIter last, Compared comp)
{
	if (static_cast<size_t>(last - first) > kSmallSectionSize)
	{
		mystl::insertion_sort(first, first + kSmallSectionSize, comp);
		mystl::unchecked_insertion_sort(first + kSmallSectionSize, last, comp);
	}
	else
	{
		mystl::insertion_sort(first, last, comp);
	}
}

template <class RandomIter, class Compared>
void sort(RandomIter first, RandomIter last, Compared comp)
{
	if (first != last)
	{
		// 内省式排序，将区间分为一个个小区间，然后对整体进行插入排序
		mystl::intro_sort(first, last, slg2(last - first) * 2, comp);
		mystl::final_insertion_sort(first, last, comp);
	}
}

/*****************************************************************************************/
// nth_element
// 对序列重排，使迭代器nth所指元素，与"整个[first,last)完整排序后，同一位置的元素"同值
//并保证[nth,last)内没有任何一个元素不大于[first,nth)内的元素
//算法只接受Random Interator，并且不保证序列的元素次序，因此效率更快
/*****************************************************************************************/
//版本1：
template <class RandomIter>void nth_element(RandomIter first, RandomIter nth, RandomIter last) {
	if (nth == last)
		return;
	//不断分割序列为不同的子序列，直到子序列长度不大于3(够小了)就进行Insertion Sort
	while (last - first > 3) {	//长度超过3
		//采用median()寻找中值元素，返回迭代器指向分割后的右段第一个元素
		auto cut = mystl::unchecked_partition(first, last, mystl::median(*first, *(first + (last - first) / 2),
			*(last - 1)));
		if (cut <= nth)			// 如果右段起点<=指定位置(nth 位于右段)
			first = cut;		//再对右段实施分割
		else					//否则(nth落在左段)
			last = cut;			// 对左段进行分割
	}
	mystl::insertion_sort(first, last);		//对长度不大于3的子序列做insertion_sort
}

// 版本2：使用函数对象 comp 代替比较操作
template <class RandomIter, class Compared>
void nth_element(RandomIter first, RandomIter nth,
	RandomIter last, Compared comp)
{
	if (nth == last)
		return;
	while (last - first > 3)
	{
		auto cut = mystl::unchecked_partition(first, last, mystl::median(*first,
			*(first + (last - first) / 2),
			*(last - 1)), comp);
		if (cut <= nth)  // 如果 nth 位于右段
			first = cut;   // 对右段进行分割
		else
			last = cut;    // 对左段进行分割
	}
	mystl::insertion_sort(first, last, comp);
}

/*****************************************************************************************/
// unique_copy
// 从[first, last)中将元素复制到 result 上，序列必须有序，如果有重复的元素，只会复制一次
/*****************************************************************************************/
//版本1：
// unique_copy_dispatch 的 forward_iterator_tag 版本
template <class InputIter, class ForwardIter>ForwardIter unique_copy_dispatch
(InputIter first, InputIter last, ForwardIter result, forward_iterator_tag) {
	*result = *first;				//记录第一个元素
	while (++first != last) {		//遍历区间
		//以下元素不同就记录，否则(元素相同)就跳过
		if (*result != *first)
			*++result = *first;
	}
	return ++result;
}

// unique_copy_dispatch 的 output_iterator_tag 版本
// 由于 output iterator 只能进行只读操作，所以不能有 *result != *first 这样的判断
template <class InputIter, class OutputIter>OutputIter unique_copy_dispatch
(InputIter first, InputIter last, OutputIter result, ouput_iterator_tag) {
	auto value = *first;
	*result = value;				//记录第一个元素
	while (++first != last) {		//遍历区间
		//以下元素不同就记录，否则(元素相同)就跳过
		if (value != *first) {
			value = *first;
			*++result = value;
		}
	}
	return ++result;
}

//unique_copy的接口
template <class InputIter, class OutputIter>OutputIter unique_copy
(InputIter first, InputIter last, OutputIter result) {
	if (first == last)
		return result;
	return mystl::unique_copy_dispatch(first, last, iterator_category(result));
}

// 版本2：使用函数对象 comp 代替比较操作
// unique_copy_dispatch 的 forward_iterator_tag 版本
template <class InputIter, class ForwardIter, class Compared>
ForwardIter
unique_copy_dispatch(InputIter first, InputIter last,
	ForwardIter result, forward_iterator_tag, Compared comp)
{
	*result = *first;
	while (++first != last)
	{
		if (!comp(*result, *first))
			*++result = *first;
	}
	return ++result;
}

// unique_copy_dispatch 的 output_iterator_tag 版本
// 由于 output iterator 只能进行只读操作，所以不能有 *result != *first 这样的判断
template <class InputIter, class OutputIter, class Compared>
OutputIter
unique_copy_dispatch(InputIter first, InputIter last,
	OutputIter result, output_iterator_tag, Compared comp)
{
	auto value = *first;
	*result = value;
	while (++first != last)
	{
		if (!comp(value, *first))
		{
			value = *first;
			*++result = value;
		}
	}
	return ++result;
}

template <class InputIter, class OutputIter, class Compared>
OutputIter
unique_copy(InputIter first, InputIter last, OutputIter result, Compared comp)
{
	if (first == last)
		return result;
	return mystl::unique_copy_dispatch(first, last, result, iterator_category(result), comp);
}


/*****************************************************************************************/
// unique
// 移除[first, last)内重复的元素，序列必须有序，和 remove 类似，它也不能真正的删除重复元素
//注意：unique只移除相邻的重复元素，如果想要移除所有(包括不相邻)重复元素，必须先将序列排序，使所有重复元素相邻
//返回一个迭代器，指向新区间的尾端
/*****************************************************************************************/
template <class ForwardIter>ForwardIter unique(ForwardIter first, ForwardIter last) {
	first = mystl::adjacent_find(first, last);			//找到相邻重复元素的起点
	return mystl::unique_copy(first, last, first);		//利用unique_copy完成
}

// 版本2：使用函数对象 comp 代替比较操作
template <class ForwardIter, class Compared>
ForwardIter unique(ForwardIter first, ForwardIter last, Compared comp)
{
	first = mystl::adjacent_find(first, last, comp);
	return mystl::unique_copy(first, last, first, comp);
}

/*****************************************************************************************/
// mergesort 归并排序
//利用"分治思想"，对区间对半分割，左右两段各自排序，再利用inplace_merge重新组合为一个完整的有序序列
//对半分割的操作可以递归进行，直到每一小段的长度为0或1(该小段也就自动完成排序)
/*****************************************************************************************/
template <class BidirectionalIter>void mergesort(BidirectionalIter first, BidirectionalIter last) {
	auto n = distance(first, last);
	if (n == 0 || n == 1)
		return;
	else
	{
		BidirectionalIter mid = first + n / 2;
		mergesort(first, mid);
		mergesort(mid, last);
		inplace_merge(first, mid, last);
	}
}

}	//命名空间结束符

#ifdef _MSC_VER
#pragma warning(pop)		//#pragma warning(pop)是恢复原先的警告状态
#endif

#endif // !MYSTL_ALGO_H_

