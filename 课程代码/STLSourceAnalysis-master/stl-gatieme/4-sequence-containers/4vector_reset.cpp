#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>


using std::cout;
using std::endl;
using std::vector;
int main( )
{

    vector<int> iv(3, 9);
    std::cout <<"size = " <<iv.size() <<", ";                //  3
    cout <<"capacity = " <<iv.capacity() <<endl;             //  3

    vector<int>::iterator itdegin1 = iv.begin();
    vector<int>::iterator itend1 = iv.end();
    cout <<"begin = " <<*itdegin1 <<", ";
    cout <<"end   = " <<*(itend1 - 1) <<endl;

    /// ѹ��һ������֮��, ���������ò������ԭ����2��
    iv.push_back(1);
    cout <<"size = " <<iv.size() <<", ";                //  4
    cout <<"capacity = " <<iv.capacity() <<endl;        //  6
    /// ERROR...  �������ú�, ԭ���ĵ�������ʧЧ
    cout <<"begin = " <<*itdegin1 <<", ";
    cout <<"end   = " <<*(itend1 - 1) <<endl;

    return 0;
}
