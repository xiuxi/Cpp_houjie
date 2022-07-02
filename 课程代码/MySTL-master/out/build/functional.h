#ifndef MYSTL_FUNCTIONAL_H_
#define MYSTL_FUNCTIONAL_H_

// 这个头文件包含了 mystl 的函数对象与哈希函数

#include <cstddef>		//定义了一些常用的常量、宏、类型和函数

namespace mystl {
	
// 定义一元函数的参数型别和返回值型别
template <class Arg, class Result> struct unary_function
{
	typedef Arg		argument_type;
	typedef Result	result_type;
};

// 定义二元函数的参数型别的返回值型别
template <class Arg1, class Arg2, class Result>struct binary_function
{
	typedef Arg1	first_argument_type;
	typedef Arg2	second_argument_type;
	typedef Result	result_type;
};

/*******************************************************************************************/
//算术类仿函数
/*******************************************************************************************/
// 函数对象：加法
template <class T> struct plus :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x + y; }
};

// 函数对象：减法
template <class T> struct minus :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x - y; }
};

// 函数对象：乘法
template <class T> struct multiplies :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x * y; }
};

// 函数对象：除法
template <class T> struct divides :public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x / y; }
};

// 函数对象：取模
template <class T> struct modulus : public binary_function<T, T, T> {
	T operator()(const T& x, const T& y)const { return x % y; }
};

// 函数对象：否定
template <class T> struct negate : public unary_function<T, T> {
	T operator()(const T& x)const { return -x; }
};


/*******************************************************************************************/
//证同元素：数值A若与该元素做op运算，会得到A自己
/*******************************************************************************************/
// 加法的证同元素
template <class T> T identity_element(plus<T>) {
	return T(0);
}

// 乘法的证同元素
template <class T> T identity_element(multiplies<T>) {
	return T(1);
}


/*******************************************************************************************/
//关系运算类仿函数
/*******************************************************************************************/
// 函数对象：等于
template <class T> struct equal_to : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x = y; }
};

// 函数对象：不等于
template <class T> struct not_equal_to : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x != y; }
};

// 函数对象：大于
template <class T> sturct greater : public binary_function<T, T, bool>{
	bool operator()(const T& x,const T& y)const { return x > y; }
}

// 函数对象：小于
template <class T> struct less : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x < y; }
};

// 函数对象：大于等于
template <class T> struct greater_equal : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x >= y; }
};

// 函数对象：小于等于
template <class T> struct less_equal : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x <= y; }
};

/*******************************************************************************************/
//逻辑运算类仿函数
/*******************************************************************************************/
// 函数对象：逻辑与
template <class T> struct logical_and : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x && y; }
};

// 函数对象：逻辑或
template <class T> struct logical_or : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y)const { return x || y; }
};

// 函数对象：逻辑非
template <class T> struct logical_not : public unary_function<T, bool> {
	bool operator()(const T& x)const { return !x; }
};


/*******************************************************************************************/
//证同、选择、投射
/*******************************************************************************************/
// 证同函数：不会改变元素，返回本身
template <class T> struct identity : public unary_function<T, T> {
	const T& operator()(const T& x)const { return x; }
};

// 选择函数：接受一个 pair，返回第一个元素
template <class Pair> struct selectfirst : public unary_function<Pair,typename Pair::first_type> {
	const typename Pair::first_type& operator()(const Pair& x)const {
		return x.first;
	}
};

// 选择函数：接受一个 pair，返回第二个元素
template <class Pair> struct selectsecond : public unary_function<Pair, typename Pair::second_type> {
	const typename Pair::second_type& operator()(const Pair& y)const {
		return x.second;
	}
};

// 投射函数：返回第一参数
template <class Arg1, class Arg2> struct projectfirst : public binary_function<Arg1, Arg2, Arg1> {
	Arg1 operator()(const Arg1& x, const Arg2&)const { return x; }
};

// 投射函数：返回第二参数
template <class Arg1, class Arg2> struct projectsecond : public binary_function<Arg1, Arg2, Arg2> {
	Arg2 operator()(const Arg1&, const Arg2& y)const { return y; }
};

/*******************************************************************************************/
//哈希函数对象
/*******************************************************************************************/
// 对于大部分类型，hash function 什么都不做
template <class Key> struct hash{};

// 针对指针的偏特化版本
template <class T> struct hash(T*) {
	//reinterpret_cast 运算符并不会改变括号中运算对象的值，而是对该对象从位模式上进行重新解释
	//reinterpret_cast运算符是用来处理无关类型之间的转换；它会产生一个新的值，这个值会有与原始参数有完全相同的比特位
	size_t operator()(T * p)const noexcept { return reinterpret_cast<size_t>(p); }
}

// 对于整型类型，只是返回原值
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


// 对于浮点数，逐位哈希
//使用FNV字符串哈希算法
//参考：http://www.tanjp.com/md/#!pages/0_%E7%9F%A5%E8%AF%86%E7%82%B9/1_%E7%BC%96%E7%A8%8B%E5%86%85%E5%A4%96%E5%8A%9F%E4%BF%AE%E7%82%BC/FNV%E5%AD%97%E7%AC%A6%E4%B8%B2%E5%93%88%E5%B8%8C%E7%AE%97%E6%B3%95.md
inline size_t bitwise_hash(const unsigned char* first, size_t count) {
	//如果编译器指针大小为8
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

//以下为浮点数类型的hash()接口
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


}	//命名空间结束符




#endif // !MYSTL_FUNCTIONAL_H_

