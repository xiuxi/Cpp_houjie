#include <iostream>
#include "hz_alloctor.h"

using namespace std;

class A
{
public:
    A()
    {
        cout << "A's constructor" << endl;
    }

    ~A()
    {
        cout << "A's destructor" << endl;
    }

    void show()
    {
        cout << "num:" << num << endl;
    }

private:
    int num;
};

class B
{
public:
    B()
    {
        cout << "B's constructor" << endl;
    }

    ~B()
    {
        cout << "B's destructor" << endl;
    }

    void SetNum(int n)
    {
        num = n;
    }

    int GetNum()
    {
        return num;
    }

private:
    int num;
};

template <int inst>
class test
{
};

union obj
{
    union obj *free_list_link1;
    char client_data[1];
};

obj a;

int main()
{
    cout << sizeof(obj) << endl;
    cout << sizeof(int *) << endl;
    cout<<a.free_list_link1<<"  "<<a.client_data<<endl;
    char mem[100];
    mem[0] = 'A';
    mem[1] = '\0';
    mem[2] = '\0';
    mem[3] = '\0';
    cout << (void *)mem << endl;
    A *p = new (mem) A;
    cout << p << endl;
    p->show();
    p->~A();

    A *a_1 = (A *)(::operator new(sizeof(A))); // operator new
    new (a_1) A(*p);
    a_1->~A();
    ::operator delete(a_1);

    char *mem_1 = new char[10 * sizeof(B)];
    cout << (void *)mem_1 << endl;
    B *p_b = new (mem_1) B; // placement new
    cout << p_b << endl;
    p_b->SetNum(10);
    cout << p_b->GetNum() << endl;
    p_b->~B();
    delete[] mem_1;
    getchar();
}
/* int main()
{
    cout << "iostream" << endl;
    system("pause");
    return 0;
} */