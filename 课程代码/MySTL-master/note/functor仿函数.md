# STL中使用仿函数

在C语言中，想要将函数作为参数传递，只能将函数指针作为参数传递
```C++
bool cmp(const void* elem1, const void* elem2);

void main()
{
    int ia[4] = {32, 92, 67, 58};

    qsort(ia,sizeof(ia)/sizeof(int),sizeof(int),cmp);//传递cmp指针，作为排序的策略
}
```
但是函数指针无法持有自己的状态，以及无法加另外的条件对函数进行修饰。  
于是在STL中采用仿函数(functor),所谓仿函数就是用起来像函数的东西。其本质是对括号()运算符进行重载。
```C++
template <typename T>
class max{
    public:
    T operator()(const T& elem1, const T& elem2)
    {
        if(elem1 > elem2) return elem1;
        else return elem2;
    }
};

int main()
{
    std::cout<<max<int>()(1 ,2)<<std::endl;// return 2
    getchar();
    return 0;
}
```