#ifndef MYSTL_FUNCTIONAL_H_
#define MYSTL_FUNCTIONAL_H_

// ���ͷ�ļ������� mystl �ĺ����������ϣ����

#include <cstddef>		//������һЩ���õĳ������ꡢ���ͺͺ���

namespace mystl {
	
// ����һԪ�����Ĳ����ͱ�ͷ���ֵ�ͱ�
template <class Arg, class Result> struct unary_function
{
	typedef Arg		argument_type;
	typedef Result	result_type;
};

// �����Ԫ�����Ĳ����ͱ�ķ���ֵ�ͱ�
template <class Arg1, class Arg2, class Result>struct binary_function
{
	typedef Arg1	first_argument_type;
	typedef Arg2	second_argument_type;
	typedef Result	result_type;
};

/*******************************************************************************************/
//������º���
/*******************************************************************************************/
// �������󣺼ӷ�
template <class T> struct plus :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x + y; }
};

// �������󣺼���
template <class T> struct minus :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x - y; }
};

// �������󣺳˷�
template <class T> struct multiplies :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x * y; }
};

// �������󣺳���
template <class T> struct divides :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x / y; }
};

// ��������ȡģ
template <class T> struct modulus : public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x % y; }
};

// �������󣺷�
template <class T> struct negate : public unary_function<T, T> {
	T operator()(const T& x)const { return -x; }
};


/*******************************************************************************************/
//֤ͬԪ�أ���ֵA�����Ԫ����op���㣬��õ�A�Լ�
/*******************************************************************************************/
// �ӷ���֤ͬԪ��
template <class T> T identity_element(plus<T>) {
	return T(0);
}

// �˷���֤ͬԪ��
template <class T> T identity_element(multiplies<T>) {
	return T(1);
}


/*******************************************************************************************/
//��ϵ������º���
/*******************************************************************************************/
// �������󣺵���
template <class T> struct equal_to : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x = y; }
};

// �������󣺲�����
template <class T> struct not_equal_to : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x != y; }
};

// �������󣺴���
template <class T> sturct greater : public binary_function<T, T, bool>{
	bool operator()(const T& x,const T& y)const { return x > y; }
}

// ��������С��
template <class T> struct less : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x < y; }
};

// �������󣺴��ڵ���
template <class T> struct greater_equal : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x >= y; }
};

// ��������С�ڵ���
template <class T> struct less_equal : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x <= y; }
};

/*******************************************************************************************/
//�߼�������º���
/*******************************************************************************************/
// ���������߼���
template <class T> struct logical_and : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x && y; }
};

// ���������߼���
template <class T> struct logical_or : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x || y; }
};

// ���������߼���
template <class T> struct logical_not : public unary_function<T, bool> {
	bool operator()(const T& x)const { return !x; }
};


/*******************************************************************************************/
//֤ͬ��ѡ��Ͷ��
/*******************************************************************************************/
// ֤ͬ����������ı�Ԫ�أ����ر���
template <class T> struct identity : public unary_function<T, T> {
	const T& operator()(const T& x)const { return x; }
};

// ѡ����������һ�� pair�����ص�һ��Ԫ��
template <class Pair> struct selectfirst : public unary_function<Pair,typename Pair::first_type> {
	const typename Pair::first_type& operator()(const Pair& x)const {
		return x.first;
	}
};

// ѡ����������һ�� pair�����صڶ���Ԫ��
template <class Pair> struct selectsecond : public unary_function<Pair, typename Pair::second_type> {
	const typename Pair::second_type& operator()(const Pair& y)const {
		return x.second;
	}
};

// Ͷ�亯�������ص�һ����
template <class Arg1, class Arg2> struct projectfirst : public binary_function<Arg1, Arg2, Arg1> {
	Arg1 operator()(const Arg1& x, const Arg2&)const { return x; }
};

// Ͷ�亯�������صڶ�����
template <class Arg1, class Arg2> struct projectsecond : public binary_function<Arg1, Arg2, Arg2> {
	Arg2 operator()(const Arg1&, const Arg2& y)const { return y; }
};

/*******************************************************************************************/
//��ϣ��������
/*******************************************************************************************/
// ���ڴ󲿷����ͣ�hash function ʲô������
template <class Key> struct hash{};

// ���ָ���ƫ�ػ��汾
template <class T> struct hash(T*) {
	//reinterpret_cast �����������ı���������������ֵ�����ǶԸö����λģʽ�Ͻ������½���
	//reinterpret_cast����������������޹�����֮���ת�����������һ���µ�ֵ�����ֵ������ԭʼ��������ȫ��ͬ�ı���λ
	size_t operator()(T * p)const noexcept { return reinterpret_cast<size_t>(p); }
}

// �����������ͣ�ֻ�Ƿ���ԭֵ
#define MYSTL_TRIVIAL_HASH_FCN(Type)							\
template <> struct hash<Type> {									\						
	size_t operator()(Type val)const noexcept					\
	{ return static_cast<size_t>(val); }						\
};

MYSTL_TRIVIAL_HASH_FCN(bool)

MYSTL_TRIVIAL_HASH_FCN(char)

MYSTL_TRIVIAL_HASH_FCN(signed char)

MYSTL_TRIVIAL_HASH_FCN(unsigned char)

MYSTL_TRIVIAL_HASH_FCN(wchar_t)

MYSTL_TRIVIAL_HASH_FCN(char16_t)

MYSTL_TRIVIAL_HASH_FCN(char32_t)

MYSTL_TRIVIAL_HASH_FCN(short)

MYSTL_TRIVIAL_HASH_FCN(unsigned short)

MYSTL_TRIVIAL_HASH_FCN(int)

MYSTL_TRIVIAL_HASH_FCN(unsigned int)

MYSTL_TRIVIAL_HASH_FCN(long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long)

MYSTL_TRIVIAL_HASH_FCN(long long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long long)

#undef MYSTL_TRIVIAL_HASH_FCN


// ���ڸ���������λ��ϣ
//ʹ��FNV�ַ�����ϣ�㷨
//�ο���http://www.tanjp.com/md/#!pages/0_%E7%9F%A5%E8%AF%86%E7%82%B9/1_%E7%BC%96%E7%A8%8B%E5%86%85%E5%A4%96%E5%8A%9F%E4%BF%AE%E7%82%BC/FNV%E5%AD%97%E7%AC%A6%E4%B8%B2%E5%93%88%E5%B8%8C%E7%AE%97%E6%B3%95.md
inline size_t bitwise_hash(const unsigned char* first, size_t count) {
	//���������ָ���СΪ8
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)		
	const size_t fnv_offset = 14695981039346656037ull;	
	const size_t fnv_prime = 1099511628211ull;
#else
	const size_t fnv_offset = 2166136261u;
	const size_t fnv_prime = 16777619u;
#endif
	size_t result = fnv_offset;
	for (size_t i = 0; i < count; ++i) {
		result ^= (size_t)first[i];
		result *= fnv_prime;
	}
	return result;
}

//����Ϊ���������͵�hash()�ӿ�
template <> struct hash<float> {
	size_t operator()(const float& val) {
		return val == 0.0f ? 0 : bitwise_hash((const unsigned char*) & val, sizeof(float));
	}
};

template <>
struct hash<double>
{
	size_t operator()(const double& val)
	{
		return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
	}
};

template <>
struct hash<long double>
{
	size_t operator()(const long double& val)
	{
		return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
	}
};


}	//�����ռ������




#endif // !MYSTL_FUNCTIONAL_H_

