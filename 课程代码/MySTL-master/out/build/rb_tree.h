#ifndef MYSTL_RB_TREE_H_
#define MYSTL_RB_TREE_H_

// 这个头文件包含一个模板类 rb_tree
// rb_tree : 红黑树

#include <initializer_list>		//初始化列表

#include <cassert>				//断言

#include "functional.h"			//仿函数
#include "iterator.h"			//迭代器
#include "memory.h"				//高级的动态内存管理
#include "type_traits.h"		//萃取机
#include "exceptdef.h"			//异常

namespace mystl {

// rb tree 节点颜色的类型
typedef bool rb_tree_color_type;
//constexpx用于指定编译器把返回值当做编译器常量使用，但这只是一种声明，也有可能返回为运行期常量
static constexpr rb_tree_color_type rb_tree_red = false;	//红色为0
static constexpr rb_tree_color_type rb_tree_black = true;	//黑色为1

// forward declaration前置声明
template <class T> struct rb_tree_node_base;	//节点结构的基类，定义左右子节点与父节点指针
template <class T> struct rb_tree_node;			//节点结构的子类，定义数据值

template <class T> struct rb_tree_iterator;		//迭代器设计，非连续空间需要具体定义类来表示迭代器
template <class T> struct rb_tree_const_iterator;

// rb tree value traits萃取值型别的类
template <class T, bool>struct rb_tree_value_traits_imp {
	typedef T key_type;			//容器的key值
	typedef T mapped_type;		//容器的value值
	typedef T value_type;		//容器的pair值，即pair<const T,T>，表示具体值的型别

	//用于key与value相同的数据结构
	template <class Ty>static const key_type& get_key(const Ty& value) { return value; }
	template <class Ty>static const value_type& get_value(const Ty& value) { return value; }
};

template <class T> struct rb_tree_value_traits_imp<T, true> {
	//remove_cv提供与type相同的成员 typedef T，只是删除了其最顶层的 cv 限定符,即c（const）、v（volatile）限定符
	//参考链接:https://www.cnblogs.com/5iedu/p/7767937.html
			   https://blog.csdn.net/janeqi1987/article/details/100049597
	typedef typename std::remove_cv<typename T::first_type>::type key_type;			//pair的first，即key值
	typedef typename T::second_type                               mapped_type;		//pair的second，即mapped值
	typedef T                                                     value_type;		//pair类型，包含first与second

	//用于pair类型
	template <class Ty>static const key_type& get_key(const T& value) {
		return value.first;
	}
	template <class Ty>static const value_type& get_value(const Ty& value){
		return value;
	}
};

template <class T>struct rb_tree_value_traits {
	static constexpr bool is_map = mystl::is_pair<T>::value;	//判断是否是pair
	typedef rb_tree_value_traits_imp<T, is_map> value_traits_type; //调用上面的 rb_tree_value_traits

	typedef typename value_traits_type::key_type    key_type;
	typedef typename value_traits_type::mapped_type mapped_type;
	typedef typename value_traits_type::value_type  value_type;

	//用于key和value不同的数据结构
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

// rb tree node traits节点的萃取
template <class T>struct rb_tree_node_traits {
	typedef rb_tree_color_type                 color_type;	//节点颜色类型

	typedef rb_tree_value_traits<T>				value_traits;
	typedef typename value_traits::key_type		key_type;
	typedef typename value_traits::mapped_type	mapped_type;
	typedef typename value_traits::value_type	value_type;

	//双重节点结构的指针
	typedef rb_tree_node_base<T>* base_ptr;		
	typedef rb_tree_node<T>*	  node_ptr;
};

template <class T>struct rb_tree_node_base {	//节点结构的基类，保存基本的指针
	typedef rb_tree_color_type color_type;		//颜色类型
	typedef rb_tree_node_base<T>* base_ptr;		//基类指针
	typedef rb_tree_node<T>* node_ptr;			//子类指针

	base_ptr   parent;  // 父节点，用于方便“上溯”父节点的操作
	base_ptr   left;    // 左子节点
	base_ptr   right;   // 右子节点
	color_type color;   // 节点颜色

	base_ptr get_base_ptr()
	{
		return &*this;		//*this表示this指针指向的对象，&*this表示*this指向的对象的指针
	}

	node_ptr get_node_ptr()
	{
		//reinterpret_cast：允许将任何指针转换为任何其他指针类型。 也允许将任何整数类型转换为任何指针类型以及反向转换
		return reinterpret_cast<node_ptr>(&*this);	//获取相应指针
	}

	node_ptr& get_node_ref()
	{
		return reinterpret_cast<node_ptr&>(*this);	//获取相应引用
	}
};

template <class T>struct rb_tree_node :public rb_tree_node_base<T> {	//节点结构的子类
	typedef rb_tree_node_base<T>* base_ptr;
	typedef rb_tree_node<T>* node_ptr;

	T value;  // 节点值

	base_ptr get_base_ptr()
	{
	//static_cast将expression转换为type-id类型，主要用于非多态类型之间的转换，不提供运行时的检查来确保转换的安全性
		return static_cast<base_ptr>(&*this);	//把子类的指针或引用转换成父类表示
	}

	node_ptr get_node_ptr()
	{
		return &*this;	//this是指针，*this表示指针指向的对象，&*this即this指针指向的对象的指针
	}
};

// rb tree traits
template <class T>
struct rb_tree_traits
{
	typedef rb_tree_value_traits<T>            value_traits;

	//相应型别的typedef
	typedef typename value_traits::key_type    key_type;
	typedef typename value_traits::mapped_type mapped_type;
	typedef typename value_traits::value_type  value_type;

	typedef value_type*							pointer;
	typedef value_type&							reference;
	typedef const value_type*					const_pointer;
	typedef const value_type&					const_reference;

	typedef rb_tree_node_base<T>               base_type;
	typedef rb_tree_node<T>                    node_type;

	//指针typedef
	typedef base_type* base_ptr;
	typedef node_type* node_ptr;
};

// rb tree 的迭代器设计

//基层迭代器
template <class T>struct rb_tree_iterator_base :public mystl::iterator<mystl::bidirectional_iterator_tag, T> {
	typedef typename rb_tree_traits<T>::base_ptr base_ptr;
	base_ptr node;	// 指向节点本身,用来与容器之间产生一个连结关系

	// 使迭代器前进，寻找该节点的后继节点，即刚好比当前节点大的元素
	void inc() {
		if (node->right != nullptr) {	//如果右子节点存在
			node = rb_tree_min(node->right);	//找到右子树的最小值，即右子树的最下层的最左节点
		}
		else {	 // 如果没有右子节点
			auto y = node->parent;		//找出父节点
			while (y->right == node) {	//如果现行节点本身是个右节点
				node = y;				//一直上溯，直到不为右子节点为止
				y = y->parent;
			}
			// 应对“寻找根节点的下一节点，而根节点没有右子节点”的特殊情况
			if (node->right != y)		//若此时的右子节点不等于此时的父节点
				node = y;				//此时的父节点即为解答
										//否则此时的node为解答
		}
	}

	// 使迭代器后退，寻找该节点的前置节点
	void dec() {
		//如果是红节点，且父节点的父节点等于自己
		if (node->parent->parent == node && rb_tree_is_red(node)) {
			node = node->right;		//右子节点即为解答
		}
		//以上情况发生于node为header时，即node为end()时，此时最大元素刚好是node的前驱节点
		// 注意：header的右子节点为most right，指向整棵树的max节点
		else if (node->left != nullptr) {		// 如果存在左子节点
			node = rb_tree_max(node->left);		// 跳到左子节点上，找出左子树的最大节点，即左子树的最右
		}
		else {		 // 如果该节点既非根节点，也不存在左子节点
			auto y = node->parent;		 // 跳到它的父节点上
			while (node == y->left) {	 // 如果它等于它的父子节点的左子节点
				node = y;				 // 则一直往上查，直到现行节点不为左子节点
				y = y->parent;
			}							
			node = y;					 // 此时他的父节点即为要找的前驱节点
		}
	}

	//操作符重载
	bool operator==(const rb_tree_iterator_base& rhs) { return node == rhs.node; }
	bool operator!=(const rb_tree_iterator_base& rhs) { return node != rhs.node; }
};

//子类迭代器
template <class T> struct rb_tree_iterator :public rb_tree_iterator_base<T> {
	typedef rb_tree_traits<T>                tree_traits;

	//内嵌型别声明
	typedef typename tree_traits::value_type value_type;
	typedef typename tree_traits::pointer    pointer;
	typedef typename tree_traits::reference  reference;
	typedef typename tree_traits::base_ptr   base_ptr;
	typedef typename tree_traits::node_ptr   node_ptr;

	//迭代器声明
	typedef rb_tree_iterator<T>              iterator;
	typedef rb_tree_const_iterator<T>        const_iterator;
	typedef iterator                         self;

	using rb_tree_iterator_base<T>::node;	//using在这里的作用是在子类中引用基类的成员

	// 构造函数
	rb_tree_iterator() {}
	rb_tree_iterator(base_ptr x) { node = x; }
	rb_tree_iterator(node_ptr x) { node = x; }
	rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
	rb_tree_iterator(const const_iterator& rhs) { node = rhs.node; }

	// 重载操作符
	reference operator*()  const { return node->get_node_ptr()->value; }
	pointer   operator->() const { return &(operator*()); }

	self& operator++()		//前置++
	{
		this->inc();		//调用inc()函数
		return *this;
	}
	self operator++(int)	//后置++
	{
		self tmp(*this);
		this->inc();		//调用inc()函数
		return tmp;
	}
	self& operator--()		//前置--
	{
		this->dec();		//调用dnc()函数
		return *this;
	}
	self operator--(int)	//后置--
	{
		self tmp(*this);
		this->dec();		//调用inc()函数
		return tmp;
	}
};

//const的子类迭代器
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

	// 构造函数
	rb_tree_const_iterator() {}
	rb_tree_const_iterator(base_ptr x) { node = x; }
	rb_tree_const_iterator(node_ptr x) { node = x; }
	rb_tree_const_iterator(const iterator& rhs) { node = rhs.node; }
	rb_tree_const_iterator(const const_iterator& rhs) { node = rhs.node; }

	// 重载操作符
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

// tree algorithm 红黑树的算法
template <class NodePtr>NodePtr rb_tree_min(NodePtr x)noexcept {
	while (x->left != nullptr) {	//一直往左走，就能找到红黑树的最小值节点
		x = x->left;				//二叉搜索树的性质
	}
	return x;
}

template <class NodePtr>NodePtr rb_tree_max(NodePtr x)noexcept {
	while (x->right != nullptr) {	//一直往右走，就能找到红黑树的最大值节点
		x = x->right;				 // 二叉搜索树的性质
	}
	return x;
}

template <class NodePtr> bool rb_tree_is_lchild(NodePtr node) noexcept
{
	return node == node->parent->left;	//判断是否是左子节点
}

template <class NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept
{
	return node->color == rb_tree_red;	//判断节点是否是红颜色
}

template <class NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept
{
	node->color = rb_tree_black;		//设置当前节点为黑色
}

template <class NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept
{
	node->color = rb_tree_red;			//设置当前节点为红色
}

//寻找当前节点的下一节点
template <class NodePtr>NodePtr rb_tree_next(NodePtr node)noexcept {
	if (node->right != nullptr) {			//右子树不为空
		return rb_tree_min(node->right);	//右子树的最小节点
	}
	while (!rb_tree_is_lchild(node))		//不存在左子节点
		node = node->parent;				//向上找父节点
	return node->parent;					//返回父节点的父节点
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
// 左旋，参数一为左旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept {
	//x为旋转点
	auto y = x->right;			// 令y 为 x 的右子节点
	x->right = y->left;			//如上图，将b连到x，成为x的右子节点
	if (y->left != nullptr) {	//如果y的左子节点不为空
		y->left->parent = x;	//令y成为x的父节点，并将x放在左子节点的位置
	}
	y->parent = x->parent;		//令x的父节点为y的父节点，即让p为y的父节点

	if (x == root) {		// 如果 x 为根节点，让 y 顶替 x 成为根节点
		root = y;
	}
	else if (rb_tree_is_lchild(x)) {	 // 如果 x 是其父节点的左子节点
		x->parent->left = y;			//让y成为x的父节点的左子树，即让y成为p的左子节点
	}
	else {								// 如果 x 是其父节点的右子节点
		x->parent->right = y;			//让y成为x的父节点的右子树，即让y成为p的右子节点
	}
	// 调整 x 与 y 的关系
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
// 右旋，参数一为右旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept {
	auto y = x->left;				// 令y 为 x 的左子节点
	x->left = y->right;				//如上图，将c连到x，成为x的左子节点
	if (y->right) {					//如果y的右子节点不为空
		y->right->parent = x;		//令y成为x的父节点，并将x放在右子节点的位置
	}
	y->parent = x->parent;			//令x的父节点为y的父节点，即让p为y的父节点

	if (x == root) {	// 如果 x 为根节点，让 y 顶替 x 成为根节点
		root = y;
	}
	else if(rb_tree_is_lchild(x))	 // 如果 x 是右子节点
	{
		x->parent->left = x;		//让y成为x的父节点的左子树，即让y成为p的左子节点
	}
	else {							// 如果 x 是左子节点
		x->parent->right = y;		//让y成为x的父节点的右子树，即让y成为p的右子节点
	}
	// 调整 x 与 y 的关系
	y->right = x;
	x->parent = y;
}

// 插入节点后使 rb tree 重新平衡，参数一为新增节点，参数二为根节点
//
// case 1: 新增节点位于根节点，令新增节点为黑
// case 2: 新增节点的父节点为黑，没有破坏平衡，直接返回
// case 3: 父节点和叔叔节点都为红，令父节点和叔叔节点为黑，祖父节点为红，
//         然后令祖父节点为当前节点，继续处理
// case 4: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为右（左）孩子，
//         让父节点成为当前节点，再以当前节点为支点左（右）旋
// case 5: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为左（右）孩子，
//         让父节点变为黑色，祖父节点变为红色，以祖父节点为支点右（左）旋
//
// 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630		插入主要参考这篇的四种情况
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept {
	rb_tree_set_red(x);			// 新增节点必为红色
	while (x != root && rb_tree_is_red(x->parent)) {	 // 父节点存在且为红色
		if (rb_tree_is_lchild(x->parent)) {		// 如果父节点是左子节点
			auto uncle = x->parent - parent->right;		//找出叔叔节点
			if (uncle != nullptr && rb_tree_is_red(uncle)) {
				// case 3: 父节点和叔叔节点都为红
				rb_tree_set_black(x->parent);		// 更改父节点为黑色  
				rb_tree_set_black(uncle);			// 更改叔叔节点为黑色  
				x = x->parent->parent;				// 把当前结点指向祖父节点
				rb_tree_set_red(x);					//祖父结点涂红
			}
			else {		// 无叔叔节点或叔叔节点为黑
				if (!rb_tree_is_lchild(x)) {
					// case 4: 当前节点 x 为右子节点
					x = x->parent;					//当前节点的父节点做为新的当前节点
					rb_tree_rotate_left(x, root);	 // 以新当前节点为支点左旋，第一个参数为左旋点  
				}
				// 都转换成 case 5： 当前节点为左子节点
				rb_tree_set_black(x->parent);		// 更改父节点为黑色 
				rb_tree_set_red(x->parent->parent);	//更改祖父节点为红色
				rb_tree_rotate_right(x->parent->parent, root);	// 在祖父节点为支点右旋，第一个参数为右旋点  
				break;
			}		
		}	//以上便是一个完整的插入修复操作流程
			//case4、case5都是针对case3插入节点以后，进行的一系列插入修复情况操作，不过，指向当前节点的指针一直在变化
		else {		// 如果父节点是右子节点，对称处理
			auto uncle = x->parent->parent->left;	//找出叔叔节点
			if (uncle != nullptr && rb_tree_is_red(uncle)) {	// 有叔叔节点，且为红  
				// case 3: 父节点和叔叔节点都为红
				rb_tree_set_black(x->parent);		 // 更改父节点为黑色  
				rb_tree_set_black(uncle);			 // 更改叔叔节点为黑色  
				x = x->parent - parent;				// 更改祖父节点为红色  
				rb_tree_set_red(x);
				// 此时祖父节点为红，可能会破坏红黑树的性质，令当前节点为祖父节点，继续处理
			}
			else {	// 无叔叔节点或叔叔节点为黑
				if (rb_tree_is_child(x)) {
					// case 4: 当前节点 x 为左子节点
					x = x->parent;					//当前节点的父节点做为新的当前节点
					rb_tree_rotate_right(x, root);	 //以新当前节点为支点右旋，第一个参数为右旋点  
				}
				// 都转换成 case 5： 当前节点为左子节点
				rb_tree_set_black(x->parent);		//更改颜色
				rb_tree_set_red(x->parent->parent);
				rb_tree_rotate_left(x->parent->parent, root);	 // 第一个参数为左旋点  
				break;
			}
		}
	}
	rb_tree_set_black(root);	// 根节点永远为黑
}

// 删除节点后使 rb tree 重新平衡，参数一为要删除的节点，参数二为根节点，参数三为最小节点，参数四为最大节点
// 
// 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153		删除主要参考这篇的四种情况
template <class NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) {
	// y 是可能的替换节点，指向最终要删除的节点
	auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
	// x 是 y 的一个独子节点或 NIL 节点
	auto x = y->left != nullptr ? y->left : y->right;
	// xp 为 x 的父节点
	NodePtr xp = nullptr;

	// y != z 说明 z 有两个非空子节点，此时 y 指向 z 右子树的最左节点，x 指向 y 的右子节点。
	// 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z
	if (y != z) {
		z->left->parent = y;
		y->left = z->left;

		// 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子
		if (y != z->right) {
			// x 替换 y 的位置
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

		// 连接 y 与 z 的父节点 
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
	// y == z 说明 z 至多只有一个孩子
	else {
		xp = y->parent;
		if (x)
			x->parent = y->parent;

		// 连接 x 与 z 的父节点
		if (root == z)
			root = x;
		else if (rb_tree_is_lchild(z))
			z->parent->left = x;
		else
			z->parent->right = x;

		// 此时 z 有可能是最左节点或最右节点，更新数据
		if (leftmost == z)
			leftmost = x = nullptr ? xp : rb_tree_min(x);
		if (rightmost == z)
			rightmost = x = nullptr ? xp : rb_tree_max(x);
	}

	// 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
	// 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
	// case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
	// case 2: 兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理
	// case 3: 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
	//         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
	// case 4: 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
	//         为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
	
	if (!rb_tree_is_red(y)) {
		// x 为黑色时，调整，否则直接将 x 变为黑色即可
		while (x != root && (x == nullptr || !rb_tree_is_red(x))) {
			if (x == xp->left) {
				// 如果 x 为左子节点
				auto brother = xp->right;	
				if (rb_tree_is_red(borther)) {		
					// case 1：x的兄弟brother为红色
					rb_tree_set_black(borther);		//将x的兄弟设为黑色
					rb_tree_set_red(xp);			//把x的父节点设为红
					rb_tree_rotate_left(xp,root);	//以x的父节点为支点进行左旋
					brother = xp->right;			//x的新兄弟为旋转之前brother的某个孩子，为黑色
				}
				// case 1 转为为了 case 2、3、4 中的一种
				if (brother->left == nullptr || !rb_tree_is_red(brother->left) &&
					(brother->right == nullptr || !rb_tree_is_red(brother->right))) {
					// case 2：x的兄弟brother是黑色的，且brother的两个孩子也是黑色
					//因为brother也是黑色的，所以x和brother中得去掉一黑色，最后，brother变为红。
					rb_tree_set_red(brother);	//将brother设为红
					x = xp;						//x的父节点成为新的x
					xp = xp->parent;			//x的父节点的父节点成为新的父节点
				}
				else {
					if (brother->right == nullptr || !rb_tree_is_red(brother->right)) {
						// case 3：x的兄弟brother是黑色的，brother的左孩子是红色，brother的右孩子是黑色
						if (brother->left != nullptr)
							rb_tree_set_black(brother->left);	//brother的左孩子设为黑
						rb_tree_set_red(brother);				//brother设为红
						rb_tree_rotate_right(brother, root);	//以brother为支点进行右旋
						brother = xp->right;		//此时x的新兄弟为x父节点的右孩子，是一个有红色右孩子的黑结点
					}
					// 于是case 3 转为 case 4
					//case 4：x的兄弟brother是黑色的，且brother的右孩子时红色的
					brother->color = xp->color;		//将brother的颜色设为父节点的颜色，即红色
					rb_tree_set_black(xp);			//将父节点设为黑色
					if (brother->right != nullptr)
						rb_tree_set_black(brother->right);	//将brother的右孩子设为黑色
					rb_tree_rotate_left(xp, root);	//以父节点为支点进行左旋
					break;
				}
			}
			else {	// x 为右子节点，对称处理
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
					// 转为 case 4
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
			rb_tree_set_black(x);		//将x设为黑
	}
	return y;
}

// 模板类 rb_tree
// 参数一代表数据类型，参数二代表键值比较类型
template <class T, class Compare>class rb_tree {		//class Compare声明如何比较大小
public:
	// rb_tree 的嵌套型别定义 
	typedef rb_tree_traits<T>                        tree_traits;
	typedef rb_tree_value_traits<T>                  value_traits;

	//萃取的相应型别
	typedef typename tree_traits::base_type          base_type;
	typedef typename tree_traits::base_ptr           base_ptr;
	typedef typename tree_traits::node_type          node_type;
	typedef typename tree_traits::node_ptr           node_ptr;
	typedef typename tree_traits::key_type           key_type;
	typedef typename tree_traits::mapped_type        mapped_type;
	typedef typename tree_traits::value_type         value_type;
	typedef Compare                                  key_compare;	//告诉rb_tree如何比较大小

	//内存配置型别
	typedef mystl::allocator<T>                      allocator_type;
	typedef mystl::allocator<T>                      data_allocator;
	typedef mystl::allocator<base_type>              base_allocator;
	typedef mystl::allocator<node_type>              node_allocator;	//节点配置类型

	//内嵌型别
	typedef typename allocator_type::pointer         pointer;
	typedef typename allocator_type::const_pointer   const_pointer;
	typedef typename allocator_type::reference       reference;
	typedef typename allocator_type::const_reference const_reference;
	typedef typename allocator_type::size_type       size_type;
	typedef typename allocator_type::difference_type difference_type;

	//迭代器型别
	typedef rb_tree_iterator<T>                      iterator;
	typedef rb_tree_const_iterator<T>                const_iterator;
	typedef mystl::reverse_iterator<iterator>        reverse_iterator;
	typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;

	allocator_type get_allocator() const { return node_allocator(); }	//调用 mystl::allocator<node_type>
	key_compare key_comp()const { return key_comp_; }		//调用比较算法

private:
	base_ptr header_;		//特殊节点,与根节点互为对方的父节点
	size_type node_count_;	//节点数
	key_compare key_comp_;	//节点键值比较的准则

private:
	// 以下三个函数用于取得根节点，最小节点和最大节点
	base_ptr& root() const { return header_->parent; }
	base_ptr& leftmost()const { return header_ - left; }
	base_ptr& rightmost() const { return header_->right; }

public:
	// 构造、复制、析构函数
	rb_tree() { rb_tree_init(); }

	rb_tree(const rb_tree& rhs);
	rb_tree(rb_tree&& rhs) noexcept;

	rb_tree& operator=(const rb_tree& rhs);
	rb_tree& operator=(rb_tree&& rhs);

	~rb_tree() { clear(); }

public:
	// 迭代器相关操作
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

	//反向迭代器
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

	//const迭代器
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

	// 容量相关操作
	bool empty()const noexcept { return node_count_ == 0; }
	size_type size()     const noexcept { return node_count_; }
	//npos是一个常数，表示size_t的最大值
	//许多容器都提供这个东西，用来表示不存在的位置，类型一般是std::container_type::size_type
	//static const size_type npos = -1相当于static_cast<size_type>(-1),注意这里的size_type是allocator决定的无符号数
	size_type max_size() const noexcept { return static_cast<size_type>(-1); }

	// 插入删除相关操作

	// emplace的声明

	template <class ...Args>
	iterator  emplace_multi(Args&& ...args);

	template <class ...Args>
	mystl::pair<iterator, bool> emplace_unique(Args&& ...args);

	template <class ...Args>
	iterator  emplace_multi_use_hint(iterator hint, Args&& ...args);

	template <class ...Args>
	iterator  emplace_unique_use_hint(iterator hint, Args&& ...args);

	// 允许重复元素的insert，用于multi容器
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
		size_type n = mystl::distance(first, last);		//获取[first,last]距离
		//节点个数>最大值-n就抛出异常
		THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
		for (; n > 0; --n, ++first)		//遍历插入元素
			insert_multi(end(), *first);
	}

	//返回pair类型的插入
	mystl::pair<iterator, bool> insert_unique(const value_type& value);
	mystl::pair<iterator, bool> insert_unique(value_type&& value)
	{
		return emplace_unique(mystl::move(value));
	}

	//不允许重复值的插入
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

	size_type erase_multi(const key_type& key);		//允许重复元素值的清除
	size_type erase_unique(const key_type& key);	//不允许重复元素值的清除

	void      erase(iterator first, iterator last);

	//clear清空
	void      clear();		

	// rb_tree 相关操作

	//find操作
	iterator       find(const key_type& key);
	const_iterator find(const key_type& key) const;

	//统计重复元素个数
	size_type count_multi(const key_type& key)const {	
		auto p = equal_range_multi(key);		//根据key查找
		return static_cast<size_type>(mystl::distance(p.first, p.second));
	}
	//统计不重复元素个数
	size_type      count_unique(const key_type& key) const
	{
		return find(key) != end() ? 1 : 0;	//要么存在，要么为0
	}

	//在first和last的前闭后开的区间中进行二分查找第一个不小于x的值
	iterator       lower_bound(const key_type& key);  
	const_iterator lower_bound(const key_type& key) const;

	//在first和last的前闭后开的区间中进行二分查找第一个大于x的值
	iterator       upper_bound(const key_type& key);
	const_iterator upper_bound(const key_type& key) const;

	//查找某个重复元素个数操作的辅助函数
	mystl::pair<iterator, iterator>
		equal_range_multi(const key_type& key) {
		return mystl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
	}
	mystl::pair<const_iterator, const_iterator>
		equal_range_multi(const key_type& key) const
	{
		return mystl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
	}

	//查找某个非重复元素操作的辅助函数
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

	//swap函数
	void swap(rb_tree& rhs) noexcept;

private:
	// node related节点的创建、克隆、销毁操作
	template <class ...Args>
	node_ptr create_node(Args&&... args);
	node_ptr clone_node(base_ptr x);
	void     destroy_node(node_ptr p);

	// init / reset 节点的初始化、重置操作
	void     rb_tree_init();
	void     reset();

	// get insert pos 获取插入元素的操作
	mystl::pair<base_ptr, bool>
		get_insert_multi_pos(const key_type& key);
	mystl::pair<mystl::pair<base_ptr, bool>, bool>
		get_insert_unique_pos(const key_type& key);

	// insert value / insert node	//插入value与插入节点操作
	iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left);
	iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

	// insert use hint	插入使用提示操作
	iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);
	iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

	// copy tree / erase tree	拷贝树与清除树
	base_ptr copy_from(base_ptr x, base_ptr p);
	void     erase_since(base_ptr x);
};

/*****************************************************************************************/

// 复制构造函数
template <class T, class Compare>
rb_tree<T, Compare>::
rb_tree(const rb_tree& rhs) {
	rb_tree_init();		//rb_tree的初始化
	if (rhs.node_count_ != 0) {	//节点数量不为空
		root() = copy_from(rhs.root, header);
		leftmost = rb_tree_min(root());
		rightmost = rb_tree_max(root());
	}
	node_count_ = rhs.node_count_;
	key_comp_ = rhs.key_comp_;
}

// 移动构造函数
template <class T, class Compare>
rb_tree<T, Compare>::
rb_tree(rb_tree&& rhs) noexcept
	:header_(mystl::move(rhs.header_)), node_count_(rgs.node_count_), key_comp_(rhs.key_comp_) {
	rhs.reset;	//移动完毕后将rhs重置
}

// 复制赋值操作符
template <class T, class Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::
operator=(const rb_tree& rhs)
{
	if (this != &rhs)	//跳出自我赋值的情况
	{
		clear();		//清除自身空间

		if (rhs.node_count_ != 0)	//节点数不为空，进行拷贝赋值
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

// 移动赋值操作符
template <class T, class Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::
operator=(rb_tree&& rhs)
{
	clear();
	header_ = mystl::move(rhs.header_);
	node_count_ = rhs.node_count_;
	key_comp_ = rhs.key_comp_;
	rhs.reset();	//移动完毕后重置rhs
	return *this;
}

// 就地插入元素，键值允许重复
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
emplace_multi(Args&& ...args) {
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	node_ptr np = create_node(mystl::forward<Args>(args)...);	//使用forward()保证参数包的完整转发
	auto res = get_insert_multi_pos(value_traits::get_key(np->value));	//调用辅助函数
	return insert_node_at(res.first, np, res.second);
}

// 就地插入元素，键值不允许重复
template <class T, class Compare>
template <class ...Args>
mystl::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::
emplace_unique(Args&& ...args)
{
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	node_ptr np = create_node(mystl::forward<Args>(args)...);	//使用forward()保证参数包的完整转发
	auto res = get_insert_unique_pos(value_traits::get_key(np->value));	//调用辅助函数
	if (res.second)
	{ // 插入成功
		return mystl::make_pair(insert_node_at(res.first.first, np, res.first.second), true);	//建立相应的pair
	}
	destroy_node(np);	//插入不成功就全部销毁，并返回make_pair为false
	return mystl::make_pair(iterator(res.first.first), false);	
}

// 就地插入元素，键值允许重复，当 hint 位置与插入位置接近时，插入操作的时间复杂度可以降低
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
emplace_multi_use_hint(iterator hint, Args&& ...args) {		//hint为插入元素
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	node_ptr np = create_node(mystl::forward<Args>(args)...);  //使用forward()保证参数包的完整转发
	if (node_count_ == 0) {	//节点个数为0，插入到header之下
		return insert_node_at(header_, np, true);	
	}
	key_type key = value_traits::get_key(np->value);	//萃取key的型别
	if (hint == begin()) {	// 插入元素位于 begin 处
		if (key_comp_(key, value_traits::get_key(*hint))) {	//萃取插入元素型别
			return insert_node_at(hint.node, np, true);
		}
		else
		{
			auto pos = get_insert_multi_pos(key);		//获取相应pair
			return insert_node_at(pos.first, np, pos.second);
		}
	}
	else if (hint == end())
	{ // 位于 end 处
		if (!key_comp_(key, value_traits::get_key(rightmost()->get_node_ptr()->value)))	//比较相应型别
		{
			return insert_node_at(rightmost(), np, false);	//最右插入
		}
		else
		{
			auto pos = get_insert_multi_pos(key);
			return insert_node_at(pos.first, np, pos.second);
		}
	}
	return insert_multi_use_hint(hint, key, np);
}

// 就地插入元素，键值不允许重复，当 hint 位置与插入位置接近时，插入操作的时间复杂度可以降低
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
	{ // 位于 begin 处
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
	{ // 位于 end 处
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

// 插入元素，节点键值允许重复
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_multi(const value_type& value)
{
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	auto res = get_insert_multi_pos(value_traits::get_key(value));
	return insert_value_at(res.first, value, res.second);
}

// 插入新值，节点键值不允许重复，返回一个 pair，若插入成功，pair 的第二参数为 true，否则为 false
template <class T, class Compare>
mystl::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::
insert_unique(const value_type& value)
{
	THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
	auto res = get_insert_unique_pos(value_traits::get_key(value));
	if (res.second)
	{ // 插入成功
		return mystl::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
	}
	return mystl::make_pair(res.first.first, false);
}

// 删除 hint 位置的节点
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
erase(iterator hint) {
	auto node = hint.node->get_node_ptr();	//获取被删除元素的指针
	iterator next(node);	
	++next;

	rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());	//进行删除元素后的调整
	destroy_node(node);		//销毁元素
	--node_count_;
	return next;
}

// 删除键值等于 key 的元素，返回删除的个数
template <class T, class Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::
erase_multi(const key_type& key) {		//允许重复元素
	auto p = equal_range_multi(key);	//获取相应key
	size_type n = mystl::distance(p.first, p.last);	//查找键值等于 key 的元素个数
	erase(p.first, p.second);	//进行删除
	return n;
}

// 删除键值等于 key 的元素，返回删除的个数
template <class T, class Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::
erase_unique(const key_type& key) {		//不允许重复元素
	auto it = find(key);	//查找键值等于 key 的元素
	if (it != end()) {
		erase(it);	//进行删除
		return 1;
	}
	return 0;
}

// 删除[first, last)区间内的元素
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

// 清空 rb tree
template <class T, class Compare>
void rb_tree<T, Compare>::
clear()
{
	if (node_count_ != 0)
	{
		erase_since(root());
		leftmost() = header_;	//保留header
		root() = nullptr;
		rightmost() = header_;
		node_count_ = 0;
	}
}

// 查找键值为 k 的节点，返回指向它的迭代器
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
find(const key_type& key) {
	auto y = header_;		// 最后一个不小于 key 的节点
	auto x = root();		//当前节点
	while (x != nullptr) {
		if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {	// key_comp_是节点键值大小比较函数
			// key 小于等于 x 键值，向左走
			y = x, x = x->left;	
		}
		else {	// key 大于 x 键值，向右走
			x = x->right;
		}
	}
	iterator j = iterator(y);
	// y的键值不小于key，返回的时候需要判断与key是相等还是小于  
	return (j==end()|| key_comp_(key, value_traits::get_key(*j))) ? end() : j;	
}
//const迭代器版本
template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::
find(const key_type& key) const
{
	auto y = header_;  // 最后一个不小于 key 的节点
	auto x = root();
	while (x != nullptr)
	{
		if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key))
		{ // key 小于等于 x 键值，向左走
			y = x, x = x->left;
		}
		else
		{ // key 大于 x 键值，向右走
			x = x->right;
		}
	}
	const_iterator j = const_iterator(y);
	return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
}

// 键值不小于 key 的第一个位置
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
lower_bound(const key_type& key) {
	auto y = header_;	// 最后一个不小于 key 的节点
	auto x = root();
	while (x != nullptr) {
		if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
			// key 小于等于 x 键值，向左走
			y = x, x = x->left;
		}
		else {	// key 大于 x 键值，向右走
			x = x->rght;
		}
	}
	return iterator(y);
}
//const迭代器版本
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

// 键值不小于 key 的最后一个位置
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
upper_bound(const key_type& key)
{
	auto y = header_;	// 最后一个不小于 key 的节点
	auto x = root();
	while (x != nullptr)
	{
		if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value)))
		{ // key < x	 key 大于 x 键值，向左走
			y = x, x = x->left;
		}
		else	// key 小于等于 x 键值，向右走
		{
			x = x->right;
		}
	}
	return iterator(y);
}
//const迭代器版本
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

