#include <iostream>
#include <new>		//placement new
using namespace std;

namespace jj03
{

	class A
	{
	public:
		int id;

		A() : id(0)      { cout << "default ctor. this=" << this << " id=" << id << endl; }
		A(int i) : id(i) { cout << "ctor. this=" << this << " id=" << id << endl; }
		~A()             { cout << "dtor. this=" << this << " id=" << id << endl; }
	};

	void test_array_new_and_placement_new()
	{
		cout << "\ntest_placement_new().......... \n";

		size_t size = 3;

		{
			//case 1
			//ģ�M memory pool ������, array new + placement new. ���� 

			A* buf = (A*)(new char[sizeof(A)*size]);
			A* tmp = buf;

			cout << "buf=" << buf << "  tmp=" << tmp << endl;

			for (int i = 0; i < size; ++i)
				new (tmp++) A(i);  			//3�� ����ctor 

			cout << "buf=" << buf << "  tmp=" << tmp << endl;

			//!	delete [] buf;    	//crash. why?
			//����@�䌍�ǂ� char array������ delete [] buf; ���g������D������ A::~A. 
			// �� array memory layout ���Ҳ����c array Ԫ�؂��� (���� 3) ���P����Ϣ, 
			// -- ������ֶ��e�y (���Ҍ� VC ���J�R����)����Ǳ����� 
			delete buf;     	//dtor just one time, ~[0]	

			cout << "\n\n";
		}

		{
			//case 2
			//���^�yԇ�μ��� array new

			A* buf = new A[size];  //default ctor 3 ��. [0]���[1]���[2])
			//A����� default ctor, ��t [Error] no matching function for call to 'jj02::A::A()'
			A* tmp = buf;

			cout << "buf=" << buf << "  tmp=" << tmp << endl;

			for (int i = 0; i < size; ++i)
				new (tmp++) A(i);  		//3�� ctor 

			cout << "buf=" << buf << "  tmp=" << tmp << endl;

			delete[] buf;    //dtor three times (�����淴, [2]���[1]���[0])	
		}

		{
			//case 3	
			//���ձ���ԭ��, �ٴ�ģ�M memory pool����, array new + placement new. 	
			//��, ������, ��� memory pool ֻ�ǹ��� memory, ���K���� construction, 
			//Ҳ���� destruction. ��ֻؓ؟���� memory. 
			//���������� void* �� char* ȡ�� memory, ጷ� (�h��)��Ҳ�� void* or char*.  
			//������ case 1 ጷ� (�h��) ���� A*. 
			//
			//���� memory pool ��ʽ�� jj04::test 
		}

	}
} //namespace

int main(void)
{
	jj03::test_array_new_and_placement_new();
	return 0;
}