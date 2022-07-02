///c++11 ����1�����ģ�������Ƶ�
///http://blog.csdn.net/coolmeme/article/details/43986163
///http://blog.csdn.net/shinehoo/article/details/5722362
///  STLԴ������ PDF-119/534

#include <iostream>
#include <typeinfo>


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


///// ������C������������
//template <typename T>
//class C             //  ��������汾�������TΪ��������
//{
//    // NOP...
//};
//
//
///// �ػ�����C����ԭ����ָ����Ϊ����
//template <typename T>
//class C<T*>             //  ��������汾������"TΪԭ��ָ������"
//{
//    //  TΪԭ��ָ�����TΪ�κ��ͱ��һ������һ������������
//    // NOP...
//};


//  ������iterator_traits����
template <typename Iter>
struct iterator_traits
{
    typedef typename Iter::value_type value_type;
};

// �ػ���iterator_traits����<T*>����, ��ȡ��һ��T����
template<class T>
struct iterator_traits<T *>
{
    typedef T value_type;
};

// �ػ���iterator_traits����<const T*>����, ��ȡ��һ��T��
template<class T>
struct iterator_traits<const T *>
{
    typedef T value_type;
};


template <class Iter>
//typename Iter::value_type          ///  ��һ����func�ķ���ֵ����
typename iterator_traits<Iter>::value_type
func(Iter iter)
{
    ///
    return *iter;
}



int main(void)
{

    MyIter<int> ite(new int(8));
    std::cout <<func(ite) <<std::endl;


    std::cout <<typeid(iterator_traits< MyIter<int> >::value_type).name();
    std::cout <<typeid(iterator_traits<int *>::value_type).name();
    std::cout <<typeid(iterator_traits<const int *>::value_type).name();


    return 0;
}
