#ifndef MYSTL_RB_TREE_H_
#define MYSTL_RB_TREE_H_

// ���ͷ�ļ�����һ��ģ���� rb_tree
// rb_tree : �����

#include <initializer_list>		//��ʼ���б�

#include <cassert>				//����

#include "functional.h"			//�º���
#include "iterator.h"			//������
#include "memory.h"				//�߼��Ķ�̬�ڴ����
#include "type_traits.h"		//��ȡ��
#include "exceptdef.h"			//�쳣

namespace mystl {

// rb tree �ڵ���ɫ������
typedef bool rb_tree_color_type;
//constexpx����ָ���������ѷ���ֵ��������������ʹ�ã�����ֻ��һ��������Ҳ�п��ܷ���Ϊ�����ڳ���
static constexpr rb_tree_color_type rb_tree_red = false;	//��ɫΪ0
static constexpr rb_tree_color_type rb_tree_black = true;	//��ɫΪ1

// forward declarationǰ������
template <class T> struct rb_tree_node_base;	//�ڵ�ṹ�Ļ��࣬���������ӽڵ��븸�ڵ�ָ��
template <class T> struct rb_tree_node;			//�ڵ�ṹ�����࣬��������ֵ

template <class T> struct rb_tree_iterator;		//��������ƣ��������ռ���Ҫ���嶨��������ʾ������
template <class T> struct rb_tree_const_iterator;

// rb tree value traits��ȡֵ�ͱ����
template <class T, bool>struct rb_tree_value_traits_imp {
	typedef T key_type;			//������keyֵ
	typedef T mapped_type;		//������valueֵ
	typedef T value_type;		//������pairֵ����pair<const T,T>����ʾ����ֵ���ͱ�

	//����key��value��ͬ�����ݽṹ
	template <class Ty>static const key_type& get_key(const Ty& value) { return value; }
	template <class Ty>static const value_type& get_value(const Ty& value) { return value; }
};

template <class T> struct rb_tree_value_traits_imp<T, true> {
	//remove_cv�ṩ��type��ͬ�ĳ�Ա typedef T��ֻ��ɾ���������� cv �޶���,��c��const����v��volatile���޶���
	//�ο�����:https://www.cnblogs.com/5iedu/p/7767937.html
			   https://blog.csdn.net/janeqi1987/article/details/100049597
	typedef typename std::remove_cv<typename T::first_type>::type key_type;			//pair��first����keyֵ
	typedef typename T::second_type                               mapped_type;		//pair��second����mappedֵ
	typedef T                                                     value_type;		//pair���ͣ�����first��second

	//����pair����
	template <class Ty>static const key_type& get_key(const T& value) {
		return value.first;
	}
	template <class Ty>static const value_type& get_value(const Ty& value){
		return value;
	}
};

template <class T>struct rb_tree_value_traits {
	static constexpr bool is_map = mystl::is_pair<T>::value;	//�ж��Ƿ���pair
	typedef rb_tree_value_traits_imp<T, is_map> value_traits_type; //��������� rb_tree_value_traits

	typedef typename value_traits_type::key_type    key_type;
	typedef typename value_traits_type::mapped_type mapped_type;
	typedef typename value_traits_type::value_type  value_type;

	//����key��value��ͬ�����ݽṹ
	template <class Ty>
	static const key_type& get_key(const Ty& value)
	{
		return value_traits_type::get_key(value);
	}

	template <class Ty>
	static const value_type& get_value(const Ty& value)
	{
		return value_traits_type::get_value(value);
	}
};

// rb tree node traits�ڵ����ȡ
template <class T>struct rb_tree_node_traits {
	typedef rb_tree_color_type                 color_type;	//�ڵ���ɫ����

	typedef rb_tree_value_traits<T>				value_traits;
	typedef typename value_traits::key_type		key_type;
	typedef typename value_traits::mapped_type	mapped_type;
	typedef typename value_traits::value_type	value_type;

	//˫�ؽڵ�ṹ��ָ��
	typedef rb_tree_node_base<T>* base_ptr;		
	typedef rb_tree_node<T>*	  node_ptr;
};

template <class T>struct rb_tree_node_base {	//�ڵ�ṹ�Ļ��࣬���������ָ��
	typedef rb_tree_color_type color_type;		//��ɫ����
	typedef rb_tree_node_base<T>* base_ptr;		//����ָ��
	typedef rb_tree_node<T>* node_ptr;			//����ָ��

	base_ptr   parent;  // ���ڵ㣬���ڷ��㡰���ݡ����ڵ�Ĳ���
	base_ptr   left;    // ���ӽڵ�
	base_ptr   right;   // ���ӽڵ�
	color_type color;   // �ڵ���ɫ

	base_ptr get_base_ptr()
	{
		return &*this;		//*this��ʾthisָ��ָ��Ķ���&*this��ʾ*thisָ��Ķ����ָ��
	}

	node_ptr get_node_ptr()
	{
		//reinterpret_cast�������κ�ָ��ת��Ϊ�κ�����ָ�����͡� Ҳ�����κ���������ת��Ϊ�κ�ָ�������Լ�����ת��
		return reinterpret_cast<node_ptr>(&*this);	//��ȡ��Ӧָ��
	}

	node_ptr& get_node_ref()
	{
		return reinterpret_cast<node_ptr&>(*this);	//��ȡ��Ӧ����
	}
};

template <class T>struct rb_tree_node :public rb_tree_node_base<T> {	//�ڵ�ṹ������
	typedef rb_tree_node_base<T>* base_ptr;
	typedef rb_tree_node<T>* node_ptr;

	T value;  // �ڵ�ֵ

	base_ptr get_base_ptr()
	{
	//static_cast��expressionת��Ϊtype-id���ͣ���Ҫ���ڷǶ�̬����֮���ת�������ṩ����ʱ�ļ����ȷ��ת���İ�ȫ��
		return static_cast<base_ptr>(&*this);	//�������ָ�������ת���ɸ����ʾ
	}

	node_ptr get_node_ptr()
	{
		return &*this;	//this��ָ�룬*this��ʾָ��ָ��Ķ���&*this��thisָ��ָ��Ķ����ָ��
	}
};

// rb tree traits
template <class T>
struct rb_tree_traits
{
	typedef rb_tree_value_traits<T>            value_traits;

	//��Ӧ�ͱ��typedef
	typedef typename value_traits::key_type    key_type;
	typedef typename value_traits::mapped_type mapped_type;
	typedef typename value_traits::value_type  value_type;

	typedef value_type*							pointer;
	typedef value_type&							reference;
	typedef const value_type*					const_pointer;
	typedef const value_type&					const_reference;

	typedef rb_tree_node_base<T>               base_type;
	typedef rb_tree_node<T>                    node_type;

