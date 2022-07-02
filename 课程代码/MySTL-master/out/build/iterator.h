#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数

#include<cstddef>			//定义了一些常用的常量、宏、类型和函数
#include<type_traits.h>		//萃取机

namespace mystl {	//命名空间mystl

//五种迭代器类型
struct input_iterator_tag{};	//所指对象，不允许外界改变。只读
struct ouput_iterator_tag{};	//唯写
struct forward_iterator_tag:public input_iterator_tag{};	//允许读与写
struct bidirectional_iterator_tag:public forward_iterator_tag{};	//可双向移动
struct random_access_iterator_tag:public bidirectional_iterator_tag{};	//涵盖所有指针的算术能力

//为了符合规范，任何迭代器都应该提供五个内嵌相应型别，以利于traits萃取。
//因此STL提供iterators class的标准范式，所有迭代器都继承它，保证符合STL所需规范
//以下是iterators class的标准范式
template<class Category,class T,class Distance=ptrdiff_t,class Pointer=T*,class Reference=T&>
struct iterator {				
	//以下是五种内嵌型别,typedef用于定义类型的新别名
	typedef Category iterator_category;	//产生一个临时对象，其型别隶属五个迭代器之一，编译器根据这个型别进行调用
	typedef T value_type;				//value_type指迭代器所指对象的型别
	typedef Distance difference_type;	//表示两个迭代器之间的距离，以C++内建的ptrdiff_t作为原生指针的difference_type
	typedef Pointer pointer;			//指针
	typedef Reference reference;		//reference引用
};

// iterator traits

template <class T>struct has_iterator_cat		//判断是否有iterator_category
{
private:
	struct two { char a; char b; };
	template <class U> static two test(...);
	template <class U> static char test(typename U::iterator_category* = 0);
public:
	static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template<class Iterator, bool>struct iterator_traits_impl {};	//iterator traits的泛化版本

template<class Iterator, bool>struct iterator_traits_impl<Iterator, true> {	//iterator traits的特化版本
	//特性萃取机traits根据传入的迭代器萃取相应的五种型别
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;
};

//定义helper函数，通过帮助函数可以探测到T::iterator的存在,如果检测T是否是一个iterator,
//如果是,则萃取它的iterator_category,如果不是这个模版匹配失败,排除出重载决议
//定义一个默认的空的iterator_traits_helper,再偏特化一个真正的萃取器iterator_traits_helper
//参考链接：https://www.blurredcode.com/2019/04/452-1/

template <class Iterator, bool>struct iterator_traits_helper {};	//has_typedef_iterator的泛化版本

template <class Iterator>
struct iterator_traits_helper<Iterator, true>						//has_typedef_iterator的特化版本
	: public iterator_traits_impl<Iterator,
	std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
	std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>{};

//萃取迭代器的特性,调用iterator_traits时,我们利用has_typedef_iterator来检验它是否包含iterator这个内嵌型别
template <class Iterator>struct iterator_traits
	:public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

//针对原生指针而设计的traits偏特化版本
template <class T>struct iterator_traits<T*> {
	typedef random_access_iterator_tag	iterator_category;
	typedef T							value_type;
	typedef ptrdiff_t					difference_type;
	typedef T*							pointer;
	typedef T&							reference;
};

//针对原生之pointer-to-const而设计的traits偏特化版本
template<class T>struct iterator_traits<const T*> {
	typedef random_access_iterator_tag	iterator_category;
	typedef T							value_type;
	typedef ptrdiff_t					difference_type;
	typedef const T*					pointer;
	typedef const T&					reference;
};

//判断是否有iterator_category，继承type_traits.h的辅助别名模板
//当存在有iterator_category时用于判断是不是某一iterator类型
template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
	: public m_bool_constant<std::is_convertible<
	typename iterator_traits<T>::iterator_category, U>::value>{};

//当不存在时直接为继承false_type
template <class T,class U>struct has_iterator_cat_of<T,U,false>:public m_false_type{};

//萃取某种迭代器
//input_iterator
template <class Iter>struct is_input_iterator:public has_iterator_cat_of<Iter,input_iterator_tag>{};

//output_iterator
template <class Iter>struct is_output_iterator: public has_iterator_cat_of<Iter, output_iterator_tag> {};

//forward_iterator
template <class Iter>struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

//bidirectional_iterator
template <class Iter>
struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

//random_access_iterator
template <class Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

//判断是否是迭代器
template <class Iterator>struct is_iterator :
	public m_bool_constant<is_input_iterator<Iterator>::value ||
	is_output_iterator<Iterator>::value>{};

//萃取某个迭代器的 category
template <class Iterator>typename iterator_traits<Iterator>::iterator_category 
iterator_category(const Iterator&) {
	typedef typename iterator_traits<Iterator>::iterator_category Category;
	return Category();
}

//萃取某个迭代器的distance_type，返回类型指针有助于判断类型
template <class Iterator>typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&) {
	return staic_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

//萃取某个迭代器的value_type，返回类型指针有助于判断类型
template <class Iterator>inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
	return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//以下函数用于计算迭代器间的距离

//distance的input_iterator_tag版本
template <class InputIterator>typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
	typename iterator_traits<InputIterator>::difference_type n = 0;
	while (first != last) {
		//逐一累计距离
		++first;
		++n;
	}
	return n;
}

//distance的random_access_iterator_tag 的版本
template <class RandomIter>typename iterator_traits<RandomIter>::difference_type
distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
	return last - first;	//直接计算差距
}

