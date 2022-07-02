#ifndef MYSTL_HEAP_ALGO_H_
#define MYSTL_HEAP_ALGO_H_

// 这个头文件包含 heap 的四个算法 : push_heap, pop_heap, sort_heap, make_heap
//以大根堆实现

#include "iterator.h"	//迭代器

namespace mystl {	

/*****************************************************************************************/
// push_heap
// 该函数接受两个迭代器，表示一个 heap 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
/*****************************************************************************************/

template<class RandomIter, class Distance, class T>
void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value) {
	auto parent = (holeIndex - 1) / 2;		//找出父节点
	while (holeIndex > topIndex && *(first + parent) < value) {	//尚未到达顶端，且父节点小于新值(不符合heap特性)
		// 使用 operator<，所以 heap 为 max-heap
		*(first + holeIndex) = *(first + parent);	//令洞值为父值
		holeIndex = parent;				//调整洞号，向上提升至父节点
		parent = (holeIndex - 1) / 2;	//新洞的父节点
	}	//持续至顶端，或满足heap的次序特性为止
	*(first + holeIndex) = value;	//令洞值为新值，完成插入操作
}

template <class RandomIter, class Distance>
void push_heap_d(RandomIter first, RandomIter last, Distance*)
{
	mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
}

template <class RandomIter>
void push_heap(RandomIter first, RandomIter last)
{ // 新元素应该已置于底部容器的最尾端
	mystl::push_heap_d(first, last, distance_type(first));
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class Distance, class T, class Compared>
void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value,
	Compared comp)
{
	auto parent = (holeIndex - 1) / 2;
	while (holeIndex > topIndex && comp(*(first + parent), value))
	{
		*(first + holeIndex) = *(first + parent);
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;
	}
	*(first + holeIndex) = value;
}

template <class RandomIter, class Compared, class Distance>
void push_heap_d(RandomIter first, RandomIter last, Distance*, Compared comp)
{
	mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0),
		*(last - 1), comp);
}

template <class RandomIter, class Compared>
void push_heap(RandomIter first, RandomIter last, Compared comp)
{
	mystl::push_heap_d(first, last, distance_type(first), comp);
}

/*****************************************************************************************/
// pop_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap 的根节点取出放到容器尾部，调整 heap
/*****************************************************************************************/

template <class RandomIter, class T, class Distance>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value) {
	// 先进行下溯(percolate down)过程
	auto topIndex = holeIndex;
	auto rchild = 2 * holeIndex + 2;	//洞节点之右子节点
	while (rchild < len) {
		if (*(first + rchild) < *(first + rchild - 1))	//比较洞节点之左右两个子值，然后以rchild代表较大子节点
			--rchild;
		*(first + holeIndex) = *(first + rchild);	//令较大子值为洞值，再令洞号下移至较大子节点处
		holeIndex = rchild;
		rchild = 2 * (rchild + 1);	//找出新洞节点的右子节点
	}
	if (rchild == len) {	// 如果没有右子节点，只有左子节点
		*(first + holeIndex) = *(first + (rchild)-1);	//令左子值为洞值，再令洞号下移至左子节点处
		holeIndex = rchild - 1;
	}
	// 再执行一次上溯(percolate up)过程
	mystl::push_heap_aux(first, holeIndex, topIndex, value);
}

template <class RandomIter, class T, class Distance>
void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value,
	Distance*)
{
	// 先将首值调至尾节点，然后调整[first, last - 1)使之重新成为一个 max-heap
	*result = *first;
	mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

template <class RandomIter>
void pop_heap(RandomIter first, RandomIter last)
{
	mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class T, class Distance, class Compared>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value,
	Compared comp)
{
	// 先进行下溯(percolate down)过程
	auto topIndex = holeIndex;
	auto rchild = 2 * holeIndex + 2;
	while (rchild < len)
	{
		if (comp(*(first + rchild), *(first + rchild - 1)))  --rchild;
		*(first + holeIndex) = *(first + rchild);
		holeIndex = rchild;
		rchild = 2 * (rchild + 1);
	}
	if (rchild == len)
	{
		*(first + holeIndex) = *(first + (rchild - 1));
		holeIndex = rchild - 1;
	}
	// 再执行一次上溯(percolate up)过程
	mystl::push_heap_aux(first, holeIndex, topIndex, value, comp);
}

template <class RandomIter, class T, class Distance, class Compared>
void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result,
	T value, Distance*, Compared comp)
{
	*result = *first;  // 先将尾指设置成首值，即尾指为欲求结果
	mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
}

template <class RandomIter, class Compared>
void pop_heap(RandomIter first, RandomIter last, Compared comp)
{
	mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1),
		distance_type(first), comp);
}

/*****************************************************************************************/
// sort_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap 操作，直到首尾最多相差1
/*****************************************************************************************/
template <class RandomIter>
void sort_heap(RandomIter first, RandomIter last) {
	// 每执行一次 pop_heap，最大的元素都被放到尾部，直到容器最多只有一个元素，完成排序
	while (last - first > 1) {
		mystl::pop_heap(first, last--);
	}
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class Compared>
void sort_heap(RandomIter first, RandomIter last, Compared comp)
{
	while (last - first > 1)
	{
		mystl::pop_heap(first, last--, comp);
	}
}

/*****************************************************************************************/
// make_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
/*****************************************************************************************/
template <class RandomIter, class Distance>
void make_heap_aux(RandomIter first, RandomIter last, Distance*) {
	if (last - first < 2) {		//如果长度为0或1，不必重新排列
		return;
	}
	auto len = last - first;
	auto holeIndex = (len - 2) / 2;	//找出第一个需要重排的子树头部，以 holeIndex标示出
	while (true) {
		// 重排以 holeIndex 为首的子树，len是为了让adjust_heap判断操作范围
		mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
		if (holeIndex == 0) {	//走完根节点，就结束
			return;
		}
		holeIndex--;	//（即将重排之子树的）头部向前一个节点
	}
}

template <class RandomIter>
void make_heap(RandomIter first, RandomIter last)
{
	mystl::make_heap_aux(first, last, distance_type(first));;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class Distance, class Compared>
void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compared comp)
{
	if (last - first < 2)
		return;
	auto len = last - first;
	auto holeIndex = (len - 2) / 2;
	while (true)
	{
		// 重排以 holeIndex 为首的子树
		mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
		if (holeIndex == 0)
			return;
		holeIndex--;
	}
}

template <class RandomIter, class Compared>
void make_heap(RandomIter first, RandomIter last, Compared comp)
{
	mystl::make_heap_aux(first, last, distance_type(first), comp);
}


}	//命名空间结束符

#endif // !MYSTL_HEAP_ALGO_H_

