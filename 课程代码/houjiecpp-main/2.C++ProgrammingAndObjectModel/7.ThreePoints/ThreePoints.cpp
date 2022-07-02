#include <iostream>
#include <bitset>

using namespace std;

/**
 * 1. variadic templates(可变参数的模板)
 * 当想要知道模板参数，可以使用sizeof...(args...)
 **/
void print()
{
    cout << endl;
    /* 最后一个print是个空函数，编译器找不到其定义 */
}
template <typename T, typename... Types>
void print(const T &firstArg, const Types &...args)
{
    cout << firstArg << endl;
    print(args...);
}

/**
 * 2. auto
 * list<string> c;
 * auto ite = fine(c.begin(),c.end(),target);
 * ||
 * list<string>::iterator ite;
 * ite = ……
 *
 * auto ite ;err!!!
 **/

/**
 * 3. ranged-based for
 * for( decl: coll )
 * {
 *      statement
 * }
 */
int main(int argc, char **argv)
{

    print(7.5, "hello", bitset<16>(377), 42);
    for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
    {
        cout << i << endl;
    }
    system("pause");
    return 0;
}