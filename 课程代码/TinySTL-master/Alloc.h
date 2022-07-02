#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <cstdlib>
#include <cstdio>

namespace TinySTL{

/*
1. �����ֽ�Ϊ��λ����ռ䣬���տռ�
2. ά��һ���ڴ�غ�һ������
3. ����ռ䳬��һ����С����ϵͳ����ռ�
4. ��С���������ϲ鿴�Ƿ��пռ�
5. ���û�У������ڴ������ռ䣬һ�������飬�������������
6. ����ڴ�ؿռ䲻�����룬�����ٿ�������һ�飬��ȫ�����أ���������Ĺ���������
7. ����ڴ��һ�鶼���벻�ˣ����ʣ���ڴ���������ϱ��λ�ã�Ȼ����ϵͳ����ռ�
8. ���ϵͳ����ɹ���ֱ�ӷ���
9. �������ʧ�ܣ������������������û�п��ÿռ䣬����Щ�ռ���Ϊ�ڴ��
*/
	class alloc{
	private:
		enum EAlign{ ALIGN = 8};		// С��������ϵ��߽�
		enum EMaxBytes{ MAXBYTES = 128};// С����������ޣ�������������malloc����
		enum ENFreeLists{ NFREELISTS = (EMaxBytes::MAXBYTES / EAlign::ALIGN)};
		enum ENObjs{ NOBJS = 20};		// ÿ�����ӵĽڵ���
	private:
		// ����ڵ�
		union obj
		{
			union obj *next;
			char client[1];
		};

		static obj *free_list[ENFreeLists::NFREELISTS];
	private:
		static char *start_free;	// �ڴ����ʼλ��
		static char *end_free;		// �ڴ�ؽ���λ��
		static size_t heap_size;	// ��������
	private:
		//��bytes�ϵ���8�ı���
		static size_t ROUND_UP(size_t bytes)
		{
			return ((bytes + EAlign::ALIGN - 1) & ~(EAlign::ALIGN - 1));
		}
		//���������С������ʹ�õ�n��free-list��n��0��ʼ����
		static size_t FREELIST_INDEX(size_t bytes)
		{
			return (((bytes)+EAlign::ALIGN - 1) / EAlign::ALIGN - 1);
		}
		//����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free-list
		static void *refill(size_t n);
		//����һ���ռ䣬������nobjs����СΪsize������
		//�������nobjs�������������㣬nobjs���ܻή��
		static char *chunk_alloc(size_t size, size_t& nobjs);

	public:
		static void *allocate(size_t bytes);
		static void deallocate(void *ptr, size_t bytes);
		static void *reallocate(void *ptr, size_t old_sz, size_t new_sz);
	};
}

#endif