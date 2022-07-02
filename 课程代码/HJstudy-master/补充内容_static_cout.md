# static
*****
|**complex**|
|:---------:|
|data members 
**static** data members|
|member functions  
**static** member functions|

******
非static成员函数有一个this指针
而static成员函数没有this指针，脱离对象，在内存单独存在一份
static函数只能处理static成员变量
static变量必须初始化/定义
  
调用static函数的方式有：  
1.通过object调用  
2.通过class name 调用  
  
单例模式Singleton  
一个类只创建一个对象  
在单例模式中将静态变量的定义放在静态函数里，则在单例模式没被调用时也不会创建一个没被用到的变量  
  
# cout
*****
cout内有ostream：：<< 的重载
  
# 类模板
```
    template<typename T>   //模板会造成代码膨胀，但是必要的膨胀
    class complex
    {
        public:
            complex(T r = 0,T i = 0)
            : re(r),im (i)
            { }
            complex& operator += (const complex&);
            T real()const{return re;}
            T imag()const{return im;} 
        private:
            T re,im;
            
            friend complex& __doapl (complex*,const complex&);
    }
```
  
# 函数模板
```
    template <class T>
    inline
    const T& min(const T& a,const T& b)
    {
        return b < a ? b : a;               //T 操作符'<'重载
    }
```
  
# namespace
```
    using namespace std;
    ...
    cin << ...;
    cout << ...;
    ...
```
```
    using std::cout;
    ...
    std::cin << ...;
    cout << ...;
    ...
```
