#include <iostream>
#include <bitset>

using namespace std;

/**
 * 1. variadic templates(�ɱ������ģ��)
 * ����Ҫ֪��ģ�����������ʹ��sizeof...(args...)
 **/
void print()
{
    cout << endl;
    /* ���һ��print�Ǹ��պ������������Ҳ����䶨�� */
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
 * ite = ����
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