#include <vector>  //for test
#include <cstddef>
#include <iostream>
#include <string>
using namespace std;

namespace jj07
{

	class Bad { };
	class Foo
	{
	public:
		Foo() { cout << "Foo::Foo()" << endl; }
		Foo(int) {
			cout << "Foo::Foo(int)" << endl;
			// throw Bad();  
		}

		//(1) �@������һ��� operator new() �����d 
		void* operator new(size_t size){
			cout << "operator new(size_t size), size= " << size << endl;
			return malloc(size);
		}

		//(2) �@�����ǘ˜ʎ��ѽ��ṩ�� placement new() �����d (��ʽ)
		//    (������Ҳģ�M standard placement new �Ą���, just return ptr) 
		void* operator new(size_t size, void* start){
			cout << "operator new(size_t size, void* start), size= " << size << "  start= " << start << endl;
			return start;
		}

		//(3) �@�����Ǎ��µ� placement new 
		void* operator new(size_t size, long extra){
			cout << "operator new(size_t size, long extra)  " << size << ' ' << extra << endl;
			return malloc(size + extra);
		}

		//(4) �@����һ�� placement new 
		void* operator new(size_t size, long extra, char init){
			cout << "operator new(size_t size, long extra, char init)  " << size << ' ' << extra << ' ' << init << endl;
			return malloc(size + extra);
		}

		//(5) �@����һ�� placement new, �����⌑�e��һ������ type (������� size_t �ԝM�������� operator new) 
		//!  	void* operator new(long extra, char init) { //[Error] 'operator new' takes type 'size_t' ('unsigned int') as first parameter [-fpermissive]
		//!	  	cout << "op-new(long,char)" << endl;
		//!    	return malloc(extra);
		//!  	} 	

		//�����Ǵ������� placement new �ĸ��� called placement delete. 
		//�� ctor �l���������@�������� operator (placement) delete �͕�������. 
		//��ԓ��Ҫؓ؟ጷ����n�ֵ� (placement new) �������õ� memory.  
		//(1) �@������һ��� operator delete() �����d 
		void operator delete(void*, size_t)
		{
			cout << "operator delete(void*,size_t)  " << endl;
		}

		//(2) �@�ǌ��������� (2)  
		void operator delete(void*, void*)
		{
			cout << "operator delete(void*,void*)  " << endl;
		}

		//(3) �@�ǌ��������� (3)  
		void operator delete(void*, long)
		{
			cout << "operator delete(void*,long)  " << endl;
		}

		//(4) �@�ǌ��������� (4)  
		//����]��һһ����, Ҳ�������κξ��g���e 
		void operator delete(void*, long, char)
		{
			cout << "operator delete(void*,long,char)  " << endl;
		}

	private:
		int m_i;
	};


	//-------------	
	void test_overload_placement_new()
	{
		cout << "\n\n\ntest_overload_placement_new().......... \n";

		Foo start;  //Foo::Foo

		Foo* p1 = new Foo;           //op-new(size_t)
		Foo* p2 = new (&start) Foo;  //op-new(size_t,void*)
		Foo* p3 = new (100) Foo;     //op-new(size_t,long)
		Foo* p4 = new (100, 'a') Foo; //op-new(size_t,long,char)

		Foo* p5 = new (100) Foo(1);     //op-new(size_t,long)  op-del(void*,long)
		Foo* p6 = new (100, 'a') Foo(1); //
		Foo* p7 = new (&start) Foo(1);  //
		Foo* p8 = new Foo(1);           //
		//VC6 warning C4291: 'void *__cdecl Foo::operator new(unsigned int)'
		//no matching operator delete found; memory will not be freed if
		//initialization throws an exception
	}
} //namespace	

int main(void)
{
	jj07::test_overload_placement_new();
	return 0;
}