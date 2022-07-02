#ifndef MYSTL_ALGOBASE_H_
#define MYSTL_ALGOBASE_H_

// ���ͷ�ļ������� mystl �Ļ����㷨

#include <cstring>				//��������char*�ַ����Ŀ⺯��

#include "iterator.h"			//������
#include "util.h"				//ͨ�ù���

namespace mystl {

#ifdef max
//Ԥ����ָ�if not define ���δ����������������
#pragma message("#undefing marco max")
// ��ӡ��ȡ���궨��
#undef max
#endif // max
//��������ĳ���ؼ��ĺ��Ƿ���ȷ�������ˡ�ͨ��if...�ж�->message "��ӡ"
//�ο�:https://www.jianshu.com/p/e0caa3beb416
#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

/*****************************************************************************************/
// equal���������������[first,last)����ȣ�����true
// �Ƚϵ�һ������ [first, last)�����ϵ�Ԫ��ֵ�Ƿ�͵ڶ��������
//����ڶ����е�Ԫ�رȽ϶࣬�������Ԫ�ز��迼��
//���ϣ����֤����������ȫ��ȣ��ڵ���equal()ʱ�������ж�Ԫ�ظ����Ƿ���ͬ
/*****************************************************************************************/
//�汾1
template <class InputIter1, class InputIter2>bool equal
(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
	for (; first1 != last1; ++first1, ++first2) {
		if (*first1 != *first2) {	//��ӦԪ�ز����
			return false;	//����false
		}	
		return true;	//���򷵻�true
	}
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
template <class InputIter1, class InputIter2, class Compared>bool equal
(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!comp(*first1, *first2))
			return false;
	}
	return true;
}

/*****************************************************************************************/
// fill_n���� first λ�ÿ�ʼ��� n ��ֵ�����صĵ�����ָ����������һ��Ԫ�ص���һλ��
/*****************************************************************************************/
//�汾1������
template <class OutputIter, class Size, class T>OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value) {
	for (; n > 0; --n, ++first) {	//����n��Ԫ��
		*first = value;				//�趨��ֵ
	}
	return first;
}

//�汾2��Ϊ one-byte �����ṩ�ػ��汾
//std::enable_if����������ʱ������Ч������SFINAE�������������������ش����ؽ������ƶ�������
// ��Ϊ��ͬ�����������ṩ�����ĺ������غ��ػ�
template <class Tp,class Size,class Up>typename std::enable_if<	
	std::is_integral<Tp>::value && sizeof(Tp) == 1 &&	//std::is_integral���T�Ƿ�����������
	!std::is_same<Tp, bool>::value &&					//std::is_same�ж������Ƿ�һ��
	std::is_integral<Up>::value && sizeof(Up) == 1,
	Tp*>::type
	unchecked_fill_n(Tp* first, Size n, Up value) {
	if (n > 0) {
		std::memset(first, (unsigned char)value, (size_t)(n));	
		//std::memset��char�ͳ�ʼ������
		//����һ������ָ���ĳһ���ڴ��е�ǰn���ֽڵ�����ȫ������Ϊ�ڶ�����ָ����ASCIIֵ�� 
		//��һ��ֵΪָ�����ڴ��ַ���ڶ�����Ϊָ����ֵ����Ĵ�С�ɵ���������ָ����
		//�������ͨ��Ϊ��������ڴ�����ʼ�������� �䷵��ֵΪָ���һ������ָ�룬
		//���ǶԽϴ�Ľṹ�������������������һ����췽��
		//�ο���https://www.cnblogs.com/Tang-tangt/p/9342103.html
	}
	return first + n;
}

//fill_n�Ľӿ�
template <class OutputIter, class Size, class T>OutputIter fill_n(OutputIter first, Size n, const T& value) {
	return unchecked_fill_n(first, n, value);
}

/*****************************************************************************************/
// fill: Ϊ [first, last)�����ڵ�����Ԫ�������ֵ
/*****************************************************************************************/
//forward_iterator�汾
template <class ForwardIter, class T>void fill_cat
(ForwardIter first, ForwardIter last, const T& value, mystl::forward_iterator_tag) {
	for (; first != last; ++first) {
		*first = value;		//�趨��ֵ
	}
}

//random_iterator�汾
template <class RandomIter, class T>void fill_cat
(RandomIter first, RandomIter last, const T& value, mystl::random_access_iterator_tag) {
	fill_n(first, last - first, value);	//����fill_n()
}