	//ָ��typedef
	typedef base_type* base_ptr;
	typedef node_type* node_ptr;
};

// rb tree �ĵ��������

//���������
template <class T>struct rb_tree_iterator_base :public mystl::iterator<mystl::bidirectional_iterator_tag, T> {
	typedef typename rb_tree_traits<T>::base_ptr base_ptr;
	base_ptr node;	// ָ��ڵ㱾��,����������֮�����һ�������ϵ

	// ʹ������ǰ����Ѱ�Ҹýڵ�ĺ�̽ڵ㣬���պñȵ�ǰ�ڵ���Ԫ��
	void inc() {
		if (node->right != nullptr) {	//������ӽڵ����
			node = rb_tree_min(node->right);	//�ҵ�����������Сֵ���������������²������ڵ�
		}
		else {	 // ���û�����ӽڵ�
			auto y = node->parent;		//�ҳ����ڵ�
			while (y->right == node) {	//������нڵ㱾���Ǹ��ҽڵ�
				node = y;				//һֱ���ݣ�ֱ����Ϊ���ӽڵ�Ϊֹ
				y = y->parent;
			}
			// Ӧ�ԡ�Ѱ�Ҹ��ڵ����һ�ڵ㣬�����ڵ�û�����ӽڵ㡱���������
			if (node->right != y)		//����ʱ�����ӽڵ㲻���ڴ�ʱ�ĸ��ڵ�
				node = y;				//��ʱ�ĸ��ڵ㼴Ϊ���
										//�����ʱ��nodeΪ���
		}
	}

	// ʹ���������ˣ�Ѱ�Ҹýڵ��ǰ�ýڵ�
	void dec() {
		//����Ǻ�ڵ㣬�Ҹ��ڵ�ĸ��ڵ�����Լ�
		if (node->parent->parent == node && rb_tree_is_red(node)) {
			node = node->right;		//���ӽڵ㼴Ϊ���
		}
		//�������������nodeΪheaderʱ����nodeΪend()ʱ����ʱ���Ԫ�ظպ���node��ǰ���ڵ�
		// ע�⣺header�����ӽڵ�Ϊmost right��ָ����������max�ڵ�
		else if (node->left != nullptr) {		// ����������ӽڵ�
			node = rb_tree_max(node->left);		// �������ӽڵ��ϣ��ҳ������������ڵ㣬��������������
		}
		else {		 // ����ýڵ�ȷǸ��ڵ㣬Ҳ���������ӽڵ�
			auto y = node->parent;		 // �������ĸ��ڵ���
			while (node == y->left) {	 // ������������ĸ��ӽڵ�����ӽڵ�
				node = y;				 // ��һֱ���ϲ飬ֱ�����нڵ㲻Ϊ���ӽڵ�
				y = y->parent;
			}							
			node = y;					 // ��ʱ���ĸ��ڵ㼴ΪҪ�ҵ�ǰ���ڵ�
		}
	}

	//����������
	bool operator==(const rb_tree_iterator_base& rhs) { return node == rhs.node; }
	bool operator!=(const rb_tree_iterator_base& rhs) { return node != rhs.node; }
};

//���������
template <class T> struct rb_tree_iterator :public rb_tree_iterator_base<T> {
	typedef rb_tree_traits<T>                tree_traits;

	//��Ƕ�ͱ�����
	typedef typename tree_traits::value_type value_type;
	typedef typename tree_traits::pointer    pointer;
	typedef typename tree_traits::reference  reference;
	typedef typename tree_traits::base_ptr   base_ptr;
	typedef typename tree_traits::node_ptr   node_ptr;

	//����������
	typedef rb_tree_iterator<T>              iterator;
	typedef rb_tree_const_iterator<T>        const_iterator;
	typedef iterator                         self;

	using rb_tree_iterator_base<T>::node;	//using������������������������û���ĳ�Ա

	// ���캯��
	rb_tree_iterator() {}
	rb_tree_iterator(base_ptr x) { node = x; }
	rb_tree_iterator(node_ptr x) { node = x; }
	rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
	rb_tree_iterator(const const_iterator& rhs) { node = rhs.node; }

	// ���ز�����
	reference operator*()  const { return node->get_node_ptr()->value; }
	pointer   operator->() const { return &(operator*()); }

	self& operator++()		//ǰ��++
	{
		this->inc();		//����inc()����
		return *this;
	}
	self operator++(int)	//����++
	{
		self tmp(*this);
		this->inc();		//����inc()����
		return tmp;
	}
	self& operator--()		//ǰ��--
	{
		this->dec();		//����dnc()����
		return *this;
	}
	self operator--(int)	//����--
	{
		self tmp(*this);
		this->dec();		//����inc()����
		return tmp;
	}
};

//const�����������
template <class T>
struct rb_tree_const_iterator :public rb_tree_iterator_base<T>
{
	typedef rb_tree_traits<T>                     tree_traits;

	typedef typename tree_traits::value_type      value_type;
	typedef typename tree_traits::const_pointer   pointer;
	typedef typename tree_traits::const_reference reference;
	typedef typename tree_traits::base_ptr        base_ptr;
	typedef typename tree_traits::node_ptr        node_ptr;

	typedef rb_tree_iterator<T>                   iterator;
	typedef rb_tree_const_iterator<T>             const_iterator;
	typedef const_iterator                        self;

	using rb_tree_iterator_base<T>::node;

	// ���캯��
	rb_tree_const_iterator() {}
	rb_tree_const_iterator(base_ptr x) { node = x; }
	rb_tree_const_iterator(node_ptr x) { node = x; }
	rb_tree_const_iterator(const iterator& rhs) { node = rhs.node; }
	rb_tree_const_iterator(const const_iterator& rhs) { node = rhs.node; }

	// ���ز�����
	reference operator*()  const { return node->get_node_ptr()->value; }
	pointer   operator->() const { return &(operator*()); }

