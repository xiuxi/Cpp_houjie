#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

// ���ͷ�ļ����ڵ�������ƣ�������һЩģ��ṹ����ȫ�ֺ���

#include<cstddef>			//������һЩ���õĳ������ꡢ���ͺͺ���
#include<type_traits.h>		//��ȡ��

namespace mystl {	//�����ռ�mystl

//���ֵ���������
struct input_iterator_tag{};	//��ָ���󣬲��������ı䡣ֻ��
struct ouput_iterator_tag{};	//Ψд
struct forward_iterator_tag:public input_iterator_tag{};	//�������д
struct bidirectional_iterator_tag:public forward_iterator_tag{};	//��˫���ƶ�
struct random_access_iterator_tag:public bidirectional_iterator_tag{};	//��������ָ�����������

//Ϊ�˷��Ϲ淶���κε�������Ӧ���ṩ�����Ƕ��Ӧ�ͱ�������traits��ȡ��
//���STL�ṩiterators class�ı�׼��ʽ�����е��������̳�������֤����STL����淶
//������iterators class�ı�׼��ʽ
template<class Category,class T,class Distance=ptrdiff_t,class Pointer=T*,class Reference=T&>
struct iterator {				
	//������������Ƕ�ͱ�,typedef���ڶ������͵��±���
	typedef Category iterator_category;	//����һ����ʱ�������ͱ��������������֮һ����������������ͱ���е���
	typedef T value_type;				//value_typeָ��������ָ������ͱ�
	typedef Distance difference_type;	//��ʾ����������֮��ľ��룬��C++�ڽ���ptrdiff_t��Ϊԭ��ָ���difference_type
	typedef Pointer pointer;			//ָ��
	typedef Reference reference;		//reference����
};

// iterator traits

template <class T>struct has_iterator_cat		//�ж��Ƿ���iterator_category
{
private:
	struct two { char a; char b; };
	template <class U> static two test(...);
	template <class U> static char test(typename U::iterator_category* = 0);
public:
	static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template<class Iterator, bool>struct iterator_traits_impl {};	//iterator traits�ķ����汾

template<class Iterator, bool>struct iterator_traits_impl<Iterator, true> {	//iterator traits���ػ��汾
	//������ȡ��traits���ݴ���ĵ�������ȡ��Ӧ�������ͱ�
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;
};

//����helper������ͨ��������������̽�⵽T::iterator�Ĵ���,������T�Ƿ���һ��iterator,
//�����,����ȡ����iterator_category,����������ģ��ƥ��ʧ��,�ų������ؾ���
//����һ��Ĭ�ϵĿյ�iterator_traits_helper,��ƫ�ػ�һ����������ȡ��iterator_traits_helper
//�ο����ӣ�https://www.blurredcode.com/2019/04/452-1/

template <class Iterator, bool>struct iterator_traits_helper {};	//has_typedef_iterator�ķ����汾

template <class Iterator>
struct iterator_traits_helper<Iterator, true>						//has_typedef_iterator���ػ��汾
	: public iterator_traits_impl<Iterator,
	std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
	std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>{};

//��ȡ������������,����iterator_traitsʱ,��������has_typedef_iterator���������Ƿ����iterator�����Ƕ�ͱ�
template <class Iterator>struct iterator_traits
	:public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

//���ԭ��ָ�����Ƶ�traitsƫ�ػ��汾
template <class T>struct iterator_traits<T*> {
	typedef random_access_iterator_tag	iterator_category;
	typedef T							value_type;
	typedef ptrdiff_t					difference_type;
	typedef T*							pointer;
	typedef T&							reference;
};

//���ԭ��֮pointer-to-const����Ƶ�traitsƫ�ػ��汾
template<class T>struct iterator_traits<const T*> {
	typedef random_access_iterator_tag	iterator_category;
	typedef T							value_type;
	typedef ptrdiff_t					difference_type;
	typedef const T*					pointer;
	typedef const T&					reference;
};

//�ж��Ƿ���iterator_category���̳�type_traits.h�ĸ�������ģ��
//��������iterator_categoryʱ�����ж��ǲ���ĳһiterator����
template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
	: public m_bool_constant<std::is_convertible<
	typename iterator_traits<T>::iterator_category, U>::value>{};

//��������ʱֱ��Ϊ�̳�false_type
template <class T,class U>struct has_iterator_cat_of<T,U,false>:public m_false_type{};

//��ȡĳ�ֵ�����
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

//�ж��Ƿ��ǵ�����
template <class Iterator>struct is_iterator :
	public m_bool_constant<is_input_iterator<Iterator>::value ||
	is_output_iterator<Iterator>::value>{};

//��ȡĳ���������� category
template <class Iterator>typename iterator_traits<Iterator>::iterator_category 
iterator_category(const Iterator&) {
	typedef typename iterator_traits<Iterator>::iterator_category Category;
	return Category();
}

//��ȡĳ����������distance_type����������ָ���������ж�����
template <class Iterator>typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&) {
	return staic_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

//��ȡĳ����������value_type����������ָ���������ж�����
template <class Iterator>inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
	return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//���º������ڼ����������ľ���

//distance��input_iterator_tag�汾
template <class InputIterator>typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
	typename iterator_traits<InputIterator>::difference_type n = 0;
	while (first != last) {
		//��һ�ۼƾ���
		++first;
		++n;
	}
	return n;
}

//distance��random_access_iterator_tag �İ汾
template <class RandomIter>typename iterator_traits<RandomIter>::difference_type
distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
	return last - first;	//ֱ�Ӽ�����
}