//fill�Ľӿ�
template <class ForwardIter, class T>void fill(ForwardIter first, ForwardIter last, const T& value) {
	fill_cat(first, last, value, iterator_category(first));	//�жϵ��������ͣ����ò�ͬfill()ʵ��
}

/*****************************************************************************************/
// iter_swap
// ��������������ָ����Ե�
/*****************************************************************************************/
template <class FIter1, class FIter2> void iter_swap(FIter1 lhs, FIter2 rhs) {
	mystl::swap(*lhs, *rhs);
}

/*****************************************************************************************/
// swap
// ���������������������
/*****************************************************************************************/
template<class T>void swap(T& a, T& b) {
	T tmp = a;
	a = b;
	b = tmp;
}


/*****************************************************************************************/
// lexicographical_compare
// ���ֵ������ж��������н��бȽϣ�����ĳ��λ�÷��ֵ�һ�鲻���Ԫ��ʱ�������м��������
// (1)�����һ���е�Ԫ�ؽ�С������ true �����򷵻� false
// (2)������� last1 ����δ���� last2 ���� true
// (3)������� last2 ����δ���� last1 ���� false
// (4)���ͬʱ���� last1 �� last2 ���� false������һ�������ֵ����з�ʽ���Բ�С�ڵڶ�����
/*****************************************************************************************/
//�汾1
template <class InputIter1, class InputIter2> bool lexicographical_compare
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2) {
	//���£��κ�һ�����е���β�ˣ��ͽ��������������о���ӦԪ��һһ���бȶ�
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (*first1 < *first2)		//��һ����Ԫ��С�ڵڶ����е���ӦԪ��ֵ
			return true;
		if (*first2 < *first1)		//�ڶ�����Ԫ��С�ڵ�һ���е���ӦԪ��ֵ
			return false;
		//�����������������������ʾ��ֵ��ȣ��ͽ�����һ����ӦԪ��ֵ�ĶԱ�
	}
	//���е���������һ���е���β�˶��ڶ�������������ô��һ����С�ڵڶ�����
	return first1 == last1 && first2 != last2;
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
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

//�汾3����� const unsigned char* ���ػ��汾
bool lexicographical_compare(const unsigned char* first1,
	const unsigned char* last1,
	const unsigned char* first2,
	const unsigned char* last2) {
	const auto len1 = last1 - first1;
	const auto len2 = last2 - first2;
	// �ȱȽ���ͬ���ȵĲ���
	////std::memcmp��C��׼��������unsigned char��ʽ���Ƚ�����������һһ��Ӧ��ÿ��bytes
	const auto result = std::memcmp(first1, first2, mystl::min(len1, len2));	
	// ����ȣ����Ƚϳ��ı���Ϊ�Ƚϴ�
	return result != 0 ? result < 0 : len1 < len2;
}