	self& operator++()
	{
		this->inc();
		return *this;
	}
	self operator++(int)
	{
		self tmp(*this);
		this->inc();
		return tmp;
	}
	self& operator--()
	{
		this->dec();
		return *this;
	}
	self operator--(int)
	{
		self tmp(*this);
		this->dec();
		return tmp;
	}
};

// tree algorithm ��������㷨
template <class NodePtr>NodePtr rb_tree_min(NodePtr x)noexcept {
	while (x->left != nullptr) {	//һֱ�����ߣ������ҵ����������Сֵ�ڵ�
		x = x->left;				//����������������
	}
	return x;
}

template <class NodePtr>NodePtr rb_tree_max(NodePtr x)noexcept {
	while (x->right != nullptr) {	//һֱ�����ߣ������ҵ�����������ֵ�ڵ�
		x = x->right;				 // ����������������
	}
	return x;
}

template <class NodePtr> bool rb_tree_is_lchild(NodePtr node) noexcept
{
	return node == node->parent->left;	//�ж��Ƿ������ӽڵ�
}

template <class NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept
{
	return node->color == rb_tree_red;	//�жϽڵ��Ƿ��Ǻ���ɫ
}

template <class NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept
{
	node->color = rb_tree_black;		//���õ�ǰ�ڵ�Ϊ��ɫ
}

template <class NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept
{
	node->color = rb_tree_red;			//���õ�ǰ�ڵ�Ϊ��ɫ
}

//Ѱ�ҵ�ǰ�ڵ����һ�ڵ�
template <class NodePtr>NodePtr rb_tree_next(NodePtr node)noexcept {
	if (node->right != nullptr) {			//��������Ϊ��
		return rb_tree_min(node->right);	//����������С�ڵ�
	}
	while (!rb_tree_is_lchild(node))		//���������ӽڵ�
		node = node->parent;				//�����Ҹ��ڵ�
	return node->parent;					//���ظ��ڵ�ĸ��ڵ�
}

/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate left      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/
// ����������һΪ�����㣬������Ϊ���ڵ�
template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept {
	//xΪ��ת��
	auto y = x->right;			// ��y Ϊ x �����ӽڵ�
	x->right = y->left;			//����ͼ����b����x����Ϊx�����ӽڵ�
	if (y->left != nullptr) {	//���y�����ӽڵ㲻Ϊ��
		y->left->parent = x;	//��y��Ϊx�ĸ��ڵ㣬����x�������ӽڵ��λ��
	}
	y->parent = x->parent;		//��x�ĸ��ڵ�Ϊy�ĸ��ڵ㣬����pΪy�ĸ��ڵ�

	if (x == root) {		// ��� x Ϊ���ڵ㣬�� y ���� x ��Ϊ���ڵ�
		root = y;
	}
	else if (rb_tree_is_lchild(x)) {	 // ��� x ���丸�ڵ�����ӽڵ�
		x->parent->left = y;			//��y��Ϊx�ĸ��ڵ��������������y��Ϊp�����ӽڵ�
	}
	else {								// ��� x ���丸�ڵ�����ӽڵ�
		x->parent->right = y;			//��y��Ϊx�ĸ��ڵ��������������y��Ϊp�����ӽڵ�
	}
	// ���� x �� y �Ĺ�ϵ
	y->left = x;						
	x->parent = y;
}

/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
// ����������һΪ�����㣬������Ϊ���ڵ�
template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept {
	auto y = x->left;				// ��y Ϊ x �����ӽڵ�
	x->left = y->right;				//����ͼ����c����x����Ϊx�����ӽڵ�
	if (y->right) {					//���y�����ӽڵ㲻Ϊ��
		y->right->parent = x;		//��y��Ϊx�ĸ��ڵ㣬����x�������ӽڵ��λ��
	}
	y->parent = x->parent;			//��x�ĸ��ڵ�Ϊy�ĸ��ڵ㣬����pΪy�ĸ��ڵ�

	if (x == root) {	// ��� x Ϊ���ڵ㣬�� y ���� x ��Ϊ���ڵ�
		root = y;
	}
	else if(rb_tree_is_lchild(x))	 // ��� x �����ӽڵ�
	{
		x->parent->left = x;		//��y��Ϊx�ĸ��ڵ��������������y��Ϊp�����ӽڵ�
	}
	else {							// ��� x �����ӽڵ�
		x->parent->right = y;		//��y��Ϊx�ĸ��ڵ��������������y��Ϊp�����ӽڵ�
	}
	// ���� x �� y �Ĺ�ϵ
	y->right = x;
	x->parent = y;
}

// ����ڵ��ʹ rb tree ����ƽ�⣬����һΪ�����ڵ㣬������Ϊ���ڵ�
//
// case 1: �����ڵ�λ�ڸ��ڵ㣬�������ڵ�Ϊ��
// case 2: �����ڵ�ĸ��ڵ�Ϊ�ڣ�û���ƻ�ƽ�⣬ֱ�ӷ���
// case 3: ���ڵ������ڵ㶼Ϊ�죬��ڵ������ڵ�Ϊ�ڣ��游�ڵ�Ϊ�죬
//         Ȼ�����游�ڵ�Ϊ��ǰ�ڵ㣬��������
// case 4: ���ڵ�Ϊ�죬����ڵ�Ϊ NIL ���ɫ�����ڵ�Ϊ���ң����ӣ���ǰ�ڵ�Ϊ�ң��󣩺��ӣ�
//         �ø��ڵ��Ϊ��ǰ�ڵ㣬���Ե�ǰ�ڵ�Ϊ֧�����ң���
// case 5: ���ڵ�Ϊ�죬����ڵ�Ϊ NIL ���ɫ�����ڵ�Ϊ���ң����ӣ���ǰ�ڵ�Ϊ���ң����ӣ�
//         �ø��ڵ��Ϊ��ɫ���游�ڵ��Ϊ��ɫ�����游�ڵ�Ϊ֧���ң�����
//
// �ο�����: http://blog.csdn.net/v_JULY_v/article/details/6105630		������Ҫ�ο���ƪ���������
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept {
	rb_tree_set_red(x);			// �����ڵ��Ϊ��ɫ
	while (x != root && rb_tree_is_red(x->parent)) {	 // ���ڵ������Ϊ��ɫ
		if (rb_tree_is_lchild(x->parent)) {		// ������ڵ������ӽڵ�
			auto uncle = x->parent - parent->right;		//�ҳ�����ڵ�
			if (uncle != nullptr && rb_tree_is_red(uncle)) {
				// case 3: ���ڵ������ڵ㶼Ϊ��
				rb_tree_set_black(x->parent);		// ���ĸ��ڵ�Ϊ��ɫ  
				rb_tree_set_black(uncle);			// ��������ڵ�Ϊ��ɫ  
				x = x->parent->parent;				// �ѵ�ǰ���ָ���游�ڵ�
				rb_tree_set_red(x);					//�游���Ϳ��
			}
			else {		// ������ڵ������ڵ�Ϊ��
				if (!rb_tree_is_lchild(x)) {
					// case 4: ��ǰ�ڵ� x Ϊ���ӽڵ�
					x = x->parent;					//��ǰ�ڵ�ĸ��ڵ���Ϊ�µĵ�ǰ�ڵ�
					rb_tree_rotate_left(x, root);	 // ���µ�ǰ�ڵ�Ϊ֧����������һ������Ϊ������  
				}
				// ��ת���� case 5�� ��ǰ�ڵ�Ϊ���ӽڵ�
				rb_tree_set_black(x->parent);		// ���ĸ��ڵ�Ϊ��ɫ 
				rb_tree_set_red(x->parent->parent);	//�����游�ڵ�Ϊ��ɫ
				rb_tree_rotate_right(x->parent->parent, root);	// ���游�ڵ�Ϊ֧����������һ������Ϊ������  
				break;
			}		
		}	//���ϱ���һ�������Ĳ����޸���������
			//case4��case5�������case3����ڵ��Ժ󣬽��е�һϵ�в����޸����������������ָ��ǰ�ڵ��ָ��һֱ�ڱ仯
		else {		// ������ڵ������ӽڵ㣬�Գƴ���
			auto uncle = x->parent->parent->left;	//�ҳ�����ڵ�
			if (uncle != nullptr && rb_tree_is_red(uncle)) {	// ������ڵ㣬��Ϊ��  
				// case 3: ���ڵ������ڵ㶼Ϊ��
				rb_tree_set_black(x->parent);		 // ���ĸ��ڵ�Ϊ��ɫ  
				rb_tree_set_black(uncle);			 // ��������ڵ�Ϊ��ɫ  
				x = x->parent - parent;				// �����游�ڵ�Ϊ��ɫ  
				rb_tree_set_red(x);
				// ��ʱ�游�ڵ�Ϊ�죬���ܻ��ƻ�����������ʣ��ǰ�ڵ�Ϊ�游�ڵ㣬��������
			}
			else {	// ������ڵ������ڵ�Ϊ��
				if (rb_tree_is_child(x)) {
					// case 4: ��ǰ�ڵ� x Ϊ���ӽڵ�
					x = x->parent;					//��ǰ�ڵ�ĸ��ڵ���Ϊ�µĵ�ǰ�ڵ�
					rb_tree_rotate_right(x, root);	 //���µ�ǰ�ڵ�Ϊ֧����������һ������Ϊ������  
				}
				// ��ת���� case 5�� ��ǰ�ڵ�Ϊ���ӽڵ�
				rb_tree_set_black(x->parent);		//������ɫ
				rb_tree_set_red(x->parent->parent);
				rb_tree_rotate_left(x->parent->parent, root);	 // ��һ������Ϊ������  
				break;
			}
		}
	}
	rb_tree_set_black(root);	// ���ڵ���ԶΪ��
}

// ɾ���ڵ��ʹ rb tree ����ƽ�⣬����һΪҪɾ���Ľڵ㣬������Ϊ���ڵ㣬������Ϊ��С�ڵ㣬������Ϊ���ڵ�
// 
// �ο�����: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153		ɾ����Ҫ�ο���ƪ���������
template <class NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) {
	// y �ǿ��ܵ��滻�ڵ㣬ָ������Ҫɾ���Ľڵ�
	auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
	// x �� y ��һ�����ӽڵ�� NIL �ڵ�
	auto x = y->left != nullptr ? y->left : y->right;
	// xp Ϊ x �ĸ��ڵ�
	NodePtr xp = nullptr;

	// y != z ˵�� z �������ǿ��ӽڵ㣬��ʱ y ָ�� z ������������ڵ㣬x ָ�� y �����ӽڵ㡣
	// �� y ���� z ��λ�ã��� x ���� y ��λ�ã������ y ָ�� z
	if (y != z) {
		z->left->parent = y;
		y->left = z->left;

		// ��� y ���� z �����ӽڵ㣬��ô z �����ӽڵ�һ��������
		if (y != z->right) {
			// x �滻 y ��λ��
			xp = y->parent;
			if (x != nullptr)
				x->parent = y->parent;

			y->parent - left = x;
			y->right = z->right;
			z->right->parent = y;
		}
		else {
			xp = y;
		}

		// ���� y �� z �ĸ��ڵ� 
		if (root = z)
			root = y;
		else if (rb_tree_is_lchild(z))
			z->parent - left = y;
		else
			z->parent - right = y;
		y->parent = z->parent;
		mystl::swap(y->color, z->color);
		y = z;
	}
	// y == z ˵�� z ����ֻ��һ������
	else {
		xp = y->parent;
		if (x)
			x->parent = y->parent;

		// ���� x �� z �ĸ��ڵ�
		if (root == z)
			root = x;
		else if (rb_tree_is_lchild(z))
			z->parent->left = x;
		else
			z->parent->right = x;

		// ��ʱ z �п���������ڵ�����ҽڵ㣬��������
		if (leftmost == z)
			leftmost = x = nullptr ? xp : rb_tree_min(x);
		if (rightmost == z)
			rightmost = x = nullptr ? xp : rb_tree_max(x);
	}

	// ��ʱ��y ָ��Ҫɾ���Ľڵ㣬x Ϊ����ڵ㣬�� x �ڵ㿪ʼ������
	// ���ɾ���Ľڵ�Ϊ��ɫ����������û�б��ƻ����������������������x Ϊ���ӽڵ�Ϊ������
	// case 1: �ֵܽڵ�Ϊ��ɫ����ڵ�Ϊ�죬�ֵܽڵ�Ϊ�ڣ��������ң�������������
	// case 2: �ֵܽڵ�Ϊ��ɫ���������ӽڵ㶼Ϊ��ɫ�� NIL�����ֵܽڵ�Ϊ�죬���ڵ��Ϊ��ǰ�ڵ㣬��������
	// case 3: �ֵܽڵ�Ϊ��ɫ�����ӽڵ�Ϊ��ɫ�� NIL�����ӽڵ�Ϊ��ɫ�� NIL��
	//         ���ֵܽڵ�Ϊ�죬�ֵܽڵ�����ӽڵ�Ϊ�ڣ����ֵܽڵ�Ϊ֧���ң���������������
	// case 4: �ֵܽڵ�Ϊ��ɫ�����ӽڵ�Ϊ��ɫ�����ֵܽڵ�Ϊ���ڵ����ɫ�����ڵ�Ϊ��ɫ���ֵܽڵ�����ӽڵ�
	//         Ϊ��ɫ���Ը��ڵ�Ϊ֧�����ң������������ʵ�����ɣ��㷨����
	
	if (!rb_tree_is_red(y)) {
		// x Ϊ��ɫʱ������������ֱ�ӽ� x ��Ϊ��ɫ����
		while (x != root && (x == nullptr || !rb_tree_is_red(x))) {
			if (x == xp->left) {
				// ��� x Ϊ���ӽڵ�
				auto brother = xp->right;	
				if (rb_tree_is_red(borther)) {		
					// case 1��x���ֵ�brotherΪ��ɫ
					rb_tree_set_black(borther);		//��x���ֵ���Ϊ��ɫ
					rb_tree_set_red(xp);			//��x�ĸ��ڵ���Ϊ��
					rb_tree_rotate_left(xp,root);	//��x�ĸ��ڵ�Ϊ֧���������
					brother = xp->right;			//x�����ֵ�Ϊ��ת֮ǰbrother��ĳ�����ӣ�Ϊ��ɫ
				}
				// case 1 תΪΪ�� case 2��3��4 �е�һ��
				if (brother->left == nullptr || !rb_tree_is_red(brother->left) &&
					(brother->right == nullptr || !rb_tree_is_red(brother->right))) {
					// case 2��x���ֵ�brother�Ǻ�ɫ�ģ���brother����������Ҳ�Ǻ�ɫ
					//��ΪbrotherҲ�Ǻ�ɫ�ģ�����x��brother�е�ȥ��һ��ɫ�����brother��Ϊ�졣
					rb_tree_set_red(brother);	//��brother��Ϊ��
					x = xp;						//x�ĸ��ڵ��Ϊ�µ�x
					xp = xp->parent;			//x�ĸ��ڵ�ĸ��ڵ��Ϊ�µĸ��ڵ�
				}
				else {
					if (brother->right == nullptr || !rb_tree_is_red(brother->right)) {
						// case 3��x���ֵ�brother�Ǻ�ɫ�ģ�brother�������Ǻ�ɫ��brother���Һ����Ǻ�ɫ
						if (brother->left != nullptr)
							rb_tree_set_black(brother->left);	//brother��������Ϊ��
						rb_tree_set_red(brother);				//brother��Ϊ��
						rb_tree_rotate_right(brother, root);	//��brotherΪ֧���������
						brother = xp->right;		//��ʱx�����ֵ�Ϊx���ڵ���Һ��ӣ���һ���к�ɫ�Һ��ӵĺڽ��
					}
					// ����case 3 תΪ case 4
					//case 4��x���ֵ�brother�Ǻ�ɫ�ģ���brother���Һ���ʱ��ɫ��
					brother->color = xp->color;		//��brother����ɫ��Ϊ���ڵ����ɫ������ɫ
					rb_tree_set_black(xp);			//�����ڵ���Ϊ��ɫ
					if (brother->right != nullptr)
						rb_tree_set_black(brother->right);	//��brother���Һ�����Ϊ��ɫ
					rb_tree_rotate_left(xp, root);	//�Ը��ڵ�Ϊ֧���������
					break;
				}
			}
			else {	// x Ϊ���ӽڵ㣬�Գƴ���
				auto brother = xp->left;
				if (rb_tree_is_red(brother))
				{ // case 1
					rb_tree_set_black(brother);
					rb_tree_set_red(xp);
					rb_tree_rotate_right(xp, root);
					brother = xp->left;
				}
				if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
					(brother->right == nullptr || !rb_tree_is_red(brother->right)))
				{ // case 2
					rb_tree_set_red(brother);
					x = xp;
					xp = xp->parent;
				}
				else
				{
					if (brother->left == nullptr || !rb_tree_is_red(brother->left))
					{ // case 3
						if (brother->right != nullptr)
							rb_tree_set_black(brother->right);
						rb_tree_set_red(brother);
						rb_tree_rotate_left(brother, root);
						brother = xp->left;
					}
					// תΪ case 4
					brother->color = xp->color;
					rb_tree_set_black(xp);
					if (brother->left != nullptr)
						rb_tree_set_black(brother->left);
					rb_tree_rotate_right(xp, root);
					break;
				}
			}
		}
		if (x != nullptr)
			rb_tree_set_black(x);		//��x��Ϊ��
	}
	return y;
}

