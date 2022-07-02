#ifndef  __MYLIST_ITER_H__
#define  __MYLIST_ITER_H__

#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>

#include "3mylist.h"


template <typename Item>    // item�����ǵ���������������˫���б�ڵ�
class ListIter
{
public :
    ///
    /// ���캯��
    ///
    ListIter(Item *p = NULL)
    :m_ptr(p)
    {
        /// NOP...
    }

    /// ����ʵ��copy ctor, ��Ϊ�������ṩȱʡ����Ϊ���㹻
    /// ����ʵ��operator=�� ��Ϊ�������ṩ��ȱʡ��Ϊ���㹻

    ///
    ///  ����ָ�������
    ///

    Item& operator*( ) const
    {
        return *m_ptr;
    }

    Item* operator->( ) const
    {
        return m_ptr;
    }

    ///
    ///  ��������operator++��ѭ��׼����,
    ///

    //(1) pre-increment operator...
    //  it meens ++i == iter.operator++( )
    Item& operator++( )
    {
        m_ptr = m_ptr->m_next;

        return (*this);
    }

    //(2) post-increment operator...
    //  it meens i++ == iter.operator++(int)
    Item& operator++(int)
    {
        Item temp = *this;
        ++*this;                //  ����ǰ��++�����

        return temp;
    }

    bool operator==(const Item &i) const
    {
        return m_ptr == i.m_ptr;
    }

    bool operator!=(const Item &i) const
    {
        return m_ptr != i.m_ptr;
    }

protected :
    Item *m_ptr;                //  ����������֮�����ϵ
};



#endif  // #define  __MYLIST_ITER_H__