/*****************************************************************************************/
// max
// ȡ�����еĽϴ�ֵ���������ʱ��֤���ص�һ������
/*****************************************************************************************/
//�汾1
template <class T> const T& max(const T& lhs, const T& rhs) {
	return lhs < rhs ? rhs : lhs;
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
template <class T, class Compare>const T& max(const T& lhs, const T& rhs, Compare comp) {
	return comp(lhs, rhs) ? rhs : lhs;
}

/*****************************************************************************************/
// min 
// ȡ�����еĽ�Сֵ���������ʱ��֤���ص�һ������
/*****************************************************************************************/
//�汾1
template <class T>const T& min(const T& lhs, const T& rhs) {
	return rhs < lhs ? rhs : lhs;
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
template <class T, class Compare>const T& min(const T& lhs, const T& rhs, Compare comp) {
	return comp(rhs, lhs) ? rhs : lhs;
}

/*****************************************************************************************/
// mismatch
// ƽ�бȽ��������У��ҵ���һ����ƥ���Ԫ�أ�����һ�Ե��������ֱ�ָ�����������в�ƥ���Ԫ��
/*****************************************************************************************/
//�汾1
template <class InputIter1,class InputIter2>mtstl::pair<InputIter1, InputIter2>mismatch
(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
	//�����һ�������꣬�ͽ���
	//�����һ���к͵ڶ����ж�Ӧ��Ԫ����ȣ��ͽ���
	while (first1!last1&&*first1==*first2)
	{
		++first1;
		++first2;
	}
	return mystl::pair<InputIter1, InputIter2>(first1, first2);
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
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
// �� [first, last)�����ڵ�Ԫ�ؿ����� [result, result + (last - first))��
/*****************************************************************************************/
// input_iterator_tag �汾
template <class InputIter, class OutputIter>OutputIter unchecked_copy_cat
(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag) {
	//�Ե�������ͬ��񣬾���ѭ���Ƿ�������ٶ���
	for (; first != last; ++first, ++result) {
		*result = *first;
	}
	return result;
}

// ramdom_access_iterator_tag �汾
template <class RandomIter, class OutputIter> OutputIter unchecked_copy_cat
(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag) {
	//��n����ѭ����ִ�д������ٶȼ���
	for (auto n = last - first; n > 0; --n, ++first, ++result) {
		*result = *first;
	}
	return result;
}

//unchecked_copy�����Ľӿ�
template <class InputIter, class OutputIter> OutputIter unchecked_copy
(InputIter first, InputIter last, OutputIter result) {
	return unchecked_copy_cat(first, last, result, mystl::iterator_category(first));	//�жϵ���������
}

// Ϊ trivially_copy_assignable �����ṩ�ػ��汾
template <class Tp, class Up>typename std::enable_if <
	std::is_same<typename std::remove_const<Tp>::type,Up>::value &&
	std::is_trivially_copy_assignable<Up>::value,
	Up*>::type
	unchecked_copy(Tp* first, Tp* last, Up* result) {
	const auto n = static_cast<size_t>(last - first);
	if (n != 0)
		std::memmove(result, first, n * sizeof(Up));	//std::memmove:����ٵ��ڴ�Կ�����
	return result + n;
}

//copy�Ľӿ�
template <class InputIter, class OutputIter>OutputIter copy(InputIter first, InputIter last, OutputIter result) {
	return unchecked_copy(first, last, result);
}

/*****************************************************************************************/
// copy_backward�������з��򿽱�
// �� [first, last)�����ڵ�Ԫ�ؿ����� [result - (last - first), result)��
/*****************************************************************************************/
//bidirectional_iterator_tag �汾
template <class BidirectionalIter1,class BidirectionalIter2>BidirectionalIter2 
unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result, mystl::bidirectional_iterator_tag) {
	//�Ե�������ͬ��񣬾���ѭ���Ƿ�������ٶ���
	while (first!=last)
	{
		*--result = *--last;	//���򿽱�
	}
	return result;
}

//random_access_iterator_tag �汾
template <class BidirectionalIter1, class BidirectionalIter2>BidirectionalIter2 
unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result, mystl::random_access_iterator_tag) {
	//��n����ѭ����ִ�д������ٶȼ���
	for (auto n = last - firs; n > 0; --n)
		*--result = *--last;
	return result;
}

//unchecked_copy_backward�Ľӿ�
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result)
{
	return unchecked_copy_backward_cat(first, last, result,
		iterator_category(first));		//�жϵ���������
}

// Ϊ trivially_copy_assignable �����ṩ�ػ��汾
template <class Tp, class Up>
typename std::enable_if<
	std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
	std::is_trivially_copy_assignable<Up>::value,
	Up*>::type
	unchecked_copy_backward(Tp* first, Tp* last, Up* result) {
	const auto n = static_cast<size_t>(last - first);
	if (n != 0) {
		result -= n;	//����
		std::memmove(result, first, n * sizeof(Up));	//std::memmove:����ٵ��ڴ�Կ�����
	}
	return result;
}

//copy_backward�Ľӿ�
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
{
	return unchecked_copy_backward(first, last, result);
}


/*****************************************************************************************/
// copy_if
// ��[first, last)������һԪ���� unary_pred ��Ԫ�ؿ������� result Ϊ��ʼ��λ����
/*****************************************************************************************/
template <class InputIter,class OutputIter,class UnaryPredicate>
OutputIter copy_if(InputIter first, InputIter last, OutputIter result, UnaryPredicate unary_pred) {
	for (; first != last; ++first) {
		if (unary_pred(*first)) {
			*result++ = *first;		//������result��result++
		}
	}
	return result;
}

