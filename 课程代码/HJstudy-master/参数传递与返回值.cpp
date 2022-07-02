#include <iostream>
using namespace std;

class complex{
    public:
        complex (double r = 0,double i = 0)
        :re(r),im(i)
        {}
        complex& operator += (const complex&);
        double real()const{return re;}      //const 函数名后面  只拿，不改变数据
        double imag()const{return im;}      //如果只取，建议加const

        int func(const complex& param)  // 同一个类的各对象互为友元
        {return param.re + param.im;}
    private:
        double re,im;

        friend complex& __doapl (complex*,const complex&);
};

ostream&
operator << (ostream& os,const complex& x){
    return os<< '('<<x.real()<<','<<x.imag ()<<')';
};
    //参数传递
    //value   reference传参
    //尽量不要pass by value而是reference
    //返回也尽量 by reference

//友元
//可以直接用“.”访问，直接拿
inline complex&
__doapl (complex* ths,const complex& r)  //第一个参数会被改动
{                                        //第二个参数不会被改动
    ths->re += r.re;                     //函数内部创建的变量、类等不能返回引用
    ths->im += r.im;
    return *ths;
}
//class body 外的各种定义
//什么情况传引用或者返回引用
inline complex&
complex::operator += (const complex& r)
{
    return __doapl(this,r);
}

int main()
{
    complex c1(2,1);
    cout<<c1.real();
    cout<<c1.imag();

    const complex c2(2,1);    //real（）不加const，而c1是const 出错
    cout<<c2.real();
    cout<<c2.imag();    


    complex c3(2,1);
    complex c4;

    c1 += c2;
    cout <<c2;

    //相同类的各对象互为友元
    complex f1(2,1);
    complex f2;

    f2.func(f1);


    return 0;
}