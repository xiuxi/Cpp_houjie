#include <iostream>
using namespace std;

void *myAlloc(size_t size) {
  return malloc(size); //分配内存
}

// 编译错误
// inline void *operator new(size_t size) //重载 operator new
// {
//   cout << "jjhou global new()	\n";
//   return myAlloc(size);
// }

inline void *operator new[](size_t size) //重载 operator new[]
{
  cout << "jjhou global new[]()	\n";
  return myAlloc(size);
}

int main(int argc, char const *argv[]) { return 0; }
