#ifndef MYSTL_UNINITALIZED_H_
#define MYSTL_UNINITALIZED_H_

// ���ͷ�ļ����ڶ�δ��ʼ���ռ乹��Ԫ��

#include "algobase.h"			//�����㷨
#include "construct.h"			//�������
#include "iterator.h"			//������
#include "type_traits.h"		//��ȡ��
#include "util.h"				//ͨ�ù���

namespace mystl {	//�����ռ�mystl

/*****************************************************************************************/
// uninitialized_copy
// �� [first, last) �ϵ����ݸ��Ƶ��� result Ϊ��ʼ���Ŀռ䣬���ظ��ƽ�����λ��
//��Ӧ�ڸ߲�ε�copy����
/*****************************************************************************************/

template <class InputIter, class ForwardIter>ForwardIter unchecked_uninit_copy
(InputIter first, InputIter last, ForwardIter result, std::true_type) {		
	return mystl::copy(first, last, result);	//true_type��false_type�ɸ������;�����ͬģ���ػ�
	//�ο���https://stackoverflow.com/questions/58694521/what-is-stdfalse-type-or-stdtrue-type
}

template <class InputIter, class ForwardIter>ForwardIter unchecked_uninit_copy
(InputIter first, InputIter last, ForwardIter result, std::false_type) {
	auto cur = result;
	try {
		for (; first != last; ++first, ++cur) {
			mystl::construct(&*cur, *first);	//����Ԫ�أ�&*curָ����cur��ָ�Ķ�������ã�by reference
		}
	}
	catch (...) {
		for (; result != cur; --cur)
			mystl::destroy(&*cur);				//����ʧ�ܾ�����Ԫ��
	}
	return cur;
}

//uninitialized_copy�Ľӿ�
template <class InputIter, class ForwardIter>ForwardIter uninitialized_copy
(InputIter first, InputIter last, ForwardIter result) {
	return mystl::unchecked_uninit_copy(first, last, result,
		std::is_trivially_copy_assignable<
		typename iterator_traits<ForwardIter>::
		value_type>{});
}

/*****************************************************************************************/
// uninitialized_copy_n
// �� [first, first + n) �ϵ����ݸ��Ƶ��� result Ϊ��ʼ���Ŀռ䣬���ظ��ƽ�����λ��
/*****************************************************************************************/
template <class InputIter, class size, class ForwardIter>ForwardIter unchecked_uninit_copy_n
(InputIter first, Size n, ForwardIter result, std::true_type) {
	return mystl::copy(first, n, result).second;	//.second��ʾ����ڶ�����
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

// uninitialized_copy_n�Ľӿ�
template <class InputIter, class Size, class ForwardIter>ForwardIter uninitialized_copy_n
(InputIter first, Size n, ForwardIter result) {
	return mystl::unchecked_uninit_copy_n(first, n, result,
		std::is_trivially_copy_assignable<
		typename iterator_traits<InputIter>::
		value_type>{});
}

/*****************************************************************************************/
// uninitialized_fill
// �� [first, last) ���������Ԫ��ֵ
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

//uninitialized_fill�Ľӿ�
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
// �� first λ�ÿ�ʼ����� n ��Ԫ��ֵ��������������λ��
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

//uninitialized_fill_n�Ľӿ�
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
// ��[first, last)�ϵ������ƶ����� result Ϊ��ʼ���Ŀռ䣬�����ƶ�������λ��
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

//uninitialized_move�Ľӿ�
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
// ��[first, first + n)�ϵ������ƶ����� result Ϊ��ʼ���Ŀռ䣬�����ƶ�������λ��
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

//uninitialized_move_n�Ľӿ�
template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
{
	return mystl::unchecked_uninit_move_n(first, n, result,
		std::is_trivially_move_assignable<
		typename iterator_traits<InputIter>::
		value_type>{});
}

}		//���������ռ�mystl

#endif // !MYSTL_UNINITALIZED_H_

