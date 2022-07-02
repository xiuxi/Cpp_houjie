#ifndef __MYSTRING__
#define __MYSTRING__
#include<cstring>

class String
{
public:
	// 构造函数 此时已经声明了默认参数值，后面不能再次进行重复声明
	String(const char* cstr = 0);
	// 拷贝构造函数
	String(const String &str);
	// 拷贝赋值
	String& operator = (const String& str);
	// 析构函数
	~String();

	// 不改变变量
	char* get_c_str() const { return m_data; }
private:
	char* m_data;
};

// 由于在类内构造函数中已经声明了参数的默认值，此时无需再次进行重复声明，否则会报错
inline
String::String(const char* cstr)
{
	if(cstr) {
		m_data = new char[strlen(cstr) + 1];
		strcpy(m_data, cstr);
	}
	else {
		m_data = new char[1];
		*m_data = '\0';
	}
}

inline
String::String(const String& str)
{
	m_data = new char[strlen(str.m_data) + 1];
	strcpy(m_data, str.m_data);
}

inline String&
String::operator = (const String& str)
{
	// 检测自我赋值 比较指针的位置，注意没有实现String 的 operator==
	if(this == &str) {
		return *this;
	}
	delete[] m_data;
	m_data = new char[strlen(str.m_data) + 1];
	strcpy(m_data, str.m_data);
	return *this;
}

inline
String::~String()
{
	delete[] m_data;
}

#include <iostream>

std::ostream&
operator << (std::ostream& os, const String& str)
{
	os << str.get_c_str();
	return os;
}

#endif