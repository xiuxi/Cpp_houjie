#include <iostream>
using namespace std;
namespace jj05 {
class Fraction {
public:
  explicit Fraction(int num, int den = 1)
      : m_numerator(num), m_denominator(den) {
    cout << m_numerator << ' ' << m_denominator << endl;
  }

  operator double() const { return (double)m_numerator / m_denominator; }

  Fraction operator+(const Fraction &f) {
    cout << "operator+(): " << f.m_numerator << ' ' << f.m_denominator << endl;
    //... plus
    return f;
  }
  /*
          Fraction(double d)
            : m_numerator(d * 1000), m_denominator(1000)
          { cout << m_numerator << ' ' << m_denominator << endl; }
  */

private:
  int m_numerator;   //
  int m_denominator; //
};

void test_conversion_functions() {
  cout << "test_conversion_functions()" << endl;

  Fraction f(3, 5);

  double d = 4 + f; // 4.6
  cout << d << endl;

  //! Fraction d2 = f + 4;	 //ambiguous
}
} // namespace jj05

int main() {
  jj05::test_conversion_functions();
  return 0;
}