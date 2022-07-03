class Account {
public:
  static double m_rate; // 声明
  static void set_rate(const double &x) { m_rate = x; }
};
double Account::m_rate =
    8.0; // 定义（声明只指明了变量类型和名字，定义则为其分配了内存）
class A {
public:
  static A &getInstance();
  void setup() {}

private:
  A(){}; // 构造函数私有
};

A &A::getInstance() {
  static A a;
  return a;
}
template <class T> inline const T &min(const T &a, const T &b) {
  return b < a ? b : a;
}
class stone {
public:
  stone(int w, int h, int we) : _w(w), _h(h), _weight(we) {}
  bool operator<(const stone &rhs) const { return _weight < rhs._weight; }

private:
  int _w, _h, _weight;
};
int main() {
  Account::set_rate(5.0); // 通过 class name 调用

  Account a;
  a.set_rate(7.0); // 通过 object 调用

  stone r1(2, 3, 1), r2(3, 3, 1);
  auto r3 = min(r1, r2);
}