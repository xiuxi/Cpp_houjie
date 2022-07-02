//复数-不带指针的类
#ifndef __COMPLEX__  //防御式声明
#define __COMPLEX__


//0.前置声明
#include <cmath>

class complex;
complex&
    __doapl (complex* ths, const complex& r);


//1.类声明
//template<typename T> 模板，使用时把double换成T
class complex  //class head
//class body ,有些函数直接在body里定义，有些在body外定义
{
public:  //访问级别公开，构造函数，供外界使用
    //函数若在class body内定义完成，则自动成为inline候选，有的复杂函数编译器没有能力将其做成inline
    complex (double r = 0,double i = 0)  //r,i是默认实参，参数传递pass by value
        : re (r), im (i)  //初始值，初始列
    { }  //函数(inline)
    /*上句相当于：complex (double r = 0,double i = 0)
                { re = r; im = i; } //直接赋值
        但是上句更好：有初始化阶段，效率高
    */
    /*complex () : re(0), im(0) { }  //构造函数可以有多个：重载，重载的函数在编译器里是不一样的
        这里这句不能和上句一起，因为上句有指定r,i初始值，函数调用如complex c1;complex c2();时
        编译器无法选择应该是哪一个complex函数
    */    
    complex& operator += (const complex&);  //参数传递pass by reference to const  返回值return by reference
    double real () const { return re; }  //函数(inline)  const:不改变数据内容  返回值按值传递
    double imag () const { return im; }  //函数(inline)  const:不改变数据内容  返回值按值传递
private:  //访问级别不公开，封装数据
    double re, im;
//protected: 访问级别受保护
    friend complex& __doapl (complex*, const complex&);  //返回值按引用传递
    //friend可以拿private的数据，打破封装
};
/*
    0.模板使用：complex<double> c1(2.5,1.5);
    complex<int> c2(2,6);
    不用模板：complex c1(2,1); complex c2;（默认为0,0)

    1.正确的打印：cout << c1.real();
              cout << c1.imag();
    错误的打印：cout << c1.re;
              cout << c1.im; 这里访问级别是private所以不能直接访问
    把构造函数放在private里就不能被外界调用了（但有时设计需要 eg.singleton）

    2.动态创建复数：complex* p = new complex(4);

    3.关于const: const complex c1(2,1);这种调用
        如果函数里没加const而使用者却这么调用，会产生矛盾，编译器拒绝使用者这种调用
    
    4.参数传递：有时候直接传值可能值太大了，可以传它的地址
      按值传递：pass by value
      按引用传递：pass by reference(to const) ，加了const就不会对传的东西做更改
      引用在底部其实就是指针，传引用相当于传指针那么快，但引用的形式更好
      良好的习惯：参数最好传引用，在值为一个字符之类的情况下传值也可以
        例：complex c1(2,1);  按值传递
           c2 += c1;  按引用传递
      返回值的传递也尽量按引用。

    5.相同class的各个objects互为friends

    6.关于返回值能否按引用传递：
        函数的运算结果放在哪？两种：1.原本的位置 2.创建新的位置
        如果是2，那么这个局部变量在作用域之外就死亡了，这种情况下不能返回引用

*/



//2.类定义
//2.1
inline complex&  //返回值按引用传递
__doapl (complex* ths, const complex& r)  //第一个参数会被改动，第二个不会被改动 do assignment plus
{
    //自由取得friend的private成员，如果没有设置friend,数据就要通过public的函数去拿
    ths->re += r.re;
    ths->im += r.im;
    return *ths;  //返回指针所指的值
}
//这里函数返回值按引用从传递但实际return是指针所指的值，矛盾吗？
//传递者无需知道接收者用什么形式(refernce)接收

inline complex&
complex::operator += (const complex& r)
{
    return __doapl (this, r);
}

//2.2
inline double  //inline类似宏但没有宏的缺点
imag(const complex& x)  //没说class,是global函数
{
    return x.imag ();
}
inline double
real(const complex& x)
{
    return x.real ();
}

//2.3 加法。为了应对client的三种可能用法，这里对应开发三个函数
inline complex
operator + (const complex& x, const complex& y)
{
    return complex (real(x) + real(y),
                    imag(x) + imag(y));
    //临时对象：typename();这里typename是complex
    //调用没有名称如complex();complex(2,5)也是临时对象
}
inline complex
operator + (const complex& x, double y)
{
    return complex (real(x) + y, imag(x));
}
inline complex
operator + (double x, const complex& y)
{
    return complex (x + real(y), imag(y));
}
//上述函数绝对不能按引用返回，因为它们返回的必定是local object（新创建的位置）

//2.4 正负号取正取反
inline complex
operator + (const complex& x)
{
    return x;
    //这个函数可以写成return by reference的形式
}
inline complex
operator - (const complex& x)
{
    return complex(-real (x), -imag (x));
    //这个函数不能return by reference，因为其返回的是local object
}

//2.5判断相等
inline bool
operator == (const complex& x, const complex& y)
{
    return real(x) == real(y) 
        && imag(x) == imag(y);
}
inline bool
operator == (const complex& x, double y)
{
    return real(x) == y && imag(x) == 0;
}
inline bool
operator == (double x, const complex& y)
{
    return x == real(y) && imag(y) == 0;
}

//2.6判断不相等
inline bool
operator != (const complex& x, const complex& y)
{
    return real(x) != real(y) 
        || imag(x) != imag(y);
}
inline bool
operator != (const complex& x, double y)
{
    return real(x) != y || imag(x) != 0;
}
inline bool
operator != (double x, const complex& y)
{
    return x != real(y) || imag(y) != 0;
}

//2.7 共轭复数
inline complex
conj (const complex& x)
{
    return complex (real(x), -imag(x));
}
//输出
ostream&
operator << (ostream& os, const complex& x)
{
  return os << '(' << real (x) << ',' << imag (x) << ')';
}//不能用void:使用者连串输出时不行
/*
    0.操作符重载-1，成员函数：成员函数有隐藏的参数this(不能写出来),谁调用函数谁就是this，如
        c2 += c1; 
                对于inline complex&
                complex::operator += ((this),const complex& r)
                {
                    return __doapl (this, r);
                }
        c2就相当于this,c1相当于r

    1.为什么inline complex&
        complex::operator += (const complex& r)的类型是complex&而不是void?
      如果这样使用：c3 += c2 += c1; 首先c2 += c1,然后c3 += c2,这里c2 += c1的结果不应该是void

    2.操作符重载-2，非成员函数（无this）：2-3对于全局函数的写法
*/

#endif