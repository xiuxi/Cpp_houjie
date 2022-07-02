#include <cstddef>
#include <iostream>
using namespace std;

namespace jj05
{
	//ref. Effective C++ 2e, item10
	//per-class allocator 

	class Airplane {   //֧Ԯ customized memory management
	private:
		struct AirplaneRep {
			unsigned long miles;
			char type;
		};
	private:
		union {
			AirplaneRep rep;  //��ᘌ� used object
			Airplane* next;   //��ᘌ� free list
		};
	public:
		unsigned long getMiles() { return rep.miles; }
		char getType() { return rep.type; }
		void set(unsigned long m, char t)
		{
			rep.miles = m;
			rep.type = t;
		}
	public:
		static void* operator new(size_t size);
		static void  operator delete(void* deadObject, size_t size);
	private:
		static const int BLOCK_SIZE;
		static Airplane* headOfFreeList;
	};

	Airplane* Airplane::headOfFreeList;
	const int Airplane::BLOCK_SIZE = 512;

	void* Airplane::operator new(size_t size)
	{
		//�����С�e�`���D���o ::operator new()
		if (size != sizeof(Airplane))
		return ::operator new(size);

		Airplane* p = headOfFreeList;

		//��� p ��Ч���Ͱ�list�^��������һ��Ԫ��
		if (p)
			headOfFreeList = p->next;
		else {
			//free list �ѿա�����һ�K���ӛ���w��
			//������ݼ{ BLOCK_SIZE �� Airplanes
			Airplane* newBlock = static_cast<Airplane*>
				(::operator new(BLOCK_SIZE * sizeof(Airplane)));
			//�M��һ���µ� free list����С�^�K����һ�𣬵����^ 
			//#0 Ԫ�أ����Ҫ�������ؽo�����ߡ�
			for (int i = 1; i < BLOCK_SIZE - 1; ++i)
				newBlock[i].next = &newBlock[i + 1];
			newBlock[BLOCK_SIZE - 1].next = 0; //��null�Y��

			// �� p �O���^������ headOfFreeList �O��
			// ��һ���ɱ��\�õ�С�^�K��
			p = newBlock;
			headOfFreeList = &newBlock[1];
		}
		return p;
	}

	// operator delete �ӫ@һ�Kӛ���w��
	// ������Ĵ�С���_���Ͱ����ӵ� free list ��ǰ��
	void Airplane::operator delete(void* deadObject,
		size_t size)
	{
		if (deadObject == 0) return;
		if (size != sizeof(Airplane)) {
			::operator delete(deadObject);
			return;
		}

		Airplane *carcass =
			static_cast<Airplane*>(deadObject);

		carcass->next = headOfFreeList;
		headOfFreeList = carcass;
	}

	//-------------
	void test_per_class_allocator_2()
	{
		cout << "\ntest_per_class_allocator_2().......... \n";

		cout << sizeof(Airplane) << endl;    //8

		size_t const N = 100;
		Airplane* p[N];

		for (int i = 0; i< N; ++i)
			p[i] = new Airplane;


		//�S�C�yԇ object ������ 
		p[1]->set(1000, 'A');
		p[5]->set(2000, 'B');
		p[9]->set(500000, 'C');
		cout << p[1] << ' ' << p[1]->getType() << ' ' << p[1]->getMiles() << endl;
		cout << p[5] << ' ' << p[5]->getType() << ' ' << p[5]->getMiles() << endl;
		cout << p[9] << ' ' << p[9]->getType() << ' ' << p[9]->getMiles() << endl;

		//ݔ��ǰ 10 �� pointers, ���Ա��^���g�� 
		for (int i = 0; i< 10; ++i)
			cout << p[i] << endl;

		for (int i = 0; i< N; ++i)
			delete p[i];
	}
} //namespace

int main(void)
{
	jj05::test_per_class_allocator_2();
	return 0;
}