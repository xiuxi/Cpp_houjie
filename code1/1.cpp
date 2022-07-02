#include <stdio.h>

#include <iostream>
using namespace std;

class String {
   public:
    String(const char *cstr = 0);
    String(const String &str);
    String &operator=(const String &str);
    ~String();
    char *get_c_str() const { return m_data; }

   private:
    char *m_data;
};
inline String::~String() { delete[] m_data; }

inline String &String::operator=(const String &str) {
    if (this == &str) return *this;

    delete[] m_data;
    m_data = new char[strlen(str.m_data) + 1];
    strcpy(m_data, str.m_data);
    return *this;
}

inline String::String(const String &str) {
    m_data = new char[strlen(str.m_data) + 1];
    strcpy(m_data, str.m_data);
}

ostream &operator<<(ostream &os, const String &str) {
    os << str.get_c_str();
    return os;
}
inline String::String(const char *cstr) {
    if (cstr) {
        m_data = new char[strlen(cstr) + 1];
        strcpy(m_data, cstr);
    } else {
        m_data = new char[1];
        *m_data = ' ';
    }
}
int main() {
    String s1("hello");
    String s2("world");

    String s3(s2);
    cout << s3 << endl;

    s3 = s1;
    cout << s3 << endl;
    cout << s2 << endl;
    cout << s1 << endl;
    return 0;
}