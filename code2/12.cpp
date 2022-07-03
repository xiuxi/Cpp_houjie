template <typename T, template <typename T> class Container> class XCls {
private:
  Container<T> c;

public:
};
// template <typename T, template <typename T> 
// class SmartPtr> 
// class XCls {
// private:
//   SmartPtr<T> sp;

// public:
//   XCls() : sp(new T) {}
// };
// template <class T, class Sequence = deque<T>> class stack {
//   friend bool operator==<>(const stack &, const stack &);
//   friend bool operator< <>(const stack &, const stack &);

protected:
  Sequence c; //底层容器
};
int main(int argc, char const *argv[]) {
//   template <typename T> using Lst = list<T, allocator<T>>;
//   XCls<string, list> mylst1; //错
//   XCls<string, Lst> mylst2;

//   XCls<string, shared_ptr> p1;
//   XCls<double, unique_ptr> p2; //错
//   XCls<int, weak_ptr> p3;      //错
//   XCls<long, auto_ptr> p4;

//   stack<int> s1;

//   stack<int, list<int>> s2;
  return 0;
}