//distance的category版本，返回distance_dispatch()去判断调用哪种迭代器版本
template <class InputIterator>typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
	return distance_dispatch(first, last, iterator_category(first));
}

//以下函数用于让迭代器前进 n 个距离

//advance 的 input_iterator_tag 的版本
template <class InputIterator, class Distance>void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag) {
	while (n--)		//单向，逐一前进
		++i;
}

//advance 的 bidirectional_iterator_tag 的版本
template <class BidirectionalIterator,class Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
	//双向，逐一前进
	if (n >= 0)
		while (n--)++i;
	else
		while (n++)--i;
}

// advance 的 random_access_iterator_tag 的版本
template <class RandomIter,class Distance>
	void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag) {
		i += n;	//双向，跳跃前进
}

//advance的category版本，返回advance_dispatch()去判断调用哪种迭代器版本
template <class InputIterator, class Distance>void advance(InputIterator& i, Distance n) {
		advance_dispatch(i, n, iterator_category(i));
}

/*****************************************************************************************/

// 模板类 : reverse_iterator,代表反向迭代器，使前进为后退，后退为前进
template <class Iterator>class reverse_iterator {
private:
	Iterator current;	//记录对应的正向迭代器
public:
	//反向迭代器的五种相应型别
	typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
	typedef typename iterator_traits<Iterator>::value_type        value_type;
	typedef typename iterator_traits<Iterator>::difference_type   difference_type;
	typedef typename iterator_traits<Iterator>::pointer           pointer;
	typedef typename iterator_traits<Iterator>::reference         reference;

	//定义迭代器与反向迭代器别名
	typedef Iterator					iterator_type;		
	typedef reverse_iterator<Iterator>	self;

public:
	//构造函数
	reverse_iterator(){}
	explicit reverse_iterator(iterator_type i):current(i){}
	reverse_iterator(const self& rhs):current(rhs.current){}

public:
	//取出对应的正向迭代器
	iterator_type base()const { return current; }

	//重载操作符
	reference operator*()const {
		//实际对应正向迭代器的前一个位置
		auto tmp = current;	
		return *--tmp;
	}
	pointer operator->()const {
		return &(operator*());
	}

	//前进(++)变为后退
	self& operator++() {		//前置++
		--current;
		return *this;
	}
	self operator++(int) {		//后置++
		self tmp = *this;
		--current;
		return tmp;
	}
	//后退(--)变为前进
	self& operator--() {
		++current;
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		++current;
		return tmp;
	}

	//重载+=,+,-=,-,[]
	self& operator+=(difference_type n) {
		current -= n;
		return *this;
	}
	self operator+(difference_type n)const {
		return self(current - n);
	}
	self& operator-=(difference_type n) {
		current += n;
		return *this;
	}
	self operator-(difference_type n)const {
		return self(current + n);
	}
	reference operator[](difference_type n)const {
		return *(*this + n);
	}
		
};	//class结束符

//重载operator-
template<class Iterator>typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
	return ths.base() - rhs.base();
}

//重载比较运算符
template <class Iterator>bool operator==
(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
	return lhs.base() == rhs.base();
}

//由于reverse_iterator在为反序，则lhs < rhs => lhs.current > rhs.current
template <class Iterator>bool operator<
	(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
	return rhs.base() < lhs.base();
}

template <class Iterator>bool operator!=
(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
	return !(lhs == rhs);
}

template <class Iterator>bool operator>
(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
	return rhs < lhs;		//operator>,返回<
}

template <class Iterator>bool operator<=
(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
	return !(rhs < lhs);	//operator<=,返回>=
}

template <class Iterator>bool operator>=
(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
	return !(lhs < rhs);	
}

}		//命名空间结束符

#endif // !MYSTL_ITERATOR_H_