// ģ���� rb_tree
// ����һ�����������ͣ������������ֵ�Ƚ�����
template <class T, class Compare>class rb_tree {		//class Compare������αȽϴ�С
public:
	// rb_tree ��Ƕ���ͱ��� 
	typedef rb_tree_traits<T>                        tree_traits;
	typedef rb_tree_value_traits<T>                  value_traits;

	//��ȡ����Ӧ�ͱ�
	typedef typename tree_traits::base_type          base_type;
	typedef typename tree_traits::base_ptr           base_ptr;
	typedef typename tree_traits::node_type          node_type;
	typedef typename tree_traits::node_ptr           node_ptr;
	typedef typename tree_traits::key_type           key_type;
	typedef typename tree_traits::mapped_type        mapped_type;
	typedef typename tree_traits::value_type         value_type;
	typedef Compare                                  key_compare;	//����rb_tree��αȽϴ�С

	//�ڴ������ͱ�
	typedef mystl::allocator<T>                      allocator_type;
	typedef mystl::allocator<T>                      data_allocator;
	typedef mystl::allocator<base_type>              base_allocator;
	typedef mystl::allocator<node_type>              node_allocator;	//�ڵ���������

	//��Ƕ�ͱ�
	typedef typename allocator_type::pointer         pointer;
	typedef typename allocator_type::const_pointer   const_pointer;
	typedef typename allocator_type::reference       reference;
	typedef typename allocator_type::const_reference const_reference;
	typedef typename allocator_type::size_type       size_type;
	typedef typename allocator_type::difference_type difference_type;

	//�������ͱ�
	typedef rb_tree_iterator<T>                      iterator;
	typedef rb_tree_const_iterator<T>                const_iterator;
	typedef mystl::reverse_iterator<iterator>        reverse_iterator;
	typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;

	allocator_type get_allocator() const { return node_allocator(); }	//���� mystl::allocator<node_type>
	key_compare key_comp()const { return key_comp_; }		//���ñȽ��㷨

private:
	base_ptr header_;		//����ڵ�,����ڵ㻥Ϊ�Է��ĸ��ڵ�
	size_type node_count_;	//�ڵ���
	key_compare key_comp_;	//�ڵ��ֵ�Ƚϵ�׼��

private:
	// ����������������ȡ�ø��ڵ㣬��С�ڵ�����ڵ�
	base_ptr& root() const { return header_->parent; }
	base_ptr& leftmost()const { return header_ - left; }
	base_ptr& rightmost() const { return header_->right; }

public:
	// ���졢���ơ���������
	rb_tree() { rb_tree_init(); }

	rb_tree(const rb_tree& rhs);
	rb_tree(rb_tree&& rhs) noexcept;

	rb_tree& operator=(const rb_tree& rhs);
	rb_tree& operator=(rb_tree&& rhs);

	~rb_tree() { clear(); }

public:
	// ��������ز���
	iterator begin()noexcept { return leftmost(); }
	const_iterator         begin()   const noexcept
	{
		return leftmost();
	}
	iterator               end()           noexcept
	{
		return header_;
	}
	const_iterator         end()     const noexcept
	{
		return header_;
	}

	//���������
	reverse_iterator       rbegin()        noexcept	
	{
		return reverse_iterator(end());
	}
	const_reverse_iterator rbegin()  const noexcept
	{
		return const_reverse_iterator(end());
	}
	reverse_iterator       rend()          noexcept
	{
		return reverse_iterator(begin());
	}
	const_reverse_iterator rend()    const noexcept
	{
		return const_reverse_iterator(begin());
	}

	//const������
	const_iterator         cbegin()  const noexcept
	{
		return begin();
	}
	const_iterator         cend()    const noexcept
	{
		return end();
	}
	const_reverse_iterator crbegin() const noexcept
	{
		return rbegin();
	}
	const_reverse_iterator crend()   const noexcept
	{
		return rend();
	}

	// ������ز���
	bool empty()const noexcept { return node_count_ == 0; }
	size_type size()     const noexcept { return node_count_; }
	//npos��һ����������ʾsize_t�����ֵ
	//����������ṩ���������������ʾ�����ڵ�λ�ã�����һ����std::container_type::size_type
	//static const size_type npos = -1�൱��static_cast<size_type>(-1),ע�������size_type��allocator�������޷�����
	size_type max_size() const noexcept { return static_cast<size_type>(-1); }

	// ����ɾ����ز���

	// emplace������

	template <class ...Args>
	iterator  emplace_multi(Args&& ...args);

	template <class ...Args>
	mystl::pair<iterator, bool> emplace_unique(Args&& ...args);

	template <class ...Args>
	iterator  emplace_multi_use_hint(iterator hint, Args&& ...args);

	template <class ...Args>
	iterator  emplace_unique_use_hint(iterator hint, Args&& ...args);

	// �����ظ�Ԫ�ص�insert������multi����
	iterator  insert_multi(const value_type& value);	
	iterator insert_multi(value_type&& value) {
		return emplace_multi(mystl::move(value));
	}

	iterator  insert_multi(iterator hint, const value_type& value)
	{
		return emplace_multi_use_hint(hint, value);
	}
	iterator  insert_multi(iterator hint, value_type&& value)
	{
		return emplace_multi_use_hint(hint, mystl::move(value));
	}

	template <class InputIterator>void insert_multi(InputIterator first, InputIterator last) {
		size_type n = mystl::distance(first, last);		//��ȡ[first,last]����
		//�ڵ����>���ֵ-n���׳��쳣
		THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
		for (; n > 0; --n, ++first)		//��������Ԫ��
			insert_multi(end(), *first);
	}

	//����pair���͵Ĳ���
	mystl::pair<iterator, bool> insert_unique(const value_type& value);
	mystl::pair<iterator, bool> insert_unique(value_type&& value)
	{
		return emplace_unique(mystl::move(value));
	}

	//�������ظ�ֵ�Ĳ���
	iterator  insert_unique(iterator hint, const value_type& value)
	{
		return emplace_unique_use_hint(hint, value);
	}
	iterator  insert_unique(iterator hint, value_type&& value)
	{
		return emplace_unique_use_hint(hint, mystl::move(value));
	}

	template <class InputIterator>
	void      insert_unique(InputIterator first, InputIterator last)
	{
		size_type n = mystl::distance(first, last);
		THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
		for (; n > 0; --n, ++first)
			insert_unique(end(), *first);
	}

	// erase
	iterator  erase(iterator hint);

	size_type erase_multi(const key_type& key);		//�����ظ�Ԫ��ֵ�����
	size_type erase_unique(const key_type& key);	//�������ظ�Ԫ��ֵ�����

	void      erase(iterator first, iterator last);

	//clear���
	void      clear();		

	// rb_tree ��ز���

	//find����
	iterator       find(const key_type& key);
	const_iterator find(const key_type& key) const;

	//ͳ���ظ�Ԫ�ظ���
	size_type count_multi(const key_type& key)const {	
		auto p = equal_range_multi(key);		//����key����
		return static_cast<size_type>(mystl::distance(p.first, p.second));
	}
	//ͳ�Ʋ��ظ�Ԫ�ظ���
	size_type      count_unique(const key_type& key) const
	{
		return find(key) != end() ? 1 : 0;	//Ҫô���ڣ�ҪôΪ0
	}

	//��first��last��ǰ�պ󿪵������н��ж��ֲ��ҵ�һ����С��x��ֵ
	iterator       lower_bound(const key_type& key);  
	const_iterator lower_bound(const key_type& key) const;

	//��first��last��ǰ�պ󿪵������н��ж��ֲ��ҵ�һ������x��ֵ
	iterator       upper_bound(const key_type& key);
	const_iterator upper_bound(const key_type& key) const;

	//����ĳ���ظ�Ԫ�ظ��������ĸ�������
	mystl::pair<iterator, iterator>
		equal_range_multi(const key_type& key) {
		return mystl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
	}
	mystl::pair<const_iterator, const_iterator>
		equal_range_multi(const key_type& key) const
	{
		return mystl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
	}

	//����ĳ�����ظ�Ԫ�ز����ĸ�������
	mystl::pair<iterator, iterator>equal_range_unique(const key_type& key) {
		iterator it = find(key);
		auto next = it;
		return it == end() ? mystl::make_pair(it, it) : mystl::make_pair(it, ++next);
	}
	mystl::pair<const_iterator, const_iterator>
		equal_range_unique(const key_type& key) const
	{
		const_iterator it = find(key);
		auto next = it;
		return it == end() ? mystl::make_pair(it, it) : mystl::make_pair(it, ++next);
	}

	//swap����
	void swap(rb_tree& rhs) noexcept;

private:
	// node related�ڵ�Ĵ�������¡�����ٲ���
	template <class ...Args>
	node_ptr create_node(Args&&... args);
	node_ptr clone_node(base_ptr x);
	void     destroy_node(node_ptr p);

	// init / reset �ڵ�ĳ�ʼ�������ò���
	void     rb_tree_init();
	void     reset();

	// get insert pos ��ȡ����Ԫ�صĲ���
	mystl::pair<base_ptr, bool>
		get_insert_multi_pos(const key_type& key);
	mystl::pair<mystl::pair<base_ptr, bool>, bool>
		get_insert_unique_pos(const key_type& key);

	// insert value / insert node	//����value�����ڵ����
	iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left);
	iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

	// insert use hint	����ʹ����ʾ����
	iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);
	iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

	// copy tree / erase tree	�������������
	base_ptr copy_from(base_ptr x, base_ptr p);
	void     erase_since(base_ptr x);
};