// 交换 rb tree
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

// 创建一个结点
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::
create_node(Args&&... args) {
	auto tmp = node_allocator::allocate(1);		//配置空间
	try {
		data_allocator::construct(mystl::address_of(tmp->value), mystl::forward<Args>(args)...); //构造内容
		tmp->left = nullptr;
		tmp->right = nullptr;
		tmp->parent = nullptr;
	}
	catch (...) {
		node_allocator::deallocate(tmp);	//构造失败就全部释放
		throw;
	}
	return tmp;
}

// 复制一个结点
template <class T, class Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::
clone_node(base_ptr x) {	//复制一个节点的值和颜色
	node_ptr tmp = create_node(x->get_node_ptr()->value);
	tmp->color = x->color;
	tmp->left = nullptr;
	tmp->right = nullptr;
	return tmp;
}

// 销毁一个结点
template <class T, class Compare>
void rb_tree<T, Compare>::
destroy_node(node_ptr p) {
	data_allocator::destroy(&p->value);		
	node_allocator::deallocate(p);
}

// 初始化容器
template <class T, class Compare>
void rb_tree<T, Compare>::
rb_tree_init() {
	header_= base_allocator::allocate(1);	//配置空间
	header_->color = rb_tree_red;	// header_ 节点颜色为红，与 root 区分
	root() = nullptr;	
	leftmost = header_;			//都指向header
	rightmost = header_;
	node_count_ = 0;
}

