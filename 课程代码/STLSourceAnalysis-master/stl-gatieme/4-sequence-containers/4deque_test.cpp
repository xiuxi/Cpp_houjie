#include <iostream>
#include <algorithm>
#include <deque>
#include <iomanip>
#include <cstdlib>


using namespace std;
void Print(int a)
{
    cout <<std::setw(4) <<a;

}

int main( )
{
    deque<int, alloc, 32> ideq(20, 9);              //  allocֻ������G++������
    cout <<"size = " <<ideq.size( );
    /// ���ڹ�����һ��deque, ��20��intԪ��, ��ֵ��Ϊ9
    /// ��������СΪ32byte

    ///  Ϊÿ��Ԫ���趨��ֵ
    for(int i = 0; i < ideq.size( ); i++)
    {
        ideq[i] = i;
    }
    cout <<endl;



    return 0;
}

