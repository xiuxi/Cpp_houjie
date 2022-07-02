#include <iostream>
#include <string>


#include <typeinfo>
using namespace std;

#if defined(__SGI_STL_USE_AutoPtr_CONVERSIONS) && \
    defined(__STL_MEMBER_TEMPLATES)

template<class U>
struct AutoPtrRef
{
  U* m_pointee;

  AutoPtrRef(U* p)
  :m_pointee(p)
  {

  }

};

#endif /* auto ptr conversions && member templates */

template<class T>
class AutoPtr
{
public :
    /// constructor
    explicit AutoPtr(T *p = NULL)
    :m_pointee(p)
    {
        /// NOP
    }

    /// copy constructor
    template<class U>
    AutoPtr(AutoPtr<U> &rhs)    //  ����Ҫ�ͷ�rhsָ���ָ�� ��˲�������Ϊconst
    :m_pointee(rhs.release())
    {
        /// NOP
    }

    ///
    ~AutoPtr()
    {
        delete this->m_pointee;
    }

    ///
    ///  �������� ��ȡget, release�ͷ�, ����reset
    ///

    ///  ��ȡָ���ָ��
    T* get( ) const
    {
        return m_pointee;
    }

    ///  �ͷ�ָ���ָ��
    T* release( )
    {
        T *temp = this->m_pointee;  //  ����ԭָ���ַ
        this->m_pointee = NULL;     //  ��ָ��ָ��

        ///     �ڴ���ͷ��������������
        return temp;
    }

    /// ����ָ���ָ��
    void reset(T *p)
    {
        if(m_pointee != p)
        {
            delete m_pointee;       //  �ͷ�ԭ���Ŀռ�
            this->m_pointee = p;    //  �޸�ָ��
        }
    }

    ///
    ///  ���ز�����
    ///

    /// *ptrȡ��ַ
    T& operator*( ) const
    {
        if(m_pointee == NULL)
        {
            std::cout <<"pointee is NULL..." <<std::endl;
        }
        return *m_pointee;
//        try
//        {
//            typeid(*m_pointee).name();
//
//            return *m_pointee;
//        }
//        catch(std::bad_typeid)
//        {
//           std::cout <<__LINE__ <<std::endl;
//           cout << "Object is NULL" << endl;
//        }
    }

    ///  ָ��ָ��->
    T*  operator->( ) const
    {
        return m_pointee;
    }

    template<class U>
    AutoPtr& operator=(AutoPtr<U> &ptr)
    {
        ///  auto_pstr��ռ�ԵĹؼ�
        if(this->m_pointee != ptr.m_pointee)
        {
            //  ����Ҫ�޸���ֵ��ָ��
            delete this->m_pointee;

            this->m_pointee = ptr.m_pointee;
            ptr.release( );                             //  �ͷ���ֵָ���ָ��

        }
        return *this;
    }

    ///
    ///  ���ӵ����ýӿ�
    ///
#if defined(__SGI_STL_USE_AutoPtr_CONVERSIONS) && \
    defined(__STL_MEMBER_TEMPLATES)
    /// ���캯��
    AutoPtr(AutoPtrRef<T> ref)
    : m_pointee(ref.m_pointee)
    {

    }

    //  �����
    AutoPtr& operator=(AutoPtrRef<T> ref)
    {
        if (ref.m_pointee != this->get()) {
        delete m_pointee;
      m_pointee = ref.m_pointee;
    }
    return *this;
  }

    template <class U>
    operator AutoPtrRef<U>( )
    {
        return AutoPtrRef<U>(this->release());
    }

    template <class U>
    operator AutoPtr<U>( )
    {
        return AutoPtr<U>(this->release());
    }

#endif /* auto ptr conversions && member templates */

private :
    T   *m_pointee;
};


int main(void)
{

    AutoPtr<string> pstr1(new string("jeancheng"));

    AutoPtr<string> pstr2(new string("gatieme"));


    std::cout <<*pstr1 <<std::endl;
    std::cout <<*pstr2 <<std::endl;

    pstr1 = pstr2;

    std::cout <<*pstr1 <<std::endl;
    std::cout <<*pstr2 <<std::endl;           //  segment fault  -=> becase


    return 0;


}
