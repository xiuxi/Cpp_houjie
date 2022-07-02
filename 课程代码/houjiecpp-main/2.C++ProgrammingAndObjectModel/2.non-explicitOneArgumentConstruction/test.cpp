using namespace std;

#include <iostream>
#include <string>

// non-explicit-one-argument ctor
// 只需要一个实参，可以默认将其他转换为这个类
class Fraction
{
public:
    /* explicit */ Fraction(int num, int den = 1)
        : m_numerator(num), m_denominator(den) {}

/*
     operator double() const {
        return (double)(m_numerator / m_denominator);
    } 
*/
    Fraction operator+(const Fraction &f)
    {
        return Fraction(3);
    }

private:
    int m_numerator;
    int m_denominator;
};

int main(int argc, char *argv[])
{

    Fraction f(3, 5);
    //这一句话，编译器会先去找有没有重载+，如果没有才会执行强制类型转含函数。

    //在构造函数中，如果没有explicit的时候会将4转换成一个Fraction
    //如果由explicit，编译器会报错，没有+操作符
    Fraction d = f + 4; 
    
    cout << "d = " << endl;
    system("pause");
    return 0;
}