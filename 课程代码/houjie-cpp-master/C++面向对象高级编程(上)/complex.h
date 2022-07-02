#ifndef __COMPLEX__
#define __COMPLEX__

class complex;
complex& _doapl(complex* ths, const complex& r);

class complex {
public:
	complex(double r = 0, double i = 0)
		: re(r), im(i)
	{ }
	complex& operator += (const complex&);

	// 此函数不能改变私有变量
	double real() const { return re; }	
	double imag() const { return im; }
private:
	double re, im;
	friend complex& _doapl(complex* ths, const complex&);
};

inline double
real(const complex& x) {
	return x.real();
}

inline double
imag(const complex& x) {
	return x.imag();
}

// 其返回的是外部已有的变量，所以可以使用reference
inline complex&
_doapl(complex* ths, const complex& r) {
	ths->re += r.re;
	ths->im += r.im;
	return *ths;
}

#include <iostream>

// 对于每一个字符的输出，cout都会改变，所以不能够返回const reference
std::ostream&
operator << (std::ostream& os, const complex& x) {
	return os << '(' << real(x) << ',' << imag(x) << ')';
}

inline complex&
complex::operator += (const complex& x) {
	return _doapl(this, x);
}

// 由于下面三个函数中，返回的是临时创建的local变量，所以不能返回reference
inline complex
operator + (const complex& x, const complex& y) {
	return complex(x.real() + y.real(), x.imag() + y.imag());
}

inline complex
operator + (double x, const complex& y) {
	return complex(x + y.real(), y.imag());
}

inline complex
operator + (const complex& x, double y) {
	return complex(x.real() + y, x.imag());
}

// 取反，编译器根据形参列表进行区分
inline complex
operator + (const complex& x) {
	return x;
}

inline complex
operator - (const complex& x) {
	return complex(-real(x), -imag(x));
}

inline bool
operator == (const complex& x, const complex& y) {
	return real(x) == real(y) && imag(x) == imag(y);
}

inline bool
operator == (const complex& x, double y) {
	return real(x) == y && imag(x) == 0;
}

inline bool
operator == (double x, const complex& y) {
	return real(y) == x && imag(y) == 0;
}

inline bool
operator != (const complex& x, const complex& y) {
	return real(x) != real(y) || imag(x) != imag(y);
}

inline bool
operator != (const complex& x, double y) {
	return real(x) != y || imag(x) != 0;
}

inline bool
operator != (double x, const complex& y) {
	return real(y) != x || imag(x) != 0;
}

// 共轭复数
inline complex
conj(const complex& x) {
	return complex(real(x), -imag(x));
}

#endif