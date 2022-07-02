#include <cstddef>
#include <iostream>
#include <complex>
using namespace std;

namespace jj09
{

	class allocator
	{
	private:
		struct obj {
			struct obj* next;  //embedded pointer
		};
	public:
		void* allocate(size_t);
		void  deallocate(void*, size_t);
		void  check();

	private:
		obj* freeStore = nullptr;
		const int CHUNK = 5; //Сһ�c�����^�� 
	};

	void* allocator::allocate(size_t size)
	{
		obj* p;

		if (!freeStore) {
			//linked list �ǿյģ����Ծ�ȡһ��K memory
			size_t chunk = CHUNK * size;
			freeStore = p = (obj*)malloc(chunk);

			//cout << "empty. malloc: " << chunk << "  " << p << endl;

			//������Á��һ��K���� linked list ��С�KС�K��������
			for (int i = 0; i < (CHUNK - 1); ++i)	{  //�]����Ư��, �������c�o���^.  
				p->next = (obj*)((char*)p + size);
				p = p->next;
			}
			p->next = nullptr;  //last       
		}
		p = freeStore;
		freeStore = freeStore->next;

		//cout << "p= " << p << "  freeStore= " << freeStore << endl;

		return p;
	}
	void allocator::deallocate(void* p, size_t)
	{
		//�� deleted object �ջز��� free list ǰ��
		((obj*)p)->next = freeStore;
		freeStore = (obj*)p;
	}
	void allocator::check()
	{
		obj* p = freeStore;
		int count = 0;

		while (p) {
			cout << p << endl;
			p = p->next;
			count++;
		}
		cout << count << endl;
	}
	//--------------

	class Foo {
	public:
		long L;
		string str;
		static allocator myAlloc;
	public:
		Foo(long l) : L(l) {  }
		static void* operator new(size_t size)
		{ return myAlloc.allocate(size); }
		static void  operator delete(void* pdead, size_t size)
		{
			return myAlloc.deallocate(pdead, size);
		}
	};
	allocator Foo::myAlloc;


	class Goo {
	public:
		complex<double> c;
		string str;
		static allocator myAlloc;
	public:
		Goo(const complex<double>& x) : c(x) {  }
		static void* operator new(size_t size)
		{ return myAlloc.allocate(size); }
		static void  operator delete(void* pdead, size_t size)
		{
			return myAlloc.deallocate(pdead, size);
		}
	};
	allocator Goo::myAlloc;

	//-------------	
	void test_static_allocator_3()
	{
		cout << "\n\n\ntest_static_allocator().......... \n";

		{
			Foo* p[100];

			cout << "sizeof(Foo)= " << sizeof(Foo) << endl;
			for (int i = 0; i<23; ++i) {	//23,���┵, �S�⿴���Y�� 
				p[i] = new Foo(i);
				cout << p[i] << ' ' << p[i]->L << endl;
			}
			//Foo::myAlloc.check();

			for (int i = 0; i<23; ++i) {
				delete p[i];
			}
			//Foo::myAlloc.check();
		}

		{
			Goo* p[100];

			cout << "sizeof(Goo)= " << sizeof(Goo) << endl;
			for (int i = 0; i<17; ++i) {	//17,���┵, �S�⿴���Y�� 
				p[i] = new Goo(complex<double>(i, i));
				cout << p[i] << ' ' << p[i]->c << endl;
			}
			//Goo::myAlloc.check();

			for (int i = 0; i<17; ++i) {
				delete p[i];
			}
			//Goo::myAlloc.check();	
		}
	}
} //namespace	

int main(void)
{
	jj09::test_static_allocator_3();
	return 0;
}