/*****************************************************************************************/

// ���ƹ��캯��
template <class T, class Compare>
rb_tree<T, Compare>::
rb_tree(const rb_tree& rhs) {
	rb_tree_init();		//rb_tree�ĳ�ʼ��
	if (rhs.node_count_ != 0) {	//�ڵ�������Ϊ��
		root() = copy_from(rhs.root, header);
		leftmost = rb_tree_min(root());
		rightmost = rb_tree_max(root());
	}
	node_count_ = rhs.node_count_;
	key_comp_ = rhs.key_comp_;
}

// �ƶ����캯��
template <class T, class Compare>
rb_tree<T, Compare>::
rb_tree(rb_tree&& rhs) noexcept
	:header_(mystl::move(rhs.header_)), node_count_(rgs.node_count_), key_comp_(rhs.key_comp_) {
	rhs.reset;	//�ƶ���Ϻ�rhs����
}

// ���Ƹ�ֵ������
template <class T, class Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::
operator=(const rb_tree& rhs)
{
	if (this != &rhs)	//�������Ҹ�ֵ�����
	{
		clear();		//�������ռ�

		if (rhs.node_count_ != 0)	//�ڵ�����Ϊ�գ����п�����ֵ
		{
			root() = copy_from(rhs.root(), header_);
			leftmost() = rb_tree_min(root());
			rightmost() = rb_tree_max(root());
		}

		node_count_ = rhs.node_count_;
		key_comp_ = rhs.key_comp_;
	}
	return *this;
}

