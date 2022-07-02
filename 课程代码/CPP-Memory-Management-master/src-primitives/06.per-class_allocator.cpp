#include <cstddef>
#include <iostream>
using namespace std;

namespace jj04
{
	//ref. C++Primer 3/e, p.765
	//per-class allocator 

	class Screen {
	public:
		Screen(int x) : i(x) { };
		int get() { return i; }

		void* operator new(size_t);
		void  operator delete(void*, size_t);	//(2)
		//! void  operator delete(void*);			//(1) ����һ. ��(1)(2)�K��,���к���ֵĈ��e (�������^�w) 

	private:
		Screen* next;
		static Screen* freeStore;
		static const int screenChunk;
	private:
		int i;
	};
	Screen* Screen::freeStore = 0;
	const int Screen::screenChunk = 24;

	void* Screen::operator new(size_t size)
	{
		Screen *p;
		if (!freeStore) {
			//linked list �ǿյģ����Ծ�ȡһ��K memory
			//���º��е��� global operator new
			size_t chunk = screenChunk * size;
			freeStore = p =
				reinterpret_cast<Screen*>(new char[chunk]);
			//������Á��һ��K memory ���� linked list ��С�KС�K��������
			for (; p != &freeStore[screenChunk - 1]; ++p)
				p->next = p + 1;
			p->next = 0;
		}
		p = freeStore;
		freeStore = freeStore->next;
		return p;
	}


	//! void Screen::operator delete(void *p)		//(1)
	void Screen::operator delete(void *p, size_t)	//(2)����һ 
	{
		//�� deleted object �ջز��� free list ǰ��
		(static_cast<Screen*>(p))->next = freeStore;
		freeStore = static_cast<Screen*>(p);
	}

	//-------------
	void test_per_class_allocator_1()
	{
		cout << "\ntest_per_class_allocator_1().......... \n";

		cout << sizeof(Screen) << endl;		//8	

		size_t const N = 100;
		Screen* p[N];

		for (int i = 0; i< N; ++i)
			p[i] = new Screen(i);

		//ݔ��ǰ 10 �� pointers, ���Ա��^���g�� 
		for (int i = 0; i< 10; ++i)
			cout << p[i] << endl;

		for (int i = 0; i< N; ++i)
			delete p[i];
	}
} //namespace

int main(void)
{
	jj04::test_per_class_allocator_1();
	return 0;
}