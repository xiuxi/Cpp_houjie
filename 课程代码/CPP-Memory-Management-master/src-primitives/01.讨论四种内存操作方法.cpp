#include <iostream>
#include <complex>
#include <memory>				 //std::allocator  
//#include <ext\pool_allocator.h>	 //GCCʹ�ã���ʹ�� std::allocator ����� allocator, �͵����� #include <ext/...> 
using namespace std;
namespace jj01
{
	void test_primitives()
	{
		cout << "\ntest_primitives().......... \n";

		void* p1 = malloc(512);	//512 bytes
		free(p1);

		complex<int>* p2 = new complex<int>; //one object
		delete p2;

		void* p3 = ::operator new(512); //512 bytes
		::operator delete(p3);

		//����ʹ�� C++ �˜ʎ��ṩ�� allocators��
		//��ӿ��m�И˜�Ҏ�񣬵����F�S�́Kδ��ȫ���أ�����������ʽ�Ԯ���
#ifdef _MSC_VER
		//���ɺ������� non-static����Ҫͨ�^ object �{�á����·��� 3 �� ints.
		int* p4 = allocator<int>().allocate(3, (int*)0);
		p4[0] = 666;
		p4[1] = 999;
		p4[2] = 888;
		cout << "p4[0] = " << p4[0] << endl;
		cout << "p4[1] = " << p4[1] << endl;
		cout << "p4[2] = " << p4[2] << endl;
		allocator<int>().deallocate(p4, 3);
#endif
#ifdef __BORLANDC__
		//���ɺ������� non-static����Ҫͨ�^ object �{�á����·��� 5 �� ints.
		int* p4 = allocator<int>().allocate(5);
		allocator<int>().deallocate(p4, 5);
#endif
#ifdef __GNUC__
		//���ɺ������� static����ͨ�^ȫ���{��֮�����·��� 512 bytes.
		//void* p4 = alloc::allocate(512); 
		//alloc::deallocate(p4,512);   

		//���ɺ������� non-static����Ҫͨ�^ object �{�á����·��� 7 �� ints.    
		void* p4 = allocator<int>().allocate(7);
		allocator<int>().deallocate((int*)p4, 7);

		//���ɺ������� non-static����Ҫͨ�^ object �{�á����·��� 9 �� ints.	
		void* p5 = __gnu_cxx::__pool_alloc<int>().allocate(9);
		__gnu_cxx::__pool_alloc<int>().deallocate((int*)p5, 9);
#endif
	}
} //namespace

int main(void)
{
	jj01::test_primitives();
	return 0;
}