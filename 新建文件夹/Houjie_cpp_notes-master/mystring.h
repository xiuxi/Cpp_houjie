#ifndef __MYSTRING__
#define __MYSTRING__

class String  //带指针成员的类必须有拷贝构造和拷贝赋值。
{
public:
    String(const char* cstr = 0);  //构造函数ctor

    String(const String& str);  //拷贝构造copy ctor
    String& operator = (const String& str);  //操作符重载，拷贝赋值copy op=
    ~String();  //析构函数dtor
    //以上三个是Big Three，三个特殊函数
    char* get_c_str() const { return m_data; }  //一般的成员函数，传回指针
private:
    char* m_data;
};

#include <cstring>

inline
String::String(const char* cstr = 0)  //构造函数
{
    if(cstr){
        m_data = new char[strlen(cstr)+1];  //new一个足够空间
        strcpy(m_data, cstr);  //拷贝到m_data
    }
    else{  //未指定初值
        m_data = new char[1];
        *m_data = '\0';
    }
}

inline
String::String(const String& str)  //拷贝构造函数-收到的参数是它自己的类型
{
    m_data = new char[ strlen(str.m_data) + 1];
    strcpy(m_data, str.m_data);  //深拷贝，把内容拷贝过去（如果是只把指针拷贝过去是浅拷贝）
}

inline
String& String::operator=(const String& str)  //拷贝赋值函数，这里&是reference
{
    if(this == &str)  //检测是否是自我赋值，这里&是取地址
        return *this;  //如果是则直接return,一能提高效率，二能避免将原有内容删除再赋值导致的不确定形为

    delete[] m_data;  //先删除原有的
    m_data = new char[ strlen(str.m_data) + 1];  //再创建新的空间
    strcpy(m_data, str.m_data);  //赋值新的内容
    return *this;
}

inline
String::~String()  //析构函数
{
    delete[] m_data;  //释放动态分配的内存
}

#include <iostream>
using namespace std;

ostream& operator << (ostream& os, const String& str)
{
    os << str.get_c_str();
    return os;
}

#endif

/*
    关于堆，栈：
    Stack是存在于某作用域的一块内存空间。例如当你调用函数时函数本身就会形成一个Stack用来放置
    其所接收的参数以及返回地址。在函数本体内声明的任何变量，其所使用的内存块都取自上述Stack.
    Heap是指由操作系统提供的一块global内存空间，程序可以动态分配从中获得若干区块(blocks).
    eg:
    class Comlex{ ... };
    ...
    Complex c4(1,2);
    int main(){
        Complex c1(1,2);
        Complex* p = new Complex(3);
        static Complex c3(1,3);
        deleta p;
    }
    这里c1所占用的空间来自stack，而Complex(3)是个临时对象，其所占用的空间是以new方法从heap中
    动态分配获得，并由p指向.
    c1是stack object，其生命在作用域结束之际结束，这种作用域内的object又称为auto object，因为
    它会被自动清理。c3是static object，其生命在作用域结束后仍然存在，直至整个程序结束。
    c4是global object,其生命在整个程序结束后才结束，也可以视为一种static object，作用域是整个程序。
    p所指的是heap object,其生命在delete时结束。如果不写delete,会出现内存泄漏(memory leak),因为当作用
    域结束，p所指的heap object仍然存在，但指针p的生命却结束了，作用域之外再也看不到p也就无法delete p.

    关于new:先分配memory再调用ctor。
    Complex* p = new Complex(1,3);编译时被编译器转化为：
        Complex *p;
        void* mem = operator new( sizeof(Complex) );  //分配内存,内部调用malloc(n)
        p = static_cast<Complex*>(mem);  //转型
        p->Complex::Complex(1,3);  //构造函数 == Complex::Complex(p,1,3)

    关于delete:先调用dtor再释放memory.
    delete p;编译时被编译器转化为：
        String::~String(p);  //析构函数
        operator delete(p);  //释放内存，内部调用free(p)

    array new一定要搭配array delete
    delete[]根据分配数组的数删，如String* p = new String[3];delete[] p;唤起3次dtor.而delete p只唤起1次
    dtor,还有剩余的内存单元里的内容没删除，是不正确的
*/