// reset 函数
template <class T, class Compare>
void rb_tree<T, Compare>::reset() {
	header_ = nullptr;
	node_count_ = 0;
}

// get_insert_multi_pos 函数，插入新值，允许节点键值重复
template <class T, class Compare>
mystl::pair<typename rb_tree<T, Compare>::base_ptr, bool>
rb_tree<T, Compare>::get_insert_multi_pos(const key_type& key) {
	auto x = root();
	auto y = header_;
	bool add_to_left = true;
	while (x != nullptr) {
		y = x;
		add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));	//比较key与value
		// x的value与key比较 如果x大于等于key的话就往左走，否则就往右边走
		x = add_to_left ? x->left : x->right;	
	}
	return mystl::make_pair(y, add_to_left);	
}

// get_insert_unique_pos 函数，插入新值，不允许重复
template <class T, class Compare>
mystl::pair<mystl::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>
rb_tree<T, Compare>::get_insert_unique_pos(const key_type& key) {
	// 返回一个 pair，第一个值为一个 pair，包含插入点的父节点和一个 bool 表示是否在左边插入，
	// 第二个值为一个 bool，表示是否插入成功
	auto x = root();
	auto y = header_;
	bool add_to_left = true;	// 树为空时也在 header_ 左边插入
	while (x != nullptr) {
		y = x;
		add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
		// x的value与key比较 如果x大于等于key的话就往左走，否则就往右边走
		x = add_to_left ? x->left : x->right;
	}
	iterator j = iterator(y);	// 此时 y 为插入点的父节点
	if (add_to_left) {
		if (y == header_ || j == begin()) {
			// 如果树为空树或插入点在最左节点处，肯定可以插入新的节点
			return mystl::make_pair(mystl::make_pair(y, true), true);
		}
		else {	// 否则，如果存在重复节点，那么 --j 就是重复的值,进行--j循环，不断测试
			--j;
		}
	}
	if (key_comp_(value_traits::get_key(*j), key)) {
		// 表明新节点没有重复,且小于新节点，将新节点插入右侧
		return mystl::make_pair(mystl::make_pair(y, add_to_left), true);
	}
	// 进行至此，表示新节点与现有节点键值重复
	return mystl::make_pair(mystl::make_pair(y, add_to_left), false);
}

