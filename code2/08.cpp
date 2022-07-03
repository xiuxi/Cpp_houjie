template <class T> inline const T &min(const T &a, const T &b) {
  return b < a ? b : a;
}

class stone {
public:
  stone() {}
  stone(int w, int h) : _w(w), _h(h) {}
  stone(int w, int h, int we) : _w(w), _h(h), _weight(we) {}
  bool operator<(const stone &rhs) const { return _weight < rhs._weight; }

private:
  int _w, _h, _weight;
};
int main() {
  stone r1(2, 3);
  stone r2(3, 3);
  stone r3;
  r3 = min(r1, r2);
  return 0;
}