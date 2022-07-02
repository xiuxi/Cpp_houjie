#ifndef MYSTL_TYPE_TRAITS_H_
#define MYSTL_TYPE_TRAITS_H_

// ���ͷ�ļ�������ȡ������Ϣ
// _type_traits����������ȡ�ͱ������

#include<type_traits.h>		//��ȡ��

namespace mystl {	//�����ռ�mystl

//Helper class�� ������������ʱ�����ı�׼��
//��C++11��std�������ռ��У�std::integral_constant��װָ�����͵ľ�̬���������� C++ ���������Ļ��ࡣ
//��ģ��ּ���ṩ����ʱ������Ϊ���͡�
//������׼��ļ������������������͵Ļ��࣬�ر��������ǵ�bool�����У��μ�true_type��false_type
//���鿴���ӣ�https://www.cplusplus.com/reference/type_traits/integral_constant/
//			  https://en.cppreference.com/w/cpp/types/integral_constant
//			  https://blog.csdn.net/timecur/article/details/97640167
template<class T, T v>struct m_integral_constant {			//helper class
	static constexpr T value = v;		//constexpr��ʾ�������ʽ���ṩת�����ܣ����ذ�װ��ֵ
};

//std::bool_constantΪ�������������һ����������ģ�壬����T��boolֵ
//������һ�����װ�ĸ�������ģ��ʵ�֣��� m_bool_constant
template<bool b> using m_bool_constant = m_integral_constant<bool b>;

//�������������typedef������T��boolֵ��
//true_type	std :: integral_constant < bool , true >��false_type	std::integral_constant<bool, false>
//����ͬ����һ�����Ƶ�typedefʵ��
typedef m_bool_constant<true> m_true_type;
typedef m_bool_constant<false> m_false_type;

// type traits
//�����ͱ���ȡ���ڿ�Դ��Ŀ�����ж��Ƿ���pair�ͱ𣬶������ж��Ƿ���POD�ͱ�
// ���������ڣ���C++ STL�����map���������ֵ�ԣ���ô����value_type���Ǹ�pair��
// ��˲����ǲ���pair���ж�
// is_pair

// --- forward declaration begin	ǰ��������ʼ
template <class T1, class T2>struct pair;			//ģ��ṹ��
// --- forward declaration end		ǰ����������

//Ϊ�ж��Ƿ���pair�ͱ𣬽���ģ��ƫ�ػ����ֱ�̳���false_type��true_type�����̳�֮���ӵ����value����
//�ο����ӣ�https://cloud.tencent.com/developer/article/1582452
template <class T>struct is_pair : mystl::m_false_type {};	

template <class T1, class T2>struct is_pair<mystl::pair<T1, T2>> : mystl::m_true_type {};


}		//�����ռ������


#endif // !MYSTL_TYPE_TRAITS_H_

