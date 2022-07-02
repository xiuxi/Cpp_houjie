#ifndef MYSTL_SET_ALGO_H_
#define MYSTL_SET_ALGO_H_
// ���ͷ�ļ����� set �������㷨: ����union, ����intersection, �difference, �ԳƲsymmetric_difference
// ���к�����Ҫ����������

#include "algobase.h"       //�����㷨
#include "iterator.h"       //������

namespace mystl {

//*****************************************************************************************/
// set_union������S1��S2�Ĳ����������ڰ���S1��S2�ڵ�ÿһ��Ԫ�أ��ȶ�����
// ���� S1��S2 �Ľ�������浽 result �У�����һ��������ָ����������β��
/*****************************************************************************************/
//�汾1
template<class InputIter1, class InputIter2, class OutputIter>OutputIter set_union
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
    //���������䶼��δ����β��ʱ��ִ�����²���
	while (first1 != last1 && first2 != last2) {
        //�������ڷֱ��ƶ������������Ƚ�Ԫ�ؽ�С��(����ΪA��)��¼��Ŀ����
        //Ȼ���ƶ�A��������ʹ֮ǰ����ͬʱ����һ��������������Ȼ�������һ�εıȴ�С����¼Сֵ���������ƶ�
        //ֱ����������һ������β�ˡ�
        //���Ԫ����ȣ�ȡS1�߼�¼��Ŀ��������ͬʱ�ƶ�����������
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
    //ֻҪ����֮����һ������β�ˣ��ͽ�������whileѭ��
    // ���½���δ����β�˵����������ʣ��Ԫ�ؿ�����Ŀ�Ķ�
    //�˿̵�[first1,last1)��[first2,last2)֮����һ���ǿհ�����
	return mystl::copy(first2, last2, mystl::copy(first1, last1, result));
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
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
    // ��ʣ��Ԫ�ؿ����� result
    return mystl::copy(first2, last2, mystl::copy(first1, last1, result));
}

/*****************************************************************************************/
// set_intersection������S1��S2֮�������˼����ں�ͬʱ������S1��S2�ڵ�ÿһ��Ԫ��
// ���� S1��S2 �Ľ�������浽 result �У�����һ��������ָ����������β��
/*****************************************************************************************/
//�汾1
template <class InputIter1, class InputIter2, class OutputIter>OutputIter set_intersection
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
    //���������䶼��δ����β��ʱ��ִ�����²���...
    while (first1 != last1 && first2 != last2) {
        //���������ڷֱ��ƶ���������ֱ������Ԫ��ֵ��ͬ����ͣ������ֵ��¼��Ŀ����
        //�ټ����ƶ���������ֱ������֮����һ������β��
        if (*first1 < *first2) {
            ++first1;
        }
        else if (*first2 < *first1) {
            ++first2;
        }
        else {
            *result = *first1;  //����ͬԪ��ʱ������ȡS1��Ԫ��
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
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
// set_difference:����S1��S2֮����˼����ں�������S1����������S2��ÿһ��Ԫ��
// ���� S1-S2 �Ľ�������浽 result �У�����һ��������ָ����������β��
/*****************************************************************************************/
//�汾1
template <class InputIter1, class InputIter2, class OutputIter>OutputIter set_difference
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
    //���������䶼��δ����β��ʱ��ִ�����²���...
    while (first1 != last1 && first2 != last2) {
        //�������ڷֱ��ƶ���������
        //����һ�����Ԫ�ص��ڵڶ������Ԫ��(��ʾ��ֵͬʱ������������),����������ͬʱǰ��
        //����һ�����Ԫ�ش��ڵڶ������Ԫ�أ����õڶ�����ǰ��
        //���������ִ����ͱ�֤����һ�����Ԫ��С�ڵڶ������Ԫ��ʱ����һ�����Ԫ��ֻ�����ڵ�һ���䣬�����ڵڶ�����
        //���ǽ�����¼��Ŀ����
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
    return mystl::copy(first1, last1, result);  //�����������Ŀ����
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
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
// set_symmetric_difference������S1��S2֮�ԳƲ���˼����ں�"������S1������������S2"�Լ�"������S2����������S1"��Ԫ��
// ���� (S1-S2)��(S2-S1) �Ľ�������浽 result �У�����һ��������ָ����������β��
/*****************************************************************************************/
//�汾1
template <class InputIter1, class InputIter2, class OutputIter>OutputIter set_symmetric_difference
(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, OutputIter result) {
    //���������䶼��δ����β��ʱ��ִ�����²���...
    while (first1 != last1 && first2 != last2) {
        //���������ڷֱ��ƶ������������������ڵ�Ԫ����ȣ�����������ͬʱǰ��
        //���������ڵ�Ԫ�ز��ȣ��ͼ�¼��Сֵ��Ŀ�����������Сֵ��������ǰ��
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
    return mystl::copy(first2, last2, mystl::copy(first1, last1, result));  //�ݹ鿽��Ԫ�ص�Ŀ����
}

//�汾2��ʹ�ú������� comp ����Ƚϲ���
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

}	//�����ռ������

#endif // !MYSTL_SET_ALGO_H_

