#pragma once
#include "SimpleAllocate.h"

namespace TinySTL
{
	template<class T>
	struct BSTree_Node
	{
		typedef T value_type;
		value_type key;
		BSTree_Node *_left;
		BSTree_Node *_right;
		BSTree_Node * _parent;
		BSTree_Node()
		{
			_left = nullptr;
			_right = nullptr;
			_parent = nullptr;
		}
	};

	template<class T>
	bool insertBST(BSTree_Node<T> *&root, BSTree_Node<T> *parent, T value);

	template<class T>
	T findRightMin(BSTree_Node<T> * parent, BSTree_Node<T> * node, bool isRight);

	template<class T>
	bool deleteBSTNode(BSTree_Node<T> *&root, T value);

	template<class T>
	void MidTraverse(BSTree_Node<T> *root);

	template<class T>
	BSTree_Node<T>* findNode(BSTree_Node<T> *root, T value);

	template<class T>
	BSTree_Node<T>* findNext(BSTree_Node<T>*root);

	template<class T>
	BSTree_Node<T>* findPre(BSTree_Node<T>*root);


	template<class T>
	struct BSTree_Node_Iterator
	{
		typedef T value_type;
		typedef BSTree_Node<T> * node_ptr;
		typedef BSTree_Node<T> node_type;
		BSTree_Node_Iterator<T> operator++(int);		// ����
		BSTree_Node_Iterator<T> operator++();
		BSTree_Node_Iterator<T> operator--(int);
		BSTree_Node_Iterator<T> operator--();
		BSTree_Node_Iterator(node_ptr it) :ptr(it) {}
		node_ptr ptr;
		bool operator ==(const BSTree_Node_Iterator<T> & a)
		{
			return ptr == a.ptr;
		}
		bool operator !=(const BSTree_Node_Iterator<T> & a)
		{
			return ptr != a.ptr;
		}
		value_type operator*()
		{
			return ptr->key;
		}
	};


	template<class T>
	BSTree_Node_Iterator<T> BSTree_Node_Iterator<T>::operator++(int)
	{
		auto res = ptr;
		ptr = findNext(ptr);
		return BSTree_Node_Iterator<T>(res);
	}

	template<class T>
	BSTree_Node_Iterator<T> BSTree_Node_Iterator<T>::operator++()
	{
		ptr = findNext(ptr);
		return *this;
	}

	template<class T>
	BSTree_Node_Iterator<T> BSTree_Node_Iterator<T>::operator--(int)
	{
		auto res = ptr;
		ptr = findPre(ptr);
		return BSTree_Node_Iterator<T>(res);
	}

	template<class T>
	BSTree_Node_Iterator<T> BSTree_Node_Iterator<T>::operator--()
	{
		ptr = findPre(ptr);
		return *this;
	}

	template<class T>
	bool insertBST(BSTree_Node<T> *&root, BSTree_Node<T> *parent, T value)
	{
		typedef SimpleAllocate<BSTree_Node<T>> alloc;
		if (root == nullptr)
		{
			root = alloc::allocate(1);
			alloc::construct(root, BSTree_Node<T>());
			root->key = value;
			root->_parent = parent;
			return true;
		}
		if (root->key < value)
			return insertBST(root->_right, root, value);
		if (root->key > value)
			return insertBST(root->_left, root, value);
		return false;
	}

	template<class T>
	T findRightMin(BSTree_Node<T> * parent, BSTree_Node<T> * node, bool isRight)
	{
		typedef SimpleAllocate<BSTree_Node<T>> alloc;
		if (node->_left == nullptr)
		{
			T value = node->key;			// ���ĸ��׽ڵ���ӽڵ�ָ��
			if (node->_right == nullptr)
			{
				if (isRight)
					parent->_right = nullptr;
				else
					parent->_left = nullptr;
				delete node;
			}
			else
			{
				if (isRight)
				{
					parent->_right = node->_right;
					node->_right->_parent = parent;
				}
				else
				{
					parent->_left = node->_right;
					node->_right->_parent = parent;
				}
				alloc::destroy(node);
				alloc::deallocate(node, 1);
			}
			return value;
		}
		return findRightMin(node, node->_left, false);
	}

	template<class T>
	bool deleteBSTNode(BSTree_Node<T> *&root, T value)
	{
		typedef SimpleAllocate<BSTree_Node<T>> alloc;
		if (root == nullptr)
			return false;
		// �ҵ���ǰ�ڵ�
		if (root->key == value)
		{
			if (root->_left == nullptr)
			{
				auto tmp_root = root;
				root = root->_right;
				alloc::destroy(tmp_root);
				alloc::deallocate(tmp_root, 1);
				return true;
			}
			if (root->_right == nullptr)
			{
				auto tmp_root = root;
				root = root->_left;
				alloc::destroy(tmp_root);
				alloc::deallocate(tmp_root, 1);
				return true;
			}
			root->key = findRightMin(root, root->_right, true);		// ���ӽڵ����Сֵ
			return true;
		}
		else
		{
			if (root->key < value)
				return deleteBSTNode(root->_right, value);
			else
				return deleteBSTNode(root->_left, value);
		}
	}

	template<class T>
	void MidTraverse(BSTree_Node<T> *root)
	{
		if (root == nullptr)
			return;
		MidTraverse(root->_left);
		cout << root->key << " ";
		MidTraverse(root->_right);
	}

	template<class T>
	BSTree_Node<T> *findNode(BSTree_Node<T> *root, T value)
	{
		if (root == nullptr)
			return nullptr;
		if (value == root->key)
			return root;
		if (value < root->key)
			return findNode(root->_left, value);
		else
			return findNode(root->_right, value);
	}

	template<class T>
	BSTree_Node<T>* findNext(BSTree_Node<T>*root)
	{
		if (root == nullptr)
			return root;
		if (root->_right != nullptr)
		{
			// �ҵ�����ڵ�
			root = root->_right;
			while (root->_left)
				root = root->_left;
			return root;
		}
		else
		{
			// �ҵ��Ǹ��ڵ�����ӽڵ�ĵ�һ��
			while (root->_parent && root->_parent->_right == root)
				root = root->_parent;
			return root->_parent;
		}
	}

	template<class T>
	BSTree_Node<T>* findPre(BSTree_Node<T>*root)
	{
		if (root == nullptr)
			return root;
		if (root->_left != nullptr)
		{
			// �ҵ����ҽڵ�
			root = root->_left;
			while (root->_right)
				root = root->_right;
			return root;
		}
		else
		{
			while (root->_parent && root->_parent->_left == root)
				root = root->_parent;
			return root->_parent;
		}
	}
}