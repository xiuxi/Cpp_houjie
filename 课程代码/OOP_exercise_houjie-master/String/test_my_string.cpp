#include "String.h"

std::ostream& operator<<(std::ostream& os, const String& str)
{
    os << str.get_c_data();
    return os;
}

int main()
{
    String a;
    std::cout << "a " << a << std::endl;
    String b("hello world");
    std::cout << "b " << b << std::endl;
    a = b;
    std::cout << "a again " << a << std::endl;
    return 0;
}