// insert_value_at 函数，真正的插入执行程序
// x 为插入点的父节点， value 为要插入的值，add_to_left 表示是否在左边插入
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

// 在 x 节点处插入新的节点
// x 为插入点的父节点， node 为要插入的节点，add_to_left 表示是否在左边插入
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

// 插入元素，键值允许重复，使用 hint
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_multi_use_hint(iterator hint, key_type key, node_ptr node) {
	// 在 hint 附近寻找可插入的位置
	auto np = hint.node;
	auto before = hint;
	--before;
	auto bnp = before.node;
	if (!key_comp_(key, value_traits::get_key(*before)) && !key_comp_(value_traits::get_key(*hint), key)) {
		// before <= node <= hint
		if (bnp->right == nullptr) {
			return insert_node_at(bnp, node, false);	//右边插入
		}
		else if (np->left == nullptr) {
			return insert_node_at(np, node, true);		//左边插入
		}
	}
	auto pos=get_insert_multi_pos(key);					//插入重复元素
	return insert_node_at(pos.first, node, pos.second);
}

// 插入元素，键值不允许重复，使用 hint
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::
insert_unique_use_hint(iterator hint, key_type key, node_ptr node) {
	// 在 hint 附近寻找可插入的位置
	auto np = hint.node;
	auto before = hint;
	--before;
	auto bnp = before.node;
	if (key_comp_(value_traits::get_key(*before), key) &&
		key_comp_(key, value_traits::get_key(*hint)))
	{ // before < node < hint
		if (bnp->right == nullptr)
		{
			return insert_node_at(bnp, node, false);	//右边插入
		}
		else if (np->left == nullptr)
		{
			return insert_node_at(np, node, true);		//左边插入
		}
	}
	auto pos= get_insert_unique_pos(key);		//非重复元素插入
	if (!pos.second) {		//没有value就销毁节点
		destroy_node(node);
		return pos.first.first;
	}
	return insert_node_at(pos.first.first, node, pos.first.second);		//插入pos的下一元素
}

// copy_from 函数
// 递归复制一颗树，节点从 x 开始，p 为 x 的父节点
template <class T, class Compare>
typename rb_tree<T, Compare>::base_ptr
rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p) {
	auto top = clone_node(x);
	top->parent = p;
	try {
		if (x->right)
			top->right = copy_from(x->right, top);	//递归右子树
		p = top;
		x = x->left;	//进行左子树的递归
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

// erase_since 函数
// 从 x 节点开始删除该节点及其子树
template <class T, class Compare>
void rb_tree<T, Compare>::
erase_since(base_ptr x) {
	while (x!=nullptr)
	{
		erase_since(x->right);	//递归右子树
		auto y = x->left;
		destroy_node(x->get_node_ptr());
		x = y;
	}
}

// 重载比较操作符
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

// 重载 mystl 的 swap
template <class T, class Compare>
void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept
{
	lhs.swap(rhs);
}

}	//命名空间结束符

#endif // !MYSTL_RB_TREE_H_

