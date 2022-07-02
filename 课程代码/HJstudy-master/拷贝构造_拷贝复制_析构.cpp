//不带指针类   ——complex
//带指#define针类   ——String    
//带指针的类的拷贝构造和拷贝赋值一定要自己写   避免浅拷贝
#include <iostream>
using namespace std;

#ifndef  __MYSTRING__
#define  __MYSTRING__
class String
{
    public:
        String(const char* cstr = 0);           //初始化
        String(const String& str);              //拷贝构造
        String& operator = (const String& str); //拷贝赋值 只要类带指针，一定要写拷贝赋值函数
        ~String();                              //析构
        char* get_c_str() const { return m_data;}

    private:
        char* m_data;
};

inline 
String::String(const char* cstr = 0)
{
    if(cstr){
        m_data = new char[strlen(cstr+1)];
        strcpy(m_data , cstr);
    }
    else
    {
        m_data = new char[1];
        *m_data = '\0';
    }
}

inline
String::String(const String& str)
{
    m_data = new char[strlen(str.m_data)+1];
    strcpy(m_data,str.m_data);
}

inline
String& String::operator=(const String& str)
{
    if (this == &str)  //自我赋值     根据执行步骤 删除-分配内存-拷贝
    {                               //如果没有检测自我赋值而用户却使用了自我赋值，数据就会丢失，程序报错
        return *this;
    }
    delete[] m_data;
    m_data = new char[strlen(str.m_data)];
    strcpy(m_data,str.m_data);
    return *this;
}

inline
String::~String()
{
    delete[] m_data;
}

#endif

ostream&
operator << (ostream& os,const String& str){
    os << str.get_c_str();
    return os;
};

int main()
{
    String s1();
    String s2("hello");   //构造函数

    String s3(s2);          //拷贝构造
    cout<<s3<<endl;
    s3 = s2;              //拷贝赋值
    cout<<s3<<endl;

    return 0;
}