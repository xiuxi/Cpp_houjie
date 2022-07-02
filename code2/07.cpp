template <class T> class complex {
public:
  complex(T r = 0, T i = 0) : re(r), im(i) {}
  complex &operator+=(const complex &);
  complex &operator-=(const complex &);
  complex &operator*=(const complex &);
  complex &operator/=(const complex &);
  T real() const { return re; }
  T imag() const { return im; }

private:
  T re, im;
};

int main() {
  complex<double> c1(2.5, 1.5);
  complex<int> c2(2, 6);
  return 0;
}