#include <iostream>

#include "complex.h"
using namespace std;

int main() {
    //complex *pc = new complex();
    complex *pc;
    void* mem = operator new (sizeof(complex));
    pc = static_cast<complex*>(mem);
    pc->complex::complex(1, 2);
    // complex::complex(pc, 1, 2); // err
    return 0;
}