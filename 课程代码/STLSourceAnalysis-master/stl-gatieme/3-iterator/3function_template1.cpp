///c++11 ����1�����ģ�������Ƶ�
///http://blog.csdn.net/coolmeme/article/details/43986163
///http://blog.csdn.net/shinehoo/article/details/5722362
#include <iostream>

template <class Iter, class T>
void func_impl(Iter iter, T t)

{
    T tmp;              //  ������������, T���ǵ�������ָ֮�������, ����Ϊint

    /// ...  ������ԭ��funcӦ������ȫ������
}




template<class Iter>
inline func(Iter iter)
{
    func_impl(iter, *iter);
}


int main( )
{
    int i;
    func(&i);
}
