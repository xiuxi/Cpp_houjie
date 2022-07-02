#include "String.h"
#include <iostream>
using namespace std;

int main()
{
	String s1("");
	String s2("hello");

	cout << s1 << endl;
	cout << s2 << endl;

	String s3(s2);

	cout << s3 << endl;

	s3 = s2;
	cout << s3 << endl;   

	return 0;
}