// �ƶ���ֵ������
template <class T, class Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::
operator=(rb_tree&& rhs)
{
	clear();
	header_ = mystl::move(rhs.header_);
	node_count_ = rhs.node_count_;
	key_comp_ = rhs.key_comp_;
	rhs.reset();	//�ƶ���Ϻ�����rhs
	return *this;
}

// �͵ز���Ԫ�أ���ֵ�����ظ�
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
emplace_multi(Args&& ...args) {
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	node_ptr np = create_node(mystl::forward<Args>(args)...);	//ʹ��forward()��֤������������ת��
	auto res = get_insert_multi_pos(value_traits::get_key(np->value));	//���ø�������
	return insert_node_at(res.first, np, res.second);
}

// �͵ز���Ԫ�أ���ֵ�������ظ�
template <class T, class Compare>
template <class ...Args>
mystl::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::
emplace_unique(Args&& ...args)
{
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	node_ptr np = create_node(mystl::forward<Args>(args)...);	//ʹ��forward()��֤������������ת��
	auto res = get_insert_unique_pos(value_traits::get_key(np->value));	//���ø�������
	if (res.second)
	{ // ����ɹ�
		return mystl::make_pair(insert_node_at(res.first.first, np, res.first.second), true);	//������Ӧ��pair
	}
	destroy_node(np);	//���벻�ɹ���ȫ�����٣�������make_pairΪfalse
	return mystl::make_pair(iterator(res.first.first), false);	
}

// �͵ز���Ԫ�أ���ֵ�����ظ����� hint λ�������λ�ýӽ�ʱ�����������ʱ�临�Ӷȿ��Խ���
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
emplace_multi_use_hint(iterator hint, Args&& ...args) {		//hintΪ����Ԫ��
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	node_ptr np = create_node(mystl::forward<Args>(args)...);  //ʹ��forward()��֤������������ת��
	if (node_count_ == 0) {	//�ڵ����Ϊ0�����뵽header֮��
		return insert_node_at(header_, np, true);	
	}
	key_type key = value_traits::get_key(np->value);	//��ȡkey���ͱ�
	if (hint == begin()) {	// ����Ԫ��λ�� begin ��
		if (key_comp_(key, value_traits::get_key(*hint))) {	//��ȡ����Ԫ���ͱ�
			return insert_node_at(hint.node, np, true);
		}
		else
		{
			auto pos = get_insert_multi_pos(key);		//��ȡ��Ӧpair
			return insert_node_at(pos.first, np, pos.second);
		}
	}
	else if (hint == end())
	{ // λ�� end ��
		if (!key_comp_(key, value_traits::get_key(rightmost()->get_node_ptr()->value)))	//�Ƚ���Ӧ�ͱ�
		{
			return insert_node_at(rightmost(), np, false);	//���Ҳ���
		}
		else
		{
			auto pos = get_insert_multi_pos(key);
			return insert_node_at(pos.first, np, pos.second);
		}
	}
	return insert_multi_use_hint(hint, key, np);
}

// �͵ز���Ԫ�أ���ֵ�������ظ����� hint λ�������λ�ýӽ�ʱ�����������ʱ�临�Ӷȿ��Խ���
template<class T, class Compare>
template<class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
emplace_unique_use_hint(iterator hint, Args&& ...args)
{
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	node_ptr np = create_node(mystl::forward<Args>(args)...);
	if (node_count_ == 0)
	{
		return insert_node_at(header_, np, true);
	}
	key_type key = value_traits::get_key(np->value);
	if (hint == begin())
	{ // λ�� begin ��
		if (key_comp_(key, value_traits::get_key(*hint)))
		{
			return insert_node_at(hint.node, np, true);
		}
		else
		{
			auto pos = get_insert_unique_pos(key);
			if (!pos.second)
			{
				destroy_node(np);
				return pos.first.first;
			}
			return insert_node_at(pos.first.first, np, pos.first.second);
		}
	}
	else if (hint == end())
	{ // λ�� end ��
		if (key_comp_(value_traits::get_key(rightmost()->get_node_ptr()->value), key))
		{
			return insert_node_at(rightmost(), np, false);
		}
		else
		{
			auto pos = get_insert_unique_pos(key);
			if (!pos.second)
			{
				destroy_node(np);
				return pos.first.first;
			}
			return insert_node_at(pos.first.first, np, pos.first.second);
		}
	}
	return insert_unique_use_hint(hint, key, np);
}

