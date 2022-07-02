#include <iostream>

using namespace std;
/* reference ��ʵ����ָ�룬�������ֳ������﷨���Ǳ��������ļ��󣬻���˵reference����һ��Ư����ָ�� */
typedef struct Stag
{
    int a, b, c, d;
} S;

void print(int a)
{
    cout << "this is first" << a << endl;
}

int main()
{
    double x = 0;
    double *p = &x;
    double &r = x;

    cout << "sizeof(x) = " << sizeof(x) << endl;
    cout << "sizeof(p) = " << sizeof(p) << endl;
    cout << "sizeof(r) = " << sizeof(r) << endl;

    cout << p << endl;
    cout << *p << endl;
    cout << x << endl;
    cout << r << endl;
    cout << &x << endl;
    cout << &r << endl;

    S s;
    S &rs = s;
    cout << "sizeof(S) = " << sizeof(s) << endl;
    cout << "sizeof(re) = " << sizeof(rs) << endl;
    cout << &s << endl;
    cout << &rs << endl;

    int a = 1;
    print(a);

    system("pause");
    return 0;
}