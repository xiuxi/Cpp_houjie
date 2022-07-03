//-------------------------------------------------------------
#include <iostream>
#include <memory> //shared_ptr
#include <string>
// using namespace std;
namespace jj01 {
class Base1 {};
class Derived1 : public Base1 {};
class Base2 {};
class Derived2 : public Base2 {};

template <class T1, class T2> struct pair {
  typedef T1 first_type;
  typedef T2 second_type;

  T1 first;
  T2 second;
  pair() : first(T1()), second(T2()) {}
  pair(const T1 &a, const T2 &b) : first(a), second(b) {}

  template <class U1, class U2>
  pair(const pair<U1, U2> &p) : first(p.first), second(p.second) {}
};

void test_member_template() {
  std::cout << "test_member_template()" << std::endl;
  pair<Derived1, Derived2> p;
  pair<Base1, Base2> p2(pair<Derived1, Derived2>());
  pair<Base1, Base2> p3(p);
  // Derived1 will be assigned to Base1; Derived2 will be assigned to Base2.
  // OO allow such assignments since of "is-a" relationship between them.

  //!	pair<Derived1, Derived2> p4(p3);
  // error messages as below appear at the ctor statements of pair member
  // template
  //  [Error] no matching function for call to 'Derived1::Derived1(const
  //  Base1&)' [Error] no matching function for call to
  //  'Derived2::Derived2(const Base2&)'

  Base1 *ptr = new Derived1;                 // up-cast
  std::shared_ptr<Base1> sptr(new Derived1); // simulate up-cast
  // Note: make sure your environment
  // support C++2.0 at first.
}
} // namespace jj01
int main() {
  jj01::test_member_template();
  return 0;
}