#include <bitset>
#include <iostream>
#include <vector>
using namespace std;
template <typename T, typename... Types>
void print(const T &firstArg, const Types &...args) {
  cout << firstArg << endl;
  print(args...);
}

int main(int argc, char const *argv[]) {
  // print(7.5,"hello",bitset<16>(377),42);
  //   for( decl : coll){
  //     statement
  // }
  for (int i : {2, 3, 5, 7, 9, 13, 17, 19}) {
    cout << i << endl;
  }
  vector<double> vec;
  for (auto elem : vec) {
    cout << elem << endl;
  }

  for (auto &elem : vec) {
    elem *= 3;
  }
  return 0;
}
