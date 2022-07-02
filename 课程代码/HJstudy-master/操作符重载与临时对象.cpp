#include <iostream>
using namespace std;

class complex{
    public:
        complex (double r = 0,double i = 0)
        :re(r),im(i)
        {}
        complex& operator += (const complex&);
        double real()const{return re;}     
        double imag()const{return im;}      

        int func(const complex& param) 
        {return param.re + param.im;}
    private:
        double re,im;

        friend complex& __doapl (complex*,const complex&);
};

ostream&
operator << (ostream& os,const complex& x){
    return os<< '('<<x.real()<<','<<x.imag ()<<')';
};

inline complex&
__doapl (complex* ths,const complex& r)  {                                      
    ths->re += r.re;                     
    ths->im += r.im;
    return *ths;
}
//1. 成员函数
inline complex&
complex::operator += (/*this,*/const complex& r) ////此处其实有一个隐藏参数——“this”指针
{
    return __doapl(this,r);
}

//2.非成员函数
inline double
imag (const complex& x)
{
    return x.imag();
}
inline double
real (const complex& x)
{
    return x.real();
}
///////以下三个函数////必不能返回引用///////返回必定是local object///////////
//////////////////////////////////////////没有this接收计算结果/////////////
inline complex
operator + (const complex& x,const complex& y)               //c2 = c1 + c2;
{
    return complex (real (x) + real(y),imag(x) + imag (y));
}
inline complex
operator + (const complex& x,double y)                       //c2 = c1 + 5;
{
    return complex (real (x) + y,imag(x));
}
inline complex
operator + (double x,const complex& y)                      //c2 = 7 + c1;
{
    return complex (x + real(y),imag (y));
}
////操作符'<<'重载只能全局，不能写成成员函数
////操作符重载   1. 成员函数    2.非成员函数
int main()
{
    complex c1(2,5);
    complex c2(5);
    complex c3(7);

    c2 += c1;
    c3 += c2 += c1;    //正因为返回complex& ,可以做连加操作。如果返回void，只能加1次。

    //临时对象  typename（）
    int(7);

    complex c1(1,2);
    complex c2;
    complex();
    complex(1,2);


    return 0;
}