#pragma once
#include "BSTree_Node.h"

namespace TinySTL
{
	template<class T>
	class BSTree
	{
	public:
		typedef T value_type;
		typedef BSTree_Node_Iterator<T> iterator;
		typedef typename iterator::node_type node_type;
		typedef typename iterator::node_type* node_ptr;
		typedef size_t size_type;
		typedef SimpleAllocate<node_type> alloc;
	protected:
		node_ptr head;
		size_type size_;
		node_ptr &BSTree<T>::root()
		{
			return head->_right;
		}
	public:
		// head��parant�洢���Ǹ��ڵ㣬left�洢����begin�ڵ㣬end�ڵ�Ϊ�ս��
		BSTree()
		{
			head = alloc::allocate(1);
			alloc::construct(head, node_type());			
			size_ = 0;
		}
		size_type size()
		{
			return size_;
		}
		bool empty()
		{
			return size() == 0;
		}
		iterator begin()
		{
			return iterator(head->_left);
		}
		iterator end()
		{
			return iterator(nullptr);
		}
		bool insert(value_type value)
		{
			bool f = insertBST<T>(root(), head, value);
			auto r = root();
			while (r != nullptr && r->_left != nullptr)
				r = r->_left;
			head->_left = r;		// ����begin()
			++size_;
			return f;
		}
		bool erase(value_type value)
		{
			bool f = deleteBSTNode<T>(root(), value);
			auto r = root();
			if (r == nullptr)
				head->_left = nullptr;
			while (r != nullptr && r->_left != nullptr)
				r = r->_left;
			head->_left = r;
			--size_;
			return f;
		}
		iterator find(value_type value)
		{
			auto res = findNode(root(), value);
			if (res == nullptr)
				return iterator(end());
			return iterator(res);
		}
	};
}