// ����Ԫ�أ��ڵ��ֵ�����ظ�
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_multi(const value_type& value)
{
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	auto res = get_insert_multi_pos(value_traits::get_key(value));
	return insert_value_at(res.first, value, res.second);
}

// ������ֵ���ڵ��ֵ�������ظ�������һ�� pair��������ɹ���pair �ĵڶ�����Ϊ true������Ϊ false
template <class T, class Compare>
mystl::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::
insert_unique(const value_type& value)
{
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	auto res = get_insert_unique_pos(value_traits::get_key(value));
	if (res.second)
	{ // ����ɹ�
		return mystl::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
	}
	return mystl::make_pair(res.first.first, false);
}

// ɾ�� hint λ�õĽڵ�
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
erase(iterator hint) {
	auto node = hint.node->get_node_ptr();	//��ȡ��ɾ��Ԫ�ص�ָ��
	iterator next(node);	
	++next;

	rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());	//����ɾ��Ԫ�غ�ĵ���
	destroy_node(node);		//����Ԫ��
	--node_count_;
	return next;
}

// ɾ����ֵ���� key ��Ԫ�أ�����ɾ���ĸ���
template <class T, class Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::
erase_multi(const key_type& key) {		//�����ظ�Ԫ��
	auto p = equal_range_multi(key);	//��ȡ��Ӧkey
	size_type n = mystl::distance(p.first, p.last);	//���Ҽ�ֵ���� key ��Ԫ�ظ���
	erase(p.first, p.second);	//����ɾ��
	return n;
}

// ɾ����ֵ���� key ��Ԫ�أ�����ɾ���ĸ���
template <class T, class Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::
erase_unique(const key_type& key) {		//�������ظ�Ԫ��
	auto it = find(key);	//���Ҽ�ֵ���� key ��Ԫ��
	if (it != end()) {
		erase(it);	//����ɾ��
		return 1;
	}
	return 0;
}

// ɾ��[first, last)�����ڵ�Ԫ��
template <class T, class Compare>
void rb_tree<T, Compare>::
erase(iterator first, iterator last)
{
	if (first == begin() && last == end())
	{
		clear();
	}
	else
	{
		while (first != last)
			erase(first++);
	}
}

// ��� rb tree
template <class T, class Compare>
void rb_tree<T, Compare>::
clear()
{
	if (node_count_ != 0)
	{
		erase_since(root());
		leftmost() = header_;	//����header
		root() = nullptr;
		rightmost() = header_;
		node_count_ = 0;
	}
}

// ���Ҽ�ֵΪ k �Ľڵ㣬����ָ�����ĵ�����
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
find(const key_type& key) {
	auto y = header_;		// ���һ����С�� key �Ľڵ�
	auto x = root();		//��ǰ�ڵ�
	while (x != nullptr) {
		if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {	// key_comp_�ǽڵ��ֵ��С�ȽϺ���
			// key С�ڵ��� x ��ֵ��������
			y = x, x = x->left;	
		}
		else {	// key ���� x ��ֵ��������
			x = x->right;
		}
	}
	iterator j = iterator(y);
	// y�ļ�ֵ��С��key�����ص�ʱ����Ҫ�ж���key����Ȼ���С��  
	return (j==end()|| key_comp_(key, value_traits::get_key(*j))) ? end() : j;	
}
//const�������汾
template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::
find(const key_type& key) const
{
	auto y = header_;  // ���һ����С�� key �Ľڵ�
	auto x = root();
	while (x != nullptr)
	{
		if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key))
		{ // key С�ڵ��� x ��ֵ��������
			y = x, x = x->left;
		}
		else
		{ // key ���� x ��ֵ��������
			x = x->right;
		}
	}
	const_iterator j = const_iterator(y);
	return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
}

// ��ֵ��С�� key �ĵ�һ��λ��
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
lower_bound(const key_type& key) {
	auto y = header_;	// ���һ����С�� key �Ľڵ�
	auto x = root();
	while (x != nullptr) {
		if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
			// key С�ڵ��� x ��ֵ��������
			y = x, x = x->left;
		}
		else {	// key ���� x ��ֵ��������
			x = x->rght;
		}
	}
	return iterator(y);
}
//const�������汾
template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::
lower_bound(const key_type& key) const
{
	auto y = header_;
	auto x = root();
	while (x != nullptr)
	{
		if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key))
		{ // key <= x
			y = x, x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	return const_iterator(y);
}

// ��ֵ��С�� key �����һ��λ��
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
upper_bound(const key_type& key)
{
	auto y = header_;	// ���һ����С�� key �Ľڵ�
	auto x = root();
	while (x != nullptr)
	{
		if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value)))
		{ // key < x	 key ���� x ��ֵ��������
			y = x, x = x->left;
		}
		else	// key С�ڵ��� x ��ֵ��������
		{
			x = x->right;
		}
	}
	return iterator(y);
}
//const�������汾
template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::
upper_bound(const key_type& key) const
{
	auto y = header_;
	auto x = root();
	while (x != nullptr)
	{
		if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value)))
		{ // key < x
			y = x, x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	return const_iterator(y);
}

// ���� rb tree
template <class T, class Compare>
void rb_tree<T, Compare>::
swap(rb_tree& rhs) noexcept
{
	if (this != &rhs)
	{
		mystl::swap(header_, rhs.header_);
		mystl::swap(node_count_, rhs.node_count_);
		mystl::swap(key_comp_, rhs.key_comp_);
	}
}

/*****************************************************************************************/
// helper function

// ����һ�����
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::
create_node(Args&&... args) {
	auto tmp = node_allocator::allocate(1);		//���ÿռ�
	try {
		data_allocator::construct(mystl::address_of(tmp->value), mystl::forward<Args>(args)...); //��������
		tmp->left = nullptr;
		tmp->right = nullptr;
		tmp->parent = nullptr;
	}
	catch (...) {
		node_allocator::deallocate(tmp);	//����ʧ�ܾ�ȫ���ͷ�
		throw;
	}
	return tmp;
}

// ����һ�����
template <class T, class Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::
clone_node(base_ptr x) {	//����һ���ڵ��ֵ����ɫ
	node_ptr tmp = create_node(x->get_node_ptr()->value);
	tmp->color = x->color;
	tmp->left = nullptr;
	tmp->right = nullptr;
	return tmp;
}

// ����һ�����
template <class T, class Compare>
void rb_tree<T, Compare>::
destroy_node(node_ptr p) {
	data_allocator::destroy(&p->value);		
	node_allocator::deallocate(p);
}

// ��ʼ������
template <class T, class Compare>
void rb_tree<T, Compare>::
rb_tree_init() {
	header_= base_allocator::allocate(1);	//���ÿռ�
	header_->color = rb_tree_red;	// header_ �ڵ���ɫΪ�죬�� root ����
	root() = nullptr;	
	leftmost = header_;			//��ָ��header
	rightmost = header_;
	node_count_ = 0;
}

// reset ����
template <class T, class Compare>
void rb_tree<T, Compare>::reset() {
	header_ = nullptr;
	node_count_ = 0;
}

