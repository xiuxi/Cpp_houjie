#ifdef MYSTL_CONSTRUCT_H_
#define MYSTL_CONSTRUCT_H_

//���ͷ�ļ������������� construct��destroy
//construct���������Ĺ���	destroy��������������

#include<new>	//����placement new��ͷ�ļ�

#include "type_traits.h"		//��ȡ��
#include "iterator.h"			//������

//�����ǹرձ���ͷ�ļ�ʱ�����ľ���
#ifdef _MSC_VER					//MSCV��΢���VC���п⣬��������
#pragma warning(push)		   //#pragmaָ����Զ�ָ��������Ĵ���ر�ָ����warning
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER

namespace mystl {	//�����ռ�mystl

//construct�������
template<class T>void construct(T* prt) {
	::new((void*)ptr) T();		//placement new,����T::T()����Ĭ�Ϲ���
}

template<class T1, class T2>void construct(T1* ptr,const T2& value){
	::new((void*)ptr) T1(value);	//placement new,����T::T(value)����ֵ�趨��ָ����ָ�Ŀռ���
}

template<class T, class...Args>void construct(T* ptr, Args&&...args) {	//�ɱ����ģ���趨��ֵ����
	::new((void*)ptr) T(mystl::forward<Args>(args)...);					//forward������֤����������ת��
}

//destroy����������
//����Ҫ�ж�Ԫ�ص���ֵ�ͱ�(value type)��������_type_traits<T>�жϸ��ͱ��Ƿ���trivial destructor(�޹�ʹ������������)
//����(_true_type_)����ʲôҲ�����ͽ���
//����(_false_type_)����ѭ����ʽѲ��������Χ������ѭ����ÿ����һ������͵��ý���ָ��汾����������

//����ָ��汾
//���Ԫ�ص���ֵ�ͱ�(value type)��trivial destructor
template <class T>void destroy_one(T*, std::true_type) {}		

//���Ԫ�ص���ֵ�ͱ�(value type)��non-trivial destructor
template <class T>void destroy_one(T* pointer, std::false_type){
	if (pointer != nullptr)
	{
		pointer->~T();
	}
}

//���ܵ������汾
//���Ԫ�ص���ֵ�ͱ�(value type)��trivial destructor
template <class ForwardIter>void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

//���Ԫ�ص���ֵ�ͱ�(value type)��non-trivial destructor
template <class ForwardIter>void destroy_cat(ForwardIter first, ForwardIter last, std::false_type){
	for (; first != last; ++first)
		destroy(&*first);
}

//destroy()�ĵ�һ�汾������һ��ָ��.�˺����跨�ҳ�Ԫ�ص���ֵ�ͱ𣬽�������_type_traits<>��ȡ���ʵ���ʩ
template<class T>void destroy(T* pointer) {			
	destroy_one(pointer, std::is_trivially_destructible<T>{});
}
//������destroy�ĵڶ��汾�������������������˺����跨�ҳ�Ԫ�ص���ֵ�ͱ𣬽�������_type_traits<>��ȡ���ʵ���ʩ
template<class ForwardIter>void destroy(ForwordIter first, ForwordIter last) {	
	destory_cat(first, last, std::is_trivially_destructible <typename iterator_traits<ForwardIter>::value_type>{});
}

}	//���������ռ�mystl

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // MYSTL_CONSTRUCT_H_

