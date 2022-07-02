#include <iostream>

#include "string1.h"

int main() {
    String* ps = new String("Hello");

    delete ps;
    return 0;
}