// get_insert_multi_pos ������������ֵ������ڵ��ֵ�ظ�
template <class T, class Compare>
mystl::pair<typename rb_tree<T, Compare>::base_ptr, bool>
rb_tree<T, Compare>::get_insert_multi_pos(const key_type& key) {
	auto x = root();
	auto y = header_;
	bool add_to_left = true;
	while (x != nullptr) {
		y = x;
		add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));	//�Ƚ�key��value
		// x��value��key�Ƚ� ���x���ڵ���key�Ļ��������ߣ���������ұ���
		x = add_to_left ? x->left : x->right;	
	}
	return mystl::make_pair(y, add_to_left);	
}

// get_insert_unique_pos ������������ֵ���������ظ�
template <class T, class Compare>
mystl::pair<mystl::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>
rb_tree<T, Compare>::get_insert_unique_pos(const key_type& key) {
	// ����һ�� pair����һ��ֵΪһ�� pair�����������ĸ��ڵ��һ�� bool ��ʾ�Ƿ�����߲��룬
	// �ڶ���ֵΪһ�� bool����ʾ�Ƿ����ɹ�
	auto x = root();
	auto y = header_;
	bool add_to_left = true;	// ��Ϊ��ʱҲ�� header_ ��߲���
	while (x != nullptr) {
		y = x;
		add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
		// x��value��key�Ƚ� ���x���ڵ���key�Ļ��������ߣ���������ұ���
		x = add_to_left ? x->left : x->right;
	}
	iterator j = iterator(y);	// ��ʱ y Ϊ�����ĸ��ڵ�
	if (add_to_left) {
		if (y == header_ || j == begin()) {
			// �����Ϊ����������������ڵ㴦���϶����Բ����µĽڵ�
			return mystl::make_pair(mystl::make_pair(y, true), true);
		}
		else {	// ������������ظ��ڵ㣬��ô --j �����ظ���ֵ,����--jѭ�������ϲ���
			--j;
		}
	}
	if (key_comp_(value_traits::get_key(*j), key)) {
		// �����½ڵ�û���ظ�,��С���½ڵ㣬���½ڵ�����Ҳ�
		return mystl::make_pair(mystl::make_pair(y, add_to_left), true);
	}
	// �������ˣ���ʾ�½ڵ������нڵ��ֵ�ظ�
	return mystl::make_pair(mystl::make_pair(y, add_to_left), false);
}

// insert_value_at �����������Ĳ���ִ�г���
// x Ϊ�����ĸ��ڵ㣬 value ΪҪ�����ֵ��add_to_left ��ʾ�Ƿ�����߲���
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_value_at(base_ptr x, const value_type& value, bool add_to_left) {
	node_ptr node = create_node(value);
	node->parent = x;
	auto base_node = node->get_base_ptr();
	if (x == header_) {
		root() = base_node;
		leftmost() = base_node;
		rightmost() = base_node;
	}
	else if (add_to_left) {
		x->left = base_node;
		if (leftmost() == x)
			leftmost() = base_node;
	}
	else {
		x->right = base_node;
		if (rightmost() == x)
			rightmost() = base_node;
	}
	rb_tree_insert_rebalance(base_node, root());
	++node_count_;
	return iterator(node);
}

// �� x �ڵ㴦�����µĽڵ�
// x Ϊ�����ĸ��ڵ㣬 node ΪҪ����Ľڵ㣬add_to_left ��ʾ�Ƿ�����߲���
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_node_at(base_ptr x, node_ptr node, bool add_to_left) {
	node->parent = x;
	auto base_node = node->get_base_ptr();
	if (x == header_)
	{
		root() = base_node;
		leftmost() = base_node;
		rightmost() = base_node;
	}
	else if (add_to_left)
	{
		x->left = base_node;
		if (leftmost() == x)
			leftmost() = base_node;
	}
	else
	{
		x->right = base_node;
		if (rightmost() == x)
			rightmost() = base_node;
	}
	rb_tree_insert_rebalance(base_node, root());
	++node_count_;
	return iterator(node);
}

// ����Ԫ�أ���ֵ�����ظ���ʹ�� hint
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_multi_use_hint(iterator hint, key_type key, node_ptr node) {
	// �� hint ����Ѱ�ҿɲ����λ��
	auto np = hint.node;
	auto before = hint;
	--before;
	auto bnp = before.node;
	if (!key_comp_(key, value_traits::get_key(*before)) && !key_comp_(value_traits::get_key(*hint), key)) {
		// before <= node <= hint
		if (bnp->right == nullptr) {
			return insert_node_at(bnp, node, false);	//�ұ߲���
		}
		else if (np->left == nullptr) {
			return insert_node_at(np, node, true);		//��߲���
		}
	}
	auto pos=get_insert_multi_pos(key);					//�����ظ�Ԫ��
	return insert_node_at(pos.first, node, pos.second);
}

// ����Ԫ�أ���ֵ�������ظ���ʹ�� hint
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_unique_use_hint(iterator hint, key_type key, node_ptr node) {
	// �� hint ����Ѱ�ҿɲ����λ��
	auto np = hint.node;
	auto before = hint;
	--before;
	auto bnp = before.node;
	if (key_comp_(value_traits::get_key(*before), key) &&
		key_comp_(key, value_traits::get_key(*hint)))
	{ // before < node < hint
		if (bnp->right == nullptr)
		{
			return insert_node_at(bnp, node, false);	//�ұ߲���
		}
		else if (np->left == nullptr)
		{
			return insert_node_at(np, node, true);		//��߲���
		}
	}
	auto pos= get_insert_unique_pos(key);		//���ظ�Ԫ�ز���
	if (!pos.second) {		//û��value�����ٽڵ�
		destroy_node(node);
		return pos.first.first;
	}
	return insert_node_at(pos.first.first, node, pos.first.second);		//����pos����һԪ��
}

// copy_from ����
// �ݹ鸴��һ�������ڵ�� x ��ʼ��p Ϊ x �ĸ��ڵ�
template <class T, class Compare>
typename rb_tree<T, Compare>::base_ptr
rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p) {
	auto top = clone_node(x);
	top->parent = p;
	try {
		if (x->right)
			top->right = copy_from(x->right, top);	//�ݹ�������
		p = top;
		x = x->left;	//�����������ĵݹ�
		while (x!=nullptr)
		{
			auto y = clone_node(x);
			p->left = y;
			y->parent = p;
			if (x->right)
				y->right = copy_from(x->right, y);
			p = y;
			x = x->left;
		}
	}
	catch (...) {
		erase_since(top);
		throw;
	}
}

// erase_since ����
// �� x �ڵ㿪ʼɾ���ýڵ㼰������
template <class T, class Compare>
void rb_tree<T, Compare>::
erase_since(base_ptr x) {
	while (x!=nullptr)
	{
		erase_since(x->right);	//�ݹ�������
		auto y = x->left;
		destroy_node(x->get_node_ptr());
		x = y;
	}
}

// ���رȽϲ�����
template <class T, class Compare>
bool operator==(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
	return lhs.size() == rhs.size() && mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class Compare>
bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
	return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class Compare>
bool operator!=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
	return !(lhs == rhs);
}

template <class T, class Compare>
bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
	return rhs < lhs;
}

template <class T, class Compare>
bool operator<=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
	return !(rhs < lhs);
}

template <class T, class Compare>
bool operator>=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
	return !(lhs < rhs);
}

// ���� mystl �� swap
template <class T, class Compare>
void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept
{
	lhs.swap(rhs);
}

}	//�����ռ������

#endif // !MYSTL_RB_TREE_H_

