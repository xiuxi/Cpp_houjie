// F:\侯捷 - C++面向对象高级开发\Cpp_houjie\课程代码\complex.h

class complex {
public:
  complex(double r = 0, double i = 0) : re(r), im(i) {}
  complex &operator+=(const complex &);
  complex &operator-=(const complex &);
  complex &operator*=(const complex &);
  complex &operator/=(const complex &);
  double real() const { return re; }
  double imag() const { return im; }

private:
  double re, im;

  friend complex &__doapl(complex *, const complex &);
  friend complex &__doami(complex *, const complex &);
  friend complex &__doaml(complex *, const complex &);
};

inline complex &__doapl(complex *ths, const complex &r) {
  ths->re += r.re;
  ths->im += r.im;
  return *ths;
}

inline complex &complex::operator+=(const complex &r) {
  return __doapl(this, r);
}

int main(int argc, char const *argv[]) {
  complex c1(1, 1);
  complex c2(2, 2);
  c2 += c1;
  // complex c3 = c1 + c2;

  return 0;
}
