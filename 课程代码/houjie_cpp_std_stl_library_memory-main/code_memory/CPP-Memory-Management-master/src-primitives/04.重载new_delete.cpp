#include <cstddef>
#include <iostream>
#include <string>
using namespace std;

namespace jj06
{

	class Foo
	{
	public:
		int _id;
		long _data;
		string _str;

	public:
		static void* operator new(size_t size);
		static void  operator delete(void* deadObject, size_t size);
		static void* operator new[](size_t size);
		static void  operator delete[](void* deadObject, size_t size);

		Foo() : _id(0)      { cout << "default ctor. this=" << this << " id=" << _id << endl; }
		Foo(int i) : _id(i) { cout << "ctor. this=" << this << " id=" << _id << endl; }
		//virtual 
		~Foo()              { cout << "dtor. this=" << this << " id=" << _id << endl; }

		//���� virtual dtor, sizeof = 12, new Foo[5] => operator new[]() �� size ������ 64, 
		//���� virtual dtor, sizeof = 16, new Foo[5] => operator new[]() �� size ������ 84, 
		//���������������� 4 ���ܾ��ǂ� size_t ��λ�Á���� array size. 
	};

	void* Foo::operator new(size_t size)
	{
		Foo* p = (Foo*)malloc(size);
		cout << "Foo::operator new(), size=" << size << "\t  return: " << p << endl;

		return p;
	}

	void Foo::operator delete(void* pdead, size_t size)
	{
		cout << "Foo::operator delete(), pdead= " << pdead << "  size= " << size << endl;
		free(pdead);
	}

	void* Foo::operator new[](size_t size)
	{
		Foo* p = (Foo*)malloc(size);  //crash, ���}���ܳ����@�� 
		cout << "Foo::operator new[](), size=" << size << "\t  return: " << p << endl;

		return p;
	}

	void Foo::operator delete[](void* pdead, size_t size)
	{
		cout << "Foo::operator delete[](), pdead= " << pdead << "  size= " << size << endl;

		free(pdead);
	}

	//-------------	
	void test_overload_operator_new_and_array_new()
	{
		cout << "\ntest_overload_operator_new_and_array_new().......... \n";

		cout << "sizeof(Foo)= " << sizeof(Foo) << endl;

		{
			Foo* p = new Foo(7);
			delete p;

			Foo* pArray = new Foo[5];	//�o���o array elements �� initializer 
			delete[] pArray;
		}

		{
			cout << "testing global expression ::new and ::new[] \n";
			// �@���@�^ overloaded new(), delete(), new[](), delete[]() 
			// ����Ȼ ctor, dtor ��������������.  

			Foo* p = ::new Foo(7);
			::delete p;

			Foo* pArray = ::new Foo[5];
			::delete[] pArray;
		}
	}
} //namespace

int main(void)
{
	jj06::test_overload_operator_new_and_array_new();
	return 0;
}