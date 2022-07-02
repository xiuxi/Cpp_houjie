# Traits 编程技法
## 问题背景
迭代器所指对象的类型一般被声明typedef为value_type,如此做可以将value_type作为函数的返回类型。  
```C++
template <class _Ty>
class array{
   typedef  _Ty value_type;
}
```
问题是当Iterator是一个自定义class时，我们可以很方便地使用value_type,如
```C++
array::value_type val;
```
但是如果Iterator是C++自定义地五种基本类型的指针(int*\float*等)，那么这时候该如何使用value_type?

## 通过Traits萃取出所需的associated type
Iterator traits(萃取机)用来分离class iterator和 非class iterator。  
需要注意的是，对于非class iterator，其value_type 也十分明显。
```C++
// 以int*为例，其value_type就是int
int* a;// 实际上Iterator是一种泛化的指针，所以指针也算是一种iterator
```
## 简单的Traits例子
```C++
template<class T>
struct iterator_traits{
    // T::value_type即是调用T的类内自定义类型
    typedef typename T::value_type valuetype;
}

template<class T>
// 通过template偏特化实现
struct iterator_traits<T*>{
    // T本身就是类型
    typedef T value_type;
}

template<class T>
struct iterator_traits<const T*>{
    typedef T value_type;
}
```
## 结论
以上，算法在拿到任意一个iterator时，就可以直接通过萃取机直接使用其类型。