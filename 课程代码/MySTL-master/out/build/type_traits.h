#ifndef MYSTL_TYPE_TRAITS_H_
#define MYSTL_TYPE_TRAITS_H_

// 这个头文件用于提取类型信息
// _type_traits技术负责萃取型别的特性

#include<type_traits.h>		//萃取机

namespace mystl {	//命名空间mystl

//Helper class： 帮助创建编译时常量的标准类
//在C++11的std的命名空间中，std::integral_constant包装指定类型的静态常量。它是 C++ 类型特征的基类。
//此模板旨在提供编译时常量作为类型。
//它被标准库的几个部分用作特征类型的基类，特别是在它们的bool变体中：参见true_type和false_type
//详情看链接：https://www.cplusplus.com/reference/type_traits/integral_constant/
//			  https://en.cppreference.com/w/cpp/types/integral_constant
//			  https://blog.csdn.net/timecur/article/details/97640167
template<class T, T v>struct m_integral_constant {			//helper class
	static constexpr T value = v;		//constexpr表示常量表达式，提供转换功能，返回包装的值
};

//std::bool_constant为常见情况定义了一个辅助别名模板，其中T是bool值
//这里做一个简易版的辅助别名模板实现，即 m_bool_constant
template<bool b> using m_bool_constant = m_integral_constant<bool b>;

//常见情况的两个typedef，其中T是bool值。
//true_type	std :: integral_constant < bool , true >与false_type	std::integral_constant<bool, false>
//这里同样做一个类似的typedef实现
typedef m_bool_constant<true> m_true_type;
typedef m_bool_constant<false> m_false_type;

// type traits
//这里型别萃取，在开源项目中是判断是否是pair型别，而不是判断是否是POD型别
// 其用意在于：在C++ STL中针对map这种如果键值对，那么它的value_type就是个pair，
// 因此采用是不是pair来判断
// is_pair

// --- forward declaration begin	前置声明开始
template <class T1, class T2>struct pair;			//模板结构体
// --- forward declaration end		前置声明结束

//为判断是否是pair型别，进行模板偏特化，分别继承了false_type、true_type，而继承之后就拥有了value属性
//参考链接：https://cloud.tencent.com/developer/article/1582452
template <class T>struct is_pair : mystl::m_false_type {};	

template <class T1, class T2>struct is_pair<mystl::pair<T1, T2>> : mystl::m_true_type {};


}		//命名空间结束符


#endif // !MYSTL_TYPE_TRAITS_H_

