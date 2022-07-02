#ifndef MYSTL_NUMERIC_H_
#define MYSTL_NUMERIC_H_

// ���ͷ�ļ������� mystl ����ֵ�㷨

#include "iterator.h"		//������

namespace mystl {

/*****************************************************************************************/
// accumulate����������init��[first,last)������Ԫ�ص��ܺ�
// �汾1���Գ�ֵ init ��ÿ��Ԫ�ؽ����ۼ�
// �汾2���Գ�ֵ init ��ÿ��Ԫ�ؽ����Զ���Ķ�Ԫ����
//����accumulate��Ҫ�ṩ��ֵinit���Ա�֤��[first,last)Ϊ��ʱ���ܻ��һ����ȷ�����ֵ
/*****************************************************************************************/
// �汾1
template <class InputIter, class T> T accumulate(InputIter first, InputIter last, T init) {
	for (; first != last; ++first) {
		init += *first;		//��ÿ��Ԫ��ֵ�ۼӵ���ֵinit����
	}
	return init;
}

//�汾2
template <class InputIter, class T, class BinaryOp> T accumulate
(InputIter first, InputIter last, T init, BinaryOp binary_op) {
	for (; first != last; ++first) {
		init = binary_op(init, *first);	//��ÿ��Ԫ��ִ�ж�Ԫ����
	}
	return init;
}

/*****************************************************************************************/
// adjacent_difference����������[first,last)������Ԫ�صĲ��͵����㷽ʽ
// �汾1������operator- ��������Ԫ�صĲ�ֵ��������浽�� result Ϊ��ʼ��������
// �汾2���Զ�������Ԫ�صĶ�Ԫ����
/*****************************************************************************************/
// �汾1
template <class InputIter, class OutputIter> OutputIter adjacent_difference
(InputIter first, InputIter last, OutputIter result) {
	if (first == last) return result;
	*result = *first;		// ��¼��һ��Ԫ��
	auto value = *first;	
	while (++first != last) {	//�߹���������
		auto tmp = *first;	
		*++result = tmp - value;	//������Ԫ�صĲ��(��-ǰ)����ֵ��Ŀ�Ķ�
		value = tmp;	//valueÿ�ν�һ
	}
	return ++result;	//����ÿһ������Ԫ�صĲ��
}

//�汾2
template <class InputIter, class OutputIter, class BinaryOp>OutputIter adjacent_difference
(InputIter first, InputIter last, OutputIter result, BinaryOp binary_op) {
	if (first == last) return result;
	*result = *first;		// ��¼��һ��Ԫ��
	auto value = *first;
	while (++first != last) {	//�߹���������
		auto tmp = *first;
		*++result = binary_op(tmp, value);	//������Ԫ�ض�Ԫ����Ľ������ֵ��Ŀ�Ķ�
		value = tmp;		//valueÿ�ν�һ
	}	
	return ++result;		//����ÿһ������Ԫ�صĲ��
}

/*****************************************************************************************/
// inner_product������[first,last)��[first2,first2+(last1-first1))���ڻ�
// �汾1���� init Ϊ��ֵ����������������ڻ�   
// �汾2���Զ��� operator+ �� operator*
//���ú���ʱ����Ҫ�ṩ��ֵinit
/*****************************************************************************************/
// �汾1
template <class InputIter1, class InputIter2, class T> T inner_product
(InputIter1 first1, InputIter1 last2, InputIter2 first2, T init) {
	for (; first1 != last1; ++first1,++first2) {	//�Ե�һ����Ԫ�ظ���Ϊ�ݣ����������ж���һ��
		init = init + (*first1 * first2);	//ִ���������е�һ���ڻ�
	}
	return init;
}

//�汾2
template <class InputIter1, class InputIter2, class T, class BinaryOp1, class BinaryOp2> T inner_product
(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init, BinaryOp1 binary_op1, BinaryOp2 binary_op2) {
	for (; first1 != last1; ++first1, ++first2) {	//�Ե�һ����Ԫ�ظ���Ϊ�ݣ����������ж���һ��
		//������ṩ�ķº�����ȡ����һ�汾�е�operator*��operator+
		init = binary_op1(init, binary_op2(*first1, *first2));	
	}
	return init;
}

/*****************************************************************************************/
// partial_sum����������ֲ��ܺͣ��͵�������ʱ��㷨
// �汾1������ֲ��ۼ���ͣ�������浽�� result Ϊ��ʼ��������
// �汾2�����оֲ������Զ����Ԫ����
/*****************************************************************************************/
//�汾1
template <class InputIter, class OutputIter> OutputIter partial_sum
(InputIter first, InputIter last, OutputIter result) {
	if (first == last) return result;
	*result = *first;		// ��¼��һ��Ԫ��
	auto value = *first;
	while (++first != last) {	
		value = value + *first;		//ǰn��Ԫ�ص��ܺ�
		*++result = *value;			//ָ����Ŀ�Ķ�
	}
	return ++result;
}

//�汾2
template <class InputIter, class OutputIter, class BinaryOp>OutputIter partial_sum
(InputIter first, InputIter last, OutputIter result, BinaryOp binary_op) {
	if (first == last) return result;
	*result = *first;	//��¼��һ��Ԫ��
	auto value = *first;
	while (++first != last) {
		value = binary_op(value, *first);	//ǰn��Ԫ�ص��ܼ�
		*++result = *value;					//ָ����Ŀ�Ķ�
	}
	return ++result;
}

///*****************************************************************************************/
////power����������ĳ����n�ݴη������Լ����Լ�����ĳ�����㣬��n��
////�������������ָ�������ָ��Ϊ�˷���������
////�汾1������
////�汾2���ݴη������ָ��Ϊ�˷����㣬��n>0ʱ����x^n
///*****************************************************************************************/
////�汾1
//template <class T, class Integer>inline T power(T x, Integer n) {
//	return power(x, n, multiplies<T>());
//}
//
////�汾2��ע��"MonoidOp"�����������ɣ����������㽻����
//template <class T, class Integer, class MonoidOp> T powder(T x, Integer n, MonoidOp Monoid_op) {
//	if(n==0) return iden
//}

/*****************************************************************************************/
// iota�������趨ĳ����������ݣ���һ���ʱ��㷨
// ���[first, last)���� value Ϊ��ֵ��ʼ����
//����[first,last)�����value,value+1,value+2...
/*****************************************************************************************/
template <class ForwardIter, class T> void iota(ForwardIter first, ForwardIter last, T value) {	
	while (++first!=last)
	{
		*first = value;
		++value;	//����value
	}
}

}	//�����ռ������

#endif // !MYSTL_NUMERIC_H_