/*****************************************************************************************/
// copy_n
// �� [first, first + n)�����ϵ�Ԫ�ؿ����� [result, result + n)��
// ����һ�� pair �ֱ�ָ�򿽱�������β��
/*****************************************************************************************/
//input_iterator�汾
template <class InputIter, class Size, class OutputIter>mystl::pair<InputIter, OutputIter> unchecked_copy_n
(InputIter first, Size n, OutputIter result, mystl::input_iterator_tag) {
	for (; n > 0; --n, ++first, ++result)
		*result = *first;
	return mystl::pair<InputIter, OutputIter>(first, result);
}

//random_iterator�汾
template <class RandomIter, class Size, class OutputIter>mystl::pair<InputIter, OutputIter> unchecked_copy_n
(RandomIter first, Size n, OutputIter result, mystl::random_access_iterator_tag) {
	auto last = first + n;
	return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));	//ʹ��copy����
}

// unchecked_copy�Ľӿ�
template <class InputIter, class Size, class OutputIter>
mystl::pair<InputIter, OutputIter>
copy_n(InputIter first, Size n, OutputIter result) {
	return unchecked_copy_n(first, n, result, mystl::iterator_category(first));	//�жϵ���������
}

/*****************************************************************************************/
// move
// �� [first, last)�����ڵ�Ԫ���ƶ��� [result, result + (last - first))��
/*****************************************************************************************/
// input_iterator_tag �汾
template <class InputIter, class OutputIter>OutputIter unchecked_move_cat
(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag) {
	//�Ե�������ͬ��񣬾���ѭ���Ƿ�������ٶ���
	for (; first != last; ++first, ++result) {
		*result = mystl::move(*first);
	}
	return result;
}

// random_access_iterator_tag �汾
template <class RandomIter, class OutputIter>OutputIter unchecked_move_cat
(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag) {
	//��n����ѭ����ִ�д������ٶȼ���
	for (auto n = last - first; n > 0; --n, ++first, ++result) {
		*result = mystl::move(*first);
	}
	return result;
}

//unchecked_move�Ľӿ�
template <class InputIter, class OutputIter>
OutputIter
unchecked_move(InputIter first, InputIter last, OutputIter result)
{
	return unchecked_move_cat(first, last, result, iterator_category(first));	//�жϵ���������
}

// Ϊ trivially_copy_assignable �����ṩ�ػ��汾
template <class Tp, class Up>
typename std::enable_if<
	std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
	std::is_trivially_move_assignable<Up>::value,
	Up*>::type
	unchecked_move(Tp* first, Tp* last, Up* result) {
	const size_t n = static_cast<size_t>(last - first);
	if (n != 0) {
		std::memmove(result, first, n * sizeof(Up));	//ʹ��memmove����
	}
	return result + n;
}

//move�Ľӿ�
template <class InputIter, class OutputIter>
OutputIter move(InputIter first, InputIter last, OutputIter result)
{
	return unchecked_move(first, last, result);
}

/*****************************************************************************************/
// move_backward
// �� [first, last)�����ڵ�Ԫ���ƶ��� [result - (last - first), result)��
/*****************************************************************************************/
// bidirectional_iterator_tag �汾
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result, mystl::bidirectional_iterator_tag) {
	//�Ե������Ƿ������Ϊ�жϣ��ٶ���
	while (first != last) {
		*--result = mystl::move(*--last);	//�����ƶ�
	}
	return result;
}

// random_access_iterator_tag �汾
template <class RandomIter1, class RandomIter2>
RandomIter2
unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
	RandomIter2 result, mystl::random_access_iterator_tag) {
	//��n����ѭ����ִ�д������ٶȼ���
	for (auto n = last - first; n > 0; --n)
		*--result = mystl::move(*--last);
	return result;
}

//unchecked_move_backward�ӿ�
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
	BidirectionalIter2 result)
{
	return unchecked_move_backward_cat(first, last, result,
		iterator_category(first));	//�жϵ���������
}

// Ϊ trivially_copy_assignable �����ṩ�ػ��汾
template <class Tp, class Up>
typename std::enable_if<
	std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
	std::is_trivially_move_assignable<Up>::value,
	Up*>::type
	unchecked_move_backward(Tp* first, Tp* last, Up* result) {
	const size_t n = static_cast<size_t>(last - first);
	if (n != 0) {
		result -= n;	//����������-=
		std::memmove(result, first, n * sizeof(Up));	//ʹ��memmove����
	}
	return result;
}

//move_backward�Ľӿ�
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
{
	return unchecked_move_backward(first, last, result);
}




}	//�����ռ������


#endif // !MYSTL_ALGOBASE_H_

