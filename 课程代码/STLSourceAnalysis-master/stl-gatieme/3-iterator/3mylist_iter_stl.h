#ifndef  __MYLIST_ITER_H__
#define  __MYLIST_ITER_H__

#include <iostream>
#include <vector>                                                                           _
#include <deque>
#include <list>
#include <algorithm>

#include "3mylist.h"


template <typename Item>    // item�����ǵ���������������˫���б�ڵ�
class ListIter  
: public std::iterator<std::forward_iterator_tag, Item>
{
	
};



#endif  // #define  __MYLIST_ITER_H__
