///c++11 ����1�����ģ�������Ƶ�
///http://blog.csdn.net/coolmeme/article/details/43986163
///http://blog.csdn.net/shinehoo/article/details/5722362
///  STLԴ������ PDF-119/534

#include <iostream>
template <class T>
struct MyIter
{
    MyIter(T *p = NULL)
    :m_ptr(p)
    {
        /// NOP...
    }

    T& operator*( ) const
    {
        return *m_ptr;
    }



    typedef T value_type;       //  ��Ƕ�ͱ�����{nested type}
    T   *m_ptr;
};


template <class Iter>
typename Iter::value_type          ///  ��һ����func�ķ���ֵ����
func(Iter iter)
{
    ///
    return *iter;
}


int main(void)
{
    MyIter<int> ite(new int(8));
    std::cout <<func(ite) <<std::endl;
    return 0;
}