//distance��category�汾������distance_dispatch()ȥ�жϵ������ֵ������汾
template <class InputIterator>typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
	return distance_dispatch(first, last, iterator_category(first));
}

//���º��������õ�����ǰ�� n ������

//advance �� input_iterator_tag �İ汾
template <class InputIterator, class Distance>void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag) {
	while (n--)		//������һǰ��
		++i;
}

//advance �� bidirectional_iterator_tag �İ汾
template <class BidirectionalIterator,class Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
	//˫����һǰ��
	if (n >= 0)
		while (n--)++i;
	else
		while (n++)--i;
}

// advance �� random_access_iterator_tag �İ汾
template <class RandomIter,class Distance>
	void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag) {
		i += n;	//˫����Ծǰ��
}

//advance��category�汾������advance_dispatch()ȥ�жϵ������ֵ������汾
template <class InputIterator, class Distance>void advance(InputIterator& i, Distance n) {
		advance_dispatch(i, n, iterator_category(i));
}

/*****************************************************************************************/

// ģ���� : reverse_iterator,�������������ʹǰ��Ϊ���ˣ�����Ϊǰ��
template <class Iterator>class reverse_iterator {
private:
	Iterator current;	//��¼��Ӧ�����������
public:
	//�����������������Ӧ�ͱ�
	typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
	typedef typename iterator_traits<Iterator>::value_type        value_type;
	typedef typename iterator_traits<Iterator>::difference_type   difference_type;
	typedef typename iterator_traits<Iterator>::pointer           pointer;
	typedef typename iterator_traits<Iterator>::reference         reference;

	//����������뷴�����������
	typedef Iterator					iterator_type;		
	typedef reverse_iterator<Iterator>	self;

public:
	//���캯��
	reverse_iterator(){}
	explicit reverse_iterator(iterator_type i):current(i){}
	reverse_iterator(const self& rhs):current(rhs.current){}

public:
	//ȡ����Ӧ�����������
	iterator_type base()const { return current; }

	//���ز�����
	reference operator*()const {
		//ʵ�ʶ�Ӧ�����������ǰһ��λ��
		auto tmp = current;	
		return *--tmp;
	}
	pointer operator->()const {
		return &(operator*());
	}

	//ǰ��(++)��Ϊ����
	self& operator++() {		//ǰ��++
		--current;
		return *this;
	}
	self operator++(int) {		//����++
		self tmp = *this;
		--current;
		return tmp;
	}
	//����(--)��Ϊǰ��
	self& operator--() {
		++current;
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		++current;
		return tmp;
	}

	//����+=,+,-=,-,[]
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
		
};	//class������

//����operator-
template<class Iterator>typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
	return ths.base() - rhs.base();
}

//���رȽ������
template <class Iterator>bool operator==
(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
	return lhs.base() == rhs.base();
}

//����reverse_iterator��Ϊ������lhs < rhs => lhs.current > rhs.current
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
	return rhs < lhs;		//operator>,����<
}

template <class Iterator>bool operator<=
(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
	return !(rhs < lhs);	//operator<=,����>=
}

template <class Iterator>bool operator>=
(const reverse_iterator<Iterator>& lhs,const reverse_iterator<Iterator>& rhs)
{
	return !(lhs < rhs);	
}

}		//�����ռ������

#endif // !MYSTL_ITERATOR_H_

