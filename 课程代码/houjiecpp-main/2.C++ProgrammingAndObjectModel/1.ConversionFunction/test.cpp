using namespace std;

#include <iostream>
#include <string>

// conversion function,转换函数
class Fraction
{
public:
    Fraction(int num, int den = 1)
        : m_numerator(num), m_denominator(den) {}
    // 将Fraction转为double，不需要返回值
    operator double() const
    {
        return (double)(m_numerator / m_denominator);
    }

private:
    int m_numerator;   //分子
    int m_denominator; //分母
};

int main(int argc, char *argv[])
{
    Fraction f(3, 5);
    //这一句话，编译器会先去找有没有重载+，如果没有才会执行强制类型转含函数。
    double d = 4 + f; //调用operator double()将f转为0.6
    cout << "d = " << d << endl;
    system("pause");
    return 0;
}