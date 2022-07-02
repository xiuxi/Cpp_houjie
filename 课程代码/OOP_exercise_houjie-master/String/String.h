#ifndef __MY_STRING__
#define __MY_STRING__

#include <iostream>
#include <cstring>

class String
{
public:
    String(const char* data = 0);
    String(const String& rhs);
    String& operator=(const String& rhs);
    ~String();
    char* get_c_data() const { return m_data;}

private:
    char* m_data;
};
inline 
String::String(const char* data)
{
    if (data) {
        m_data = new char[strlen(data)+1];
        strcpy(m_data, data);
    }
    else {
        m_data = new char[1];
        m_data[0] = '\0';
    }
}
inline
String::String(const String& rhs) 
{
    m_data = new char[strlen(rhs.m_data)+1];//这里需要加1吗？也就是rhs的data里面是否含有换行符
    //需要加1，但是rhs的data里面确实已经有换行符了，但是strlen返回的只有一般的值
}

inline 
String::~String()
{
    delete[] m_data;
}

inline
String& String::operator=(const String& rhs)
{
    if (this == &rhs) {
        return *this;
    }
    delete[] m_data;
    m_data = new char[strlen(rhs.m_data)+1];
    strcpy(m_data, rhs.m_data);
}


#endif

