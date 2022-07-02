#include <memory>
#include <iostream>
using namespace std;

class A
{
public:
    A( )
    {
        id = ++count;
        cout <<"create A" <<id  <<std::endl;
    }

    ~A()
    {
        cout <<"destroy A" <<id <<std::endl;
    }

private:
    static int count;
    int id;
};

int A::count = 0;

/*  ���øú����ᶪʧ������Ȩ */
void sink(auto_ptr<A> a)
{
    cout << "Enter sink()\n";
}

/*  ���øú����ᴴ�����󣬲���ȡ����Ȩ  */
auto_ptr<A> create()
{
    cout << "Enter create()\n";
    auto_ptr<A> a(new A());
    return a;
}

int main(int argc, char *argv[])
{
    //auto_ptr<string> pstr(new string("abcd"));
    //auto_ptr<string> pstr = new string("abcd");
    auto_ptr<A> a1 = create();
    cout << "Exit create()\n";


    auto_ptr<A> a2 = a1;            /* ��ֵ������ת������Ȩ����ʱa1��Ч��*/

    auto_ptr<A> a3(new A());


    sink(a2);                       /* ���λᶪʧ����Ȩ���ᷢ��a2���ͷ���sink�����н���*/

    cout << "Exit sink()\n";
    return 0;
}
