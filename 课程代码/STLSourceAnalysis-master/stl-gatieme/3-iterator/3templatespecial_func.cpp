#include <iostream>
#include <cstring>

///  ģ���ػ�

template <class T>
int compare(const T left, const T right)
{
    std::cout <<"in template<class T>..." <<std::endl;
    return (left - right);
}


//  �����һ���ػ��ĺ���ģ��
template < >
int compare<const char*>(const char* left, const char* right)
{
    std::cout <<"in special template< >..." <<std::endl;

    return strcmp(left, right);
}
//  �ػ��ĺ���ģ��, �����ػ���ģ�汾����ͬ, ��˱������ᱨ��
// error: redefinition of 'int compare(T, T) [with T = const char*]'|
//template < >
//int compare(const char* left, const char* right)
//{
//    std::cout <<"in special template< >..." <<std::endl;
//
//    return strcmp(left, right);
//}


//  �����ʵ�����Ǻ�������
int compare(char* left, char* right)
{
    std::cout <<"in overload function..." <<std::endl;

    return strcmp(left, right);
}

int main( )
{
    compare(1, 4);

    const char *left = "gatieme";
    const char *right = "jeancheng";
    compare(left, right);

    return 0;
}
