#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    int ia[6] = {27, 210, 12, 47, 109, 83};
    vector<int, allocator<int>> vi(ia, ia + 6);
    cout << count_if(vi.begin(), vi.end(), not1(bind2nd(less<int>(), 40))) << endl;

    cout << count_if(ia, ia + 6, not1(bind1st(greater<int>(), 40))) << endl;

    for (auto i : ia)
        cout << i << " ";
    system("pause");
    return 0;
}