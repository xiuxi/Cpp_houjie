#include <iostream>
using namespace std;

struct B                ///  B  �ɱ���ΪInputIterator
{

};

struct D1 : public B   ///  D1 �ɱ���ΪForwardIteraor
{

};

struct D2 : public D1    //  D2 �ɱ���ΪBidirectionalIterator
{

};


template <class I>
func(I &p, B)
{
    std::cout <<"B version..." <<endl;
}

template <class I>
func(I &p, D2)
{
    std::cout <<"D2 version..." <<endl;

}



int main(void)
{
    int *p;
    func(p, B());        //  ����B��ȫ�Ǻ�, ���"B version..."
    func(p, D1());       //  ����δ����ȫ�Ǻ�, ��̳й�ϵ����, ���"B version..."
    func(p, D2());       //  ����D2��ȫ�Ǻ�, ���"D2 version..."



    return 0;
}
