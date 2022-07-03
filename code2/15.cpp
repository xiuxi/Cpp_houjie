#include <iostream>
using namespace std;

namespace jj06 {
void test_reference() {
  cout << "test_reference()" << endl;

  {
    int x = 0;
    int *p = &x;
    int &r = x; // r is a reference to x. now r==x==0
    int x2 = 5;
    r = x2;      // r cannot re-reference to another. now r==x==5
    int &r2 = r; // now r2==5 (r2 is a reference to r, i.e. a reference to x)
  }

  {
    typedef struct Stag {
      int a, b, c, d;
    } S;

    double x = 0;
    double *p = &x; // p is a pointer to x. p's value is same as x's address.
    double &r = x;  // r is a reference to x, now r==x==0

    cout << sizeof(x) << endl; // 8
    cout << sizeof(p) << endl; // 4
    cout << sizeof(r) << endl; // 8
    cout << p << endl;         // 0065FDFC
    cout << *p << endl;        // 0
    cout << x << endl;         // 0
    cout << r << endl;         // 0
    cout << &x << endl;        // 0065FDFC
    cout << &r << endl;        // 0065FDFC

    S s;
    S &rs = s;
    cout << sizeof(s) << endl;  // 16
    cout << sizeof(rs) << endl; // 16
    cout << &s << endl;         // 0065FDE8
    cout << &rs << endl;        // 0065FDE8
  }
}
} // namespace jj06
//-------------------------------------------------------------
int main(int argc, char **argv) {
  std::cout << __cplusplus << endl; // 199711 or 201103

  // jj01::test_member_template();
  // jj02::test_template_template_parameters_1();
  // jj03::test_template_template_parameters_2();
  // jj04::test_object_model();
  // jj05::test_conversion_functions();
  jj06::test_reference();
}