using namespace std;

#include <iostream>
#include <string>

// conversion function,ת������
class Fraction
{
public:
    Fraction(int num, int den = 1)
        : m_numerator(num), m_denominator(den) {}
    // ��FractionתΪdouble������Ҫ����ֵ
    operator double() const
    {
        return (double)(m_numerator / m_denominator);
    }

private:
    int m_numerator;   //����
    int m_denominator; //��ĸ
};

int main(int argc, char *argv[])
{
    Fraction f(3, 5);
    //��һ�仰������������ȥ����û������+�����û�вŻ�ִ��ǿ������ת��������
    double d = 4 + f; //����operator double()��fתΪ0.6
    cout << "d = " << d << endl;
    system("pause");
    return 0;
}