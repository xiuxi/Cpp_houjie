#ifndef MYSTL_HASHTABLE_H_
#define MYSTL_HASHTABLE_H_

// 这个头文件包含了一个模板类 hashtable
// hashtable : 哈希表，使用开链法处理冲突

#include <initializer_list>		//初始化列表

#include "algo.h"				//其他算法
#include "functional.h"			//仿函数与哈希函数
#include "memory.h"				//高级的动态内存管理
#include "vector.h"				//vector
#include "util.h"				//通用工具
#include "exceptdef.h"			//异常

namespace mystl {

// hashtable 的节点定义
template <class T> struct hashtable_node {
	hashtable_node* next;		// 指向下一个节点
	T				value;		// 储存实值

	//构造、拷贝、移动函数
	hashtable_node() = default;
	hashtable_node(const T& n):next(nullptr),value(n){}

	hashtable_node(const hashtable_node& node):node(node.next),value(node.value){}
	hashtable_node(hashtable_node&& node):next(node.next),value(mystl::move(node.value)){
		node.next = nullptr;
	}
};

// value traits 
//萃取容器的具体实现
template <class T, bool>
struct ht_value_traits_imp
{
	typedef T key_type;			//容器的key值
	typedef T mapped_type;		//容器的value值
	typedef T value_type;		//容器的pair值，即pair<const T,T>，表示具体值的型别

	template <class Ty>
	static const key_type& get_key(const Ty& value)			//获取key值
	{
		return value;
	}

	template <class Ty>
	static const value_type& get_value(const Ty& value)		//获取value值
	{
		return value;
	}
};

//萃取pair
template <class T>
struct ht_value_traits_imp<T, true> {
	//remove_cv提供与type相同的成员 typedef T，只是删除了其最顶层的 cv 限定符,即c（const）、v（volatile）限定符
	//以下参考博客：https://www.cnblogs.com/5iedu/p/7767937.html
					https://blog.csdn.net/janeqi1987/article/details/100049597
	typedef typename std::remove_cv<typename T::first_type>::type   key_type;		//pair的first，即key值
	typedef typename T::second_type									mapped_type;	//pair的second，即mapped值
	typedef T														value_type;		//pair类型，包含first与second

	template <class Ty>static const key_type& get_key(const Ty& value) {
		return value.first;		//获取pair的key值，即first
	}
	template <class Ty>static const value_type& get_value(const Ty& value) {
		return value;			//获取pair的value值
	}
};

//萃取当前值
template <class T>
struct ht_value_traits {
	static constexpr bool is_map = mystl::is_pair<T>::value;	//判断是否是map，即是否含有pair

	typedef ht_value_traits_imp<T, is_map>value_traits_type;	//萃取型别定义

	typedef typename value_traits_type::key_type		key_type;		//key值
	typedef typename value_traits_type::mapped_type		mapped_type;	//mapped值
	typedef typename value_traits_type::value_type		value_type;		//value值

	template <class Ty>static const key_type& get_key(const Ty& value) {
		return value_traits_type::get_key(value);		//调用萃取型别判断后获取key值
	}
	template <class Ty>static const key_type& get_value(const Ty& value) {
		return value_traits_type::get_value(value);		//调用萃取型别判断后获取value值
	}
};

// forward declaration	前置声明

template <class T, class HashFun, class KeyEqual>
class hashtable;

template <class T, class HashFun, class KeyEqual>
struct ht_iterator;

template <class T, class HashFun, class KeyEqual>
struct ht_const_iterator;

template <class T>
struct ht_local_iterator;

template <class T>
struct ht_const_local_iterator;

// ht_iterator
//基本迭代器，class Hash指定hash Function，class KeyEqual指定key值如何相等
template <class T, class Hash, class KeyEqual>struct ht_iterator_base :	
	public mystl::iterator<mystl::forward_iterator_tag, T> {

	typedef mystl::hashtable<T, Hash, KeyEqual>         hashtable;		//hashtable型别，保持与容器的关联
	typedef ht_iterator_base<T, Hash, KeyEqual>         base;			//自身型别
	typedef mystl::ht_iterator<T, Hash, KeyEqual>       iterator;		//迭代器型别
	typedef mystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
	typedef hashtable_node<T>*							node_ptr;		//节点型别
	typedef hashtable*									contain_ptr;	//指向hashtable的指针型别
	typedef const node_ptr                              const_node_ptr;
	typedef const contain_ptr                           const_contain_ptr;

	typedef size_t                                      size_type;			//大小型别
	typedef ptrdiff_t                                   difference_type;	//距离型别

	node_ptr	node;	// 迭代器当前所指节点
	contain_ptr ht;		// 指向hashtable，保持与容器的连结（因为可能需要从bucket跳到bucket）

	ht_iterator_base() = default;	//默认构造函数

	//重载运算符
	bool operator==(const base& rhs)const { return node == rhs.node; }
	bool operator!=(const base& rhs) const { return node != rhs.node; }
};

//正式的迭代器，继承基本迭代器
template <class T, class Hash, class KeyEqual>
struct ht_iterator :public ht_iterator_base<T, Hash, KeyEqual> {

	typedef ht_iterator_base<T, Hash, KeyEqual> base;			//父类型别
	typedef typename base::hashtable            hashtable;		
	typedef typename base::iterator             iterator;
	typedef typename base::const_iterator       const_iterator;
	typedef typename base::node_ptr             node_ptr;
	typedef typename base::contain_ptr          contain_ptr;

	typedef ht_value_traits<T>                  value_traits;	
	typedef T                                   value_type;
	typedef value_type*							pointer;
	typedef value_type&							reference;

	//using用于声明在子类中引用父类的成员
	using base::node;
	using base::ht;

	//构造、拷贝、赋值函数
	ht_iterator() = default;
	ht_iterator(node_ptr n, contain_ptr t) {
		node = n;
		ht = t;
	}
	ht_iterator(const iterator& rhs) {
		node = rhs.node;
		ht = rhs.ht;
	}
	ht_iterator(const const_iterator& rhs)
	{
		node = rhs.node;
		ht = rhs.ht;
	}
	iterator& operator=(const iterator& rhs) {
		if (this != &rhs) {
			node = rhs.node;
			ht = rhs.ht;
		}
		return *this;
	}
	iterator& operator=(const const_iterator& rhs)
	{
		if (this != &rhs)
		{
			node = rhs.node;
			ht = rhs.ht;
		}
		return *this;
	}

	// 重载操作符
	reference operator*()const { return node->value; }
	pointer operator->()const { return &(operator*()); }

	iterator& operator++()const {		//前置++
		MYSTL_DEBUG(node != nullptr);
		const node_ptr old = node;	//先保存当前指针
		node = node->next;			//前进一个位置，如果存在，说明在同个bucket内，即可返回
		if (node == nullptr) {		// 如果下一个位置为空，跳到下一个 bucket 的起始处
			auto index = ht->hash(value_traits::get_key(old->value));	//定位下一个bucket的位置
			while (!node && ++index < ht->bucket_size_) {		//如果没有超出bucket大小
				node = ht->buckets_[index];		//到达下一个bucket的起始处
			}
		}
		return *this;
	}
	iterator& operator++(int) {		//后置++
		iterator tmp = *this;
		++* this;	//调用前置++
		return tmp;
	}

	//hashtable的迭代器没有后退操作，hashtable也没有定义逆向迭代器
};

//const版本的迭代器设计
template <class T, class Hash, class KeyEqual>
struct ht_const_iterator :public ht_iterator_base<T, Hash, KeyEqual>
{
	typedef ht_iterator_base<T, Hash, KeyEqual> base;
	typedef typename base::hashtable            hashtable;
	typedef typename base::iterator             iterator;
	typedef typename base::const_iterator       const_iterator;
	typedef typename base::const_node_ptr       node_ptr;
	typedef typename base::const_contain_ptr    contain_ptr;

	typedef ht_value_traits<T>                  value_traits;
	typedef T                                   value_type;
	typedef const value_type* pointer;
	typedef const value_type& reference;

	using base::node;
	using base::ht;

	ht_const_iterator() = default;
	ht_const_iterator(node_ptr n, contain_ptr t)
	{
		node = n;
		ht = t;
	}
	ht_const_iterator(const iterator& rhs)
	{
		node = rhs.node;
		ht = rhs.ht;
	}
	ht_const_iterator(const const_iterator& rhs)
	{
		node = rhs.node;
		ht = rhs.ht;
	}
	const_iterator& operator=(const iterator& rhs)
	{
		if (this != &rhs)
		{
			node = rhs.node;
			ht = rhs.ht;
		}
		return *this;
	}
	const_iterator& operator=(const const_iterator& rhs)
	{
		if (this != &rhs)
		{
			node = rhs.node;
			ht = rhs.ht;
		}
		return *this;
	}

	// 重载操作符
	reference operator*()  const { return node->value; }
	pointer   operator->() const { return &(operator*()); }

	const_iterator& operator++()
	{
		MYSTL_DEBUG(node != nullptr);
		const node_ptr old = node;
		node = node->next;
		if (node == nullptr)
		{ // 如果下一个位置为空，跳到下一个 bucket 的起始处
			auto index = ht->hash(value_traits::get_key(old->value));
			while (!node && ++index < ht->bucket_size_)
			{
				node = ht->buckets_[index];
			}
		}
		return *this;
	}
	const_iterator operator++(int)
	{
		const_iterator tmp = *this;
		++* this;
		return tmp;
	}
};

// local iterator用于本bucket的迭代器
template <class T>
struct ht_local_iterator :public mystl::iterator<mystl::forward_iterator_tag, T> {
	//内嵌型别
	typedef T                          value_type;
	typedef value_type*				   pointer;
	typedef value_type&				   reference;
	typedef size_t                     size_type;
	typedef ptrdiff_t                  difference_type;
	typedef hashtable_node<T>*		   node_ptr;		//节点型别

	typedef ht_local_iterator<T>       self;
	typedef ht_local_iterator<T>       local_iterator;
	typedef ht_const_local_iterator<T> const_local_iterator;

	node_ptr node;	//指向当前节点

	//构造、拷贝函数
	ht_local_iterator(node_ptr n):node(n) {}
	ht_local_iterator(const local_iterator& rhs)
		:node(rhs.node)
	{
	}
	ht_local_iterator(const const_local_iterator& rhs)
		:node(rhs.node)
	{
	}

	//操作符重载
	reference operator*()  const { return node->value; }
	pointer   operator->() const { return &(operator*()); }

	self& operator++()
	{
		MYSTL_DEBUG(node != nullptr);
		node = node->next;
		return *this;
	}

	self operator++(int)
	{
		self tmp(*this);
		++* this;
		return tmp;
	}

	bool operator==(const self& other) const { return node == other.node; }
	bool operator!=(const self& other) const { return node != other.node; }
};

//const版本的本地迭代器
template <class T>
struct ht_const_local_iterator :public mystl::iterator<mystl::forward_iterator_tag, T>
{
	typedef T                          value_type;
	typedef const value_type* pointer;
	typedef const value_type& reference;
	typedef size_t                     size_type;
	typedef ptrdiff_t                  difference_type;
	typedef const hashtable_node<T>* node_ptr;

	typedef ht_const_local_iterator<T> self;
	typedef ht_local_iterator<T>       local_iterator;
	typedef ht_const_local_iterator<T> const_local_iterator;

	node_ptr node;

	ht_const_local_iterator(node_ptr n)
		:node(n)
	{
	}
	ht_const_local_iterator(const local_iterator& rhs)
		:node(rhs.node)
	{
	}
	ht_const_local_iterator(const const_local_iterator& rhs)
		:node(rhs.node)
	{
	}

	reference operator*()  const { return node->value; }
	pointer   operator->() const { return &(operator*()); }

	self& operator++()
	{
		MYSTL_DEBUG(node != nullptr);
		node = node->next;
		return *this;
	}

	self operator++(int)
	{
		self tmp(*this);
		++* this;
		return tmp;
	}

	bool operator==(const self& other) const { return node == other.node; }
	bool operator!=(const self& other) const { return node != other.node; }
};

// bucket 使用的大小

//如果是VC编译器或GUNC或clang编译器并且指针大小为8bytes
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1			//64位系统
#else
#define SYSTEM_32 1			//32位系统
#endif

#ifdef SYSTEM_64			//如果是64位系统

#define PRIME_NUM 99		//定义99个质数

//质数的列表与运算规则
// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 63 - 1)
static constexpr size_t ht_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
};

#else	//如果不是64位系统，而是32位系统

#define PRIME_NUM 44	//定义44个质数

// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 31 - 1)
static constexpr size_t ht_prime_list[] = {
  101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u,
  10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
  402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
  10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
  176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
  2006689337u, 3010034021u, 4294967291u,
};

#endif

//找出上述定义的质数之中，最接近并大于等于 n 的那个质数
inline size_t ht_next_prime(size_t n) {
	const size_t* first = ht_prime_list;
	const size_t* last = ht_prime_list + PRIME_NUM;
	//lower_bound是泛型算法，使用lower_bound()，序列需先排序
	const size_t* pos=mystl::lower_bound(first, last, n);
	return pos == last ? *(last - 1) : *pos;
}

// 模板类 hashtable
// 参数一代表数据类型，参数二代表哈希函数，参数三代表键值相等的比较函数
template <class T, class	Hash, class KeyEqual>class hashtable {
	//声明可访问hashtable的迭代器友元
	friend struct mystl::ht_iterator<T, Hash, KeyEqual>;
	friend struct mystl::ht_const_iterator<T, Hash, KeyEqual>;

public:
	// hashtable 的型别定义
	typedef ht_value_traits<T>                          value_traits;	
	typedef typename value_traits::key_type             key_type;			//key型别
	typedef typename value_traits::mapped_type          mapped_type;		//mapped型别
	typedef typename value_traits::value_type           value_type;			//pair的value型别
	typedef Hash                                        hasher;				//hashFunction型别
	typedef KeyEqual                                    key_equal;			//比较函数型别

	typedef hashtable_node<T>                           node_type;			//节点型别
	typedef node_type*									node_ptr;			//节点指针型别
	typedef mystl::vector<node_ptr>                     bucket_type;		//vector bucket型别

	typedef mystl::allocator<T>                         allocator_type;		//配置内存型别
	typedef mystl::allocator<T>                         data_allocator;
	typedef mystl::allocator<node_type>                 node_allocator;

	typedef typename allocator_type::pointer            pointer;			//内嵌型别
	typedef typename allocator_type::const_pointer      const_pointer;
	typedef typename allocator_type::reference          reference;
	typedef typename allocator_type::const_reference    const_reference;
	typedef typename allocator_type::size_type          size_type;			//容器大小型别
	typedef typename allocator_type::difference_type    difference_type;
		
	typedef mystl::ht_iterator<T, Hash, KeyEqual>       iterator;			//迭代器型别
	typedef mystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
	typedef mystl::ht_local_iterator<T>                 local_iterator;
	typedef mystl::ht_const_local_iterator<T>           const_local_iterator;

	allocator_type get_allocator() const { return allocator_type(); }	//获取相应内存型别

private:
	bucket_type buckets_;		//buckets，以vector完成
	size_type	bucket_size_;	//bucket大小
	size_type	size_;			//bucket vector的大小
	float		mlf_;			
	hasher		hash_;			//哈希函数
	key_equal	equal_;			//比较函数

private:

	//比较函数
	bool is_equal(const key_type& key1, const key_type& key2) {
		return equal_(key1, key2);
	}
	bool is_equal(const key_type& key1, const key_type& key2) const
	{
		return equal_(key1, key2);
	}

	//用于获取节点
	const_iterator M_cit(node_ptr node)const noexcept {
		//const_cast去除节点的const性质，若不能修改，则无意义
		return const_iterator(node, const_cast<hashtable*>(this));	
	}

	iterator M_begin()noexcept {
		for (size_type n = 0; n < bucket_size_; ++n) {
			if (buckets_[n])	 // 找到第一个有节点的位置就返回
				return iterator(buckets_[n], this);
		}
		return iterator(nullptr, this);
	}

	const_iterator M_begin()noexcept {
		for (size_type n = 0; n < bucket_size_; ++n) {
			if (buckets_[n])	// 找到第一个有节点的位置就返回
				return M_cit(buckets_[n]);
		}
		return M_cit(nullptr);
	}

public:
	// 构造、复制、移动、析构函数
	explicit hashtable(size_type bucket_count, const Hash& hash = hash(), const KeyEqual& euqal = KeyEqual())
		:size_(0), mlf_(1.0f), hash_(hash), equal_(equal) {
		init(bucket_count);		//调用init()
	}

	template <class Iter, typename std::enable_if<		//enable_if根据迭代器类型寻找最佳匹配函数
		mystl::is_input_iterator<Iter>::value, int>::type = 0>
		hashtable(Iter first, Iter last,
			size_type bucket_count,
			const Hash& hash = Hash(),
			const KeyEqual& equal = KeyEqual())
		: size_(mystl::distance(first, last)), mlf_(1.0f), hash_(hash), equal_(equal)
	{	//初始化最大值
		init(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))));
	}

	hashtable(const hashtable& rhs)					//拷贝构造
		:hash_(rhs.hash_), equal_(rhs.equal_)
	{
		copy_init(rhs);
	}

	hashtable(hashtable&& rhs) noexcept				//移动构造
		: bucket_size_(rhs.bucket_size_),
		size_(rhs.size_),
		mlf_(rhs.mlf_),
		hash_(rhs.hash_),
		equal_(rhs.equal_)
	{
		buckets_ = mystl::move(rhs.buckets_);
		rhs.bucket_size_ = 0;
		rhs.size_ = 0;
		rhs.mlf_ = 0.0f;
	}

	hashtable& operator=(const hashtable& rhs);			//赋值重载
	hashtable& operator=(hashtable&& rhs) noexcept;

	~hashtable() { clear(); }							//析构函数

	// 迭代器相关操作
	iterator       begin()        noexcept
	{
		return M_begin();
	}
	const_iterator begin()  const noexcept
	{
		return M_begin();
	}
	iterator       end()          noexcept
	{
		return iterator(nullptr, this);
	}
	const_iterator end()    const noexcept
	{
		return M_cit(nullptr);
	}

	const_iterator cbegin() const noexcept
	{
		return begin();
	}
	const_iterator cend()   const noexcept
	{
		return end();
	}

	// 容量相关操作
	bool      empty()    const noexcept { return size_ == 0; }
	size_type size()     const noexcept { return size_; }
	//npos是一个常数，表示size_t的最大值
	//许多容器都提供这个东西，用来表示不存在的位置，类型一般是std::container_type::size_type
	//static const size_type npos = -1相当于static_cast<size_type>(-1),注意这里的size_type是allocator决定的无符号数
	size_type max_size() const noexcept { return static_cast<size_type>(-1); }

	// 修改容器相关操作

	// emplace / empalce_hint

	template <class ...Args>
	iterator emplace_multi(Args&& ...args);					//允许重复元素的插入

	template <class ...Args>
	pair<iterator, bool> emplace_unique(Args&& ...args);	//不允许重复元素的插入

	// [note]: hint 对于 hash_table 其实没有意义，因为即使提供了 hint，也要做一次 hash，
	// 来确保 hash_table 的性质，所以选择忽略它
	template <class ...Args>
	iterator emplace_multi_use_hint(const_iterator /*hint*/, Args&& ...args)
	{
		return emplace_multi(mystl::forward<Args>(args)...);	//forward保证参数包的完整转发
	}

	template <class ...Args>
	iterator emplace_unique_use_hint(const_iterator /*hint*/, Args&& ...args)
	{
		return emplace_unique(mystl::forward<Args>(args)...).first;	//forward保证参数包的完整转发
	}

	// insert

	iterator             insert_multi_noresize(const value_type& value);	//允许重复元素的插入
	pair<iterator, bool> insert_unique_noresize(const value_type& value);	//不允许重复元素的插入

	//允许重复元素的插入
	iterator insert_multi(const value_type& value)	
	{
		rehash_if_need(1);		//调用rehash函数进行节点的重新分配
		return insert_multi_noresize(value);	//元素插入
	}
	iterator insert_multi(value_type&& value)
	{
		return emplace_multi(mystl::move(value));	//元素插入
	}

	//不允许重复元素的插入
	pair<iterator, bool> insert_unique(const value_type& value)
	{
		rehash_if_need(1);		//调用rehash函数进行bucket vector的重新分配
		return insert_unique_noresize(value);		//元素插入
	}
	pair<iterator, bool> insert_unique(value_type&& value)
	{
		return emplace_unique(mystl::move(value));	//元素插入
	}

	// [note]: 同 emplace_hint
	iterator insert_multi_use_hint(const_iterator /*hint*/, const value_type& value)
	{
		return insert_multi(value);
	}
	iterator insert_multi_use_hint(const_iterator /*hint*/, value_type&& value)
	{
		return emplace_multi(mystl::move(value));
	}

	iterator insert_unique_use_hint(const_iterator /*hint*/, const value_type& value)
	{
		return insert_unique(value).first;
	}
	iterator insert_unique_use_hint(const_iterator /*hint*/, value_type&& value)
	{
		return emplace_unique(mystl::move(value));
	}

	//插入迭代器区间
	template <class InputIter>
	void insert_multi(InputIter first, InputIter last)		//允许重复元素插入
	{
		copy_insert_multi(first, last, iterator_category(first));
	}

	template <class InputIter>
	void insert_unique(InputIter first, InputIter last)		//不允许重复元素插入
	{
		copy_insert_unique(first, last, iterator_category(first));
	}

	// erase / clear

	void      erase(const_iterator position);		//清除某个元素
	void      erase(const_iterator first, const_iterator last);		//清除一个区间

	size_type erase_multi(const key_type& key);		//允许重复元素清除
	size_type erase_unique(const key_type& key);	//不允许重复元素的清除

	void      clear();			//清除全部元素

	void      swap(hashtable& rhs) noexcept;	//交换元素

	// 查找相关操作

	size_type                            count(const key_type& key) const;	//查找与key值相等元素的个数

	iterator                             find(const key_type& key);
	const_iterator                       find(const key_type& key) const;

	//查找重复元素中与key相等的元素
	pair<iterator, iterator>             equal_range_multi(const key_type& key);
	pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const;

	//查找非重复元素中与key相等的元素
	pair<iterator, iterator>             equal_range_unique(const key_type& key);
	pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const;

	// bucket interface
	//获取某个bucket的起始位置
	local_iterator       begin(size_type n)        noexcept
	{
		MYSTL_DEBUG(n < size_);
		return buckets_[n];
	}
	const_local_iterator begin(size_type n)  const noexcept
	{
		MYSTL_DEBUG(n < size_);
		return buckets_[n];
	}
	const_local_iterator cbegin(size_type n) const noexcept
	{
		MYSTL_DEBUG(n < size_);
		return buckets_[n];
	}
	
	//获取某个bucket的末尾，即为空
	local_iterator       end(size_type n)          noexcept
	{
		MYSTL_DEBUG(n < size_);
		return nullptr;
	}
	const_local_iterator end(size_type n)    const noexcept
	{
		MYSTL_DEBUG(n < size_);
		return nullptr;
	}
	const_local_iterator cend(size_type n)   const noexcept
	{
		MYSTL_DEBUG(n < size_);
		return nullptr;
	}

	//获取bucket的个数，即bucket vector 的大小
	size_type bucket_count() const noexcept { return bucket_size_; }

	//获取bucket的最大值，即质数列表中的最大质数-1(下标从0开始)
	size_type max_bucket_count() const noexcept {
		return ht_prime_list[PRIME_NUM - 1];
	}

	size_type bucket_size(size_type n)       const noexcept;	//在某个 bucket 节点的个数
	size_type bucket(const key_type& key)    const
	{
		return hash(key);	//调用哈希函数
	}

	// hash policy哈希策略
	float load_factor() const noexcept	//加载元素
	{
		//bucket_size不为0，返回容器大小/bucket大小，否则为0
		return bucket_size_ != 0 ? (float)size_ / bucket_size_ : 0.0f;
	}
	float max_load_factor() const noexcept	//最大加载因子
	{
		return mlf_;
	}
	void max_load_factor(float ml)
	{	//抛出异常"无效的哈希负载因子"
		THROW_OUT_OF_RANGE_IF(ml != ml || ml < 0, "invalid hash load factor");
		mlf_ = ml;
	}

	void rehash(size_type count);		//重建哈希计算各节点应落在哪个位置

	void reserve(size_type count)		//重建bucket vector
	{
		rehash(static_cast<size_type>((float)count / max_load_factor() + 0.5f));
	}

	hasher    hash_fcn() const { return hash_; }	//哈希函数
	key_equal key_eq()   const { return equal_; }	//比较函数

private:
	// hashtable 成员函数
	// init
	void      init(size_type n);
	void      copy_init(const hashtable& ht);

	// node
	template  <class ...Args>
	node_ptr  create_node(Args&& ...args);	//创建节点
	void      destroy_node(node_ptr n);		//销毁节点

	// hash
	size_type next_size(size_type n) const;	//下一节点大小
	size_type hash(const key_type& key, size_type n) const;		//hash函数
	size_type hash(const key_type& key) const;
	void      rehash_if_need(size_type n);	//重新hash

	// insert
	//允许相同元素
	template <class InputIter>
	void copy_insert_multi(InputIter first, InputIter last, mystl::input_iterator_tag);
	template <class ForwardIter>
	void copy_insert_multi(ForwardIter first, ForwardIter last, mystl::forward_iterator_tag);
	//不允许相同元素
	template <class InputIter>
	void copy_insert_unique(InputIter first, InputIter last, mystl::input_iterator_tag);
	template <class ForwardIter>
	void copy_insert_unique(ForwardIter first, ForwardIter last, mystl::forward_iterator_tag);
	
	// insert node
	pair<iterator, bool> insert_node_unique(node_ptr np);	//不允许相同元素
	iterator             insert_node_multi(node_ptr np);	//允许相同元素

	 // bucket operator
	void replace_bucket(size_type bucket_count);			//bucket的重置
	void erase_bucket(size_type n, node_ptr first, node_ptr last);	//bucket的清除
	void erase_bucket(size_type n, node_ptr last);

	// comparision 比较函数
	bool equal_to_multi(const hashtable& other);
	bool equal_to_unique(const hashtable& other);
};

/*****************************************************************************************/

// 复制赋值运算符
template <class T, class Hash, class KeyEqual>
hashtable<T, Hash, KeyEqual>&
hashtable<T, Hash, KeyEqual>::
operator=(const hashtable& rhs) {
	if (this != rhs) {
		hashtable tmp(rhs);
		swap(tmp);	//交换this与tmp
	}
	return *this;
}

// 移动赋值运算符
template <class T, class Hash, class KeyEqual>
hashtable<T, Hash, KeyEqual>&
hashtable<T, Hash, KeyEqual>::
operator=(hashtable&& rhs) noexcept {
	hashtable tmp(mystl::move(rhs));
	swap(tmp);
	return *this;
}

// 就地构造元素，键值允许重复
// 强异常安全保证
template <class T, class Hash, class KeyEqual>
template <class ...Args>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
emplace_multi(Args&& ...args) {
	auto np = create_node(mystl::forward<Args>(args)...);	//forward保证参数安全转发
	try {
		//拿元素个数(把新增元素计入后)和bucket vector大小对比，如果前者大于后者，就重建bucket vector
		if ((float)(size + 1) > (float)bucket_size_ * max_load_factor())	
			rehash(size_ + 1);
	}
	catch (...) {
		destroy_node(np);	//失败就销毁元素
		throw;
	}
	return insert_node_multi(np);	//重建完毕，插入元素，允许重复元素
}

// 就地构造元素，键值不允许重复
// 强异常安全保证
template <class T, class Hash, class KeyEqual>
template <class ...Args>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::
emplace_unique(Args&& ...args)
{
	auto np = create_node(mystl::forward<Args>(args)...);
	try
	{
		if ((float)(size_ + 1) > (float)bucket_size_ * max_load_factor())
			rehash(size_ + 1);
	}
	catch (...)
	{
		destroy_node(np);
		throw;
	}
	return insert_node_unique(np);
}

// 在不需要重建表格的情况下插入新节点，键值不允许重复
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::
insert_unique_noresize(const value_type& value) {
	const auto n = hash(value_traits::get_key(value));	//决定value应位于第n个bucket
	auto first = buckets_[n];	//令first指向bucket对应之链表头部
	//如果bucket[n]已被占用，此时first不为0，于是进入循环，走过bucket所对应之整个链表
	for (auto cur = first; cur; cur = cur->next) {
		if(is_equal(value_traits::get_key(cur->value), value_traits::get_key(value)))
			//如果发现与链表中的某键值相同，就不插入，立刻返回
			return mystl::make_pair(iterator(cur, this), false);
	}
	// 离开以上循环(或根本未进入循环)，让first指向bucket所指链表的头部节点
	auto tmp = create_node(value);
	tmp->next = first;
	buckets_[n] = tmp;		//令新节点成为链表的第一个节点
	++size_;				//节点个数累加1
	return mystl::make_pair(iterator(tmp, this), true);		//返回插入成功
}

// 在不需要重建表格的情况下插入新节点，键值允许重复
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
insert_multi_noresize(const value_type& value) {
	const auto n = hash(value_traits::get_key(value));		//决定value应位于第n个bucket
	auto first = buckets_[n];		//令first指向bucket对应之链表头部
	auto tmp = create_node(value);	//产生新节点
	//如果bucket[n]已被占用，此时first不为0，于是进入循环，走过bucket所对应之整个链表
	for (auto cur = first; cur; cur = cur->next) {
		if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value))) {
			// 如果链表中存在相同键值的节点就马上插入，然后返回
			tmp->next = cur->next;	//将新节点插于目前位置
			cur->next = tmp;		
			++size_;				//节点个数累加1
			return	iterator(tmp, this);	//返回一个迭代器，指向新增节点
		}	
	}
	// 进行至此，表示没有发现重复的键值，于是插入在链表头部
	tmp->next = first;	//将新节点插入于链表头部
	buckets_[n] = tmp;
	++size_;			//节点个数累加1
	return iterator(tmp, this);	// 返回一个迭代器，指向新增节点
}

// 删除迭代器所指的节点
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase(const_iterator position) {
	auto p = position.node;
	if (p) {
		const auto n = hash(value_traits::get_key(p->value));	//决定value应位于第n个bucket
		auto cur = buckets_[n];			//令cur指向bucket对应之链表头部
		if (cur == p) {					// p 位于链表头部
			buckets_[n] = cur->next;	//令bucket[n]成为当前元素的下一个
			destroy_node(next);			//销毁要删除的元素
			--size_;					//节点个数累减1
			break;
		}
		else {	//不为链表头部时
			auto next = cur->next;		//向后查找
			while (next)
			{
				if (next == p) {		//当查找要被删除的元素，进行删除
					cur->next = next->next;
					destroy_node(next);
					--size_;
					break;
				}
				else {				//不断向后查找
					cur = next;
					next = cur->next;
				}
			}
		}
		
	}
}

// 删除[first, last)内的节点
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase(const_iterator first, const_iterator last) {
	if (first.node == last.node)	//判断first与last是否同一节点
		return;
	//获取first所在bucket
	auto first_bucket = first.node	
		? hash(value_traits::get_key(first.node->value))
		: bucket_size_;
	//获取last所在bucket
	auto last_bucket = last.node
		? hash(value_traits::get_key(last.node->value))
		: bucket_size_;
	if (first_bucket == last_bucket) {
		// 如果在 bucket 的同一个位置
		erase_bucket(first_bucket, first.node, last.node);
	}
	else {
		erase_bucket(first_bucket, first.node, nullptr);	//删除first bucket所在元素
		for (auto n = first_bucket + 1; n < last_bucket; ++n) {	//遍历[first，last]进行删除
			if (buckets_[n] != nullptr)
				erase_bucket(n, nullptr);
		}
		if (last_bucket != bucket_size_) {		//删除last bucket
			erase_bucket(last_bucket_last.node);
		}
	}
}

// 删除键值为 key 的节点，允许键值重复
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
erase_multi(const key_type& key) {
	auto p=equal_range_multi(key);	//获取与键值key相等的节点
	if (p.first.node != nullptr) {
		erase(p.first, p.second);	//进行删除
		return mystl::distance(p.first, p.second);
	}
	return 0;
}

// 删除键值为 key 的节点，不允许元素重复
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
erase_unique(const key_type& key) {
	const auto n = hash(key);		//哈希计算key所在的bucket
	auto first = buckets_[n];		//令first指向bucket的起始处
	if (first) {
		if (is_equal(value_traits::get_key(first->value), key)) {	
			//如果查找到与key值相等的元素，就进行删除
			buckets_[n] = first->next;
			destroy_node(first);
			--size_;
			return 1;
		}
		else {	//如果该bucket查找不到，就跳到下一个bucket进行查找
			auto next = first->next;
			while (next)
			{
				if (is_equal(value_traits::get_key(next->value), key)) {
					//如果查找到与key值相等的元素，就进行删除
					first->next = next->next;
					destroy_node(next);
					--size_;
					return 1;
				}
				//查找不到就继续跳跃到下一个bucket
				first = next;
				next = first->next;
			}
		}
	}
	return 0;
}

// clear 清空 hashtable
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::clear() {
	if (size_ != 0) {
		//针对每个bucket
		for (size_type i = 0; i < bucket_size_; ++i) {
			node_ptr cur = buckets_[i];
			//将bucket list中的每个节点删除
			while (cur!=nullptr)
			{
				node_ptr next = cur->next;
				destroy_node(cur);
				cur = next;
			}
			buckets_[i] = nullptr;	//令bucket内容为nullptr
		}
		size_ = 0;	//令总节点个数为0
	}
	//注意：bucket vector并未释放掉空间，仍保有原来大小
}

// 在某个 bucket 节点的个数
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
bucket_size(size_type n) const noexcept {
	size_type result = 0;
	for (auto cur = buckets_[n]; cur = cur->next) {		//遍历bucket
		++result;	//不断累加
	}
	return result;
}

// 重新对元素进行一遍哈希，插入到新的位置
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
rehash(size_type count) {
	auto n = ht_next_prime(count);	//n为下一个比count大的质数
	if (n > bucket_size_) {			//如果n大于当前bucket_size就进行bucket vector的重置
		replace_bucket(n);
	}
	else {
		if ((float)size_ / (float)n < max_load_factor() - 0.25f &&
			(float)n < (float)bucket_size_ * 0.75)  // worth rehash
		{	//如果值得重新哈希，也同样进行bucket vector的重置
			replace_bucket(n);
		}

	}
}

// 查找键值为 key 的节点，返回其迭代器
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
find(const key_type& key) {
	const auto n = hash(key);		//哈希计算key要落在哪个bucket
	node_ptr first = buckets_[n];	//令first指向bucket起始处
	for(; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {}	//进行查找
	return iterator(first, this);	//返回指向first的迭代器
}

// 查找键值为 key 的节点，返回其迭代器，const版本
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::const_iterator
hashtable<T, Hash, KeyEqual>::
find(const key_type& key) const {	
	const auto n = hash(key);		//哈希计算key要落在哪个bucket
	node_ptr first = buckets_[n];	//令first指向bucket起始处
	for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {} //进行查找
	return M_cit(first);	//返回去除了const属性的迭代器
}

// 查找键值为 key 出现的次数
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
count(const key_type& key) const {
	const auto n = hash(key);		//哈希计算key要落在哪个bucket
	size_type result = 0;
	for (node_ptr cur = buckets_[n]; cur; cur = cur->next) {	//遍历bucket
		if (is_equal(value_traits::get_key(cur->value), key))	//若找到与键值为key相同的元素
			++result;	//进行累加
	}
	return result;
}

// 查找与键值 key 相等的区间，返回一个 pair，指向相等区间的首尾，允许元素重复
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator,
	typename hashtable<T, Hash, KeyEqual>::iterator>
	hashtable<T, Hash, KeyEqual>::
	equal_range_multi(const key_type& key) {
	const auto n = hash(key);	//哈希计算key要落在哪个bucket
	for (node_ptr first = buckets_[n]; first; first = first->next) {	//遍历bucket
		if (is_equal(value_traits::get_key(first->value), key)) {
			// 如果出现相等的键值
			for (node_ptr second = first->next; second; second = second->next) {
				if(!is_equal(value_traits::get_key(second->value), key))
					// 如果出现相等的value值
					return mystl::make_pair(iterator(first, this), iterator(second, this));
			}
			for (auto m = n + 1; m < bucket_size_; ++m) {
				// 整个链表都相等，查找下一个链表出现的位置
				if(buckets_[m])
					return mystl::make_pair(iterator(first, this), iterator(buckets_[m], this));
			}
			return mystl::make_pair(iterator(first, this), end());
		}
	}
	return mystl::make_pair(end(), end());	//查找不到，返回末尾迭代器
}

//查找与键值 key 相等的区间，返回一个 pair，指向相等区间的首尾，允许元素重复，const版本
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::const_iterator,
	typename hashtable<T, Hash, KeyEqual>::const_iterator>
	hashtable<T, Hash, KeyEqual>::
	equal_range_multi(const key_type& key) const {
	const auto n = hash(key);
	for (node_ptr first = buckets_[n]; first; first = first->next)
	{
		if (is_equal(value_traits::get_key(first->value), key))
		{
			for (node_ptr second = first->next; second; second = second->next)
			{
				if (!is_equal(value_traits::get_key(second->value), key))
					return mystl::make_pair(M_cit(first), M_cit(second));
			}
			for (auto m = n + 1; m < bucket_size_; ++m)
			{ // 整个链表都相等，查找下一个链表出现的位置
				if (buckets_[m])
					return mystl::make_pair(M_cit(first), M_cit(buckets_[m]));
			}
			return mystl::make_pair(M_cit(first), cend());
		}
	}
	return mystl::make_pair(cend(), cend());
}

//查找与键值 key 相等的区间，返回一个 pair，指向相等区间的首尾，不允许元素重复
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator,
	typename hashtable<T, Hash, KeyEqual>::iterator>
	hashtable<T, Hash, KeyEqual>::
	equal_range_unique(const key_type& key)
{
	const auto n = hash(key);
	for (node_ptr first = buckets_[n]; first; first = first->next)
	{
		if (is_equal(value_traits::get_key(first->value), key))
		{
			if (first->next)	//使用next指针进行遍历
				return mystl::make_pair(iterator(first, this), iterator(first->next, this));
			for (auto m = n + 1; m < bucket_size_; ++m)
			{ // 整个链表都相等，查找下一个链表出现的位置
				if (buckets_[m])
					return mystl::make_pair(iterator(first, this), iterator(buckets_[m], this));
			}
			return mystl::make_pair(iterator(first, this), end());
		}
	}
	return mystl::make_pair(end(), end());
}

//查找与键值 key 相等的区间，返回一个 pair，指向相等区间的首尾，不允许元素重复，const版本
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::const_iterator,
	typename hashtable<T, Hash, KeyEqual>::const_iterator>
	hashtable<T, Hash, KeyEqual>::
	equal_range_unique(const key_type& key) const
{
	const auto n = hash(key);
	for (node_ptr first = buckets_[n]; first; first = first->next)
	{
		if (is_equal(value_traits::get_key(first->value), key))
		{
			if (first->next)
				return mystl::make_pair(M_cit(first), M_cit(first->next));
			for (auto m = n + 1; m < bucket_size_; ++m)
			{ // 整个链表都相等，查找下一个链表出现的位置
				if (buckets_[m])
					return mystl::make_pair(M_cit(first), M_cit(buckets_[m]));
			}
			return mystl::make_pair(M_cit(first), cend());
		}
	}
	return mystl::make_pair(cend(), cend());
}

// 交换 hashtable
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
swap(hashtable& rhs) noexcept {
	if (this != rhs) {
		buckets_.swap(rhs.buckets_);
		mystl::swap(bucket_size_, rhs.bucket_size_);
		mystl::swap(size_, rhs.size_);
		mystl::swap(mlf_, rhs.mlf_);
		mystl::swap(hash_, rhs.hash_);
		mystl::swap(equal_, rhs.equal_);
	}
}

/****************************************************************************************/
// helper function

// init 函数
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
init(size_type n) {
	const auto bucket_nums = next_size(n);	//获取下一个质数大小，作为bucket的个数
	try {
		buckets_.reserve(bucket_nums);		//重置bucket
		buckets.assign(bucket_nums, nullptr);	//赋值为空指针
	}
	catch (...)
	{
		bucket_size_ = 0;
		size_ = 0;
		throw;
	}
	bucket_size_ = buckets_.size();
}

// copy_init 函数
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
copy_init(const hashtable& ht) {
	bucket_size_ = 0;
	buckets_.reserve(ht.bucket_size_);
	buckets_.assign(ht.bucket_size_, nullptr);
	try {
		for (size_type i = 0; i < ht.bucket_size_; ++i) {	//遍历bucket
			node_ptr cur = ht.buckets_[i];	//指向bucket起始处
			if (cur) {
				// 如果某 bucket 存在链表
				auto copy = create_node(cur->value);
				buckets_[i] = copy;
				for (auto next = cur->next; next; cur = next, next = cur->next) {	//遍历bucket下的list
					//复制链表
					copy->next = create_node(next->value);
					copy = copy->next;
				}
				copy = nullptr;
			}
		}
		bucket_size_ = ht.bucket_size_;
		mlf_ = ht.mlf_;
		size_ = ht.size_;
	}
	catch (...)
	{
		clear();
	}
}

// create_node 函数
template <class T, class Hash, class KeyEqual>
template <class ...Args>
typename hashtable<T, Hash, KeyEqual>::node_ptr
hashtable<T, Hash, KeyEqual>::
create_node(Args&& ...args) {
	node_ptr tmp = node_allocator::allocate(1);	//配置空间
	try
	{
		data_allocator::construct(mystl::address_of(tmp->value), mystl::forward<Args>(args)...);	//构造元素
		tmp->next = nullptr;
	}
	catch (...)
	{
		node_allocator::deallocate(tmp);
		throw;
	}
	return tmp;
}

// destroy_node 函数
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
destroy_node(node_ptr node)
{
	data_allocator::destroy(mystl::address_of(node->value));	//销毁元素
	node_allocator::deallocate(node);	//释放空间
	node = nullptr;	
}

// next_size 函数
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::next_size(size_type n) const
{
	return ht_next_prime(n);	//质数列表中n大的质数
}

// hash 函数
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
hash(const key_type& key, size_type n) const
{
	return hash_(key) % n;	//调用真正的hash函数，对n取模求出余数，即bucket的位置
}

template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
hash(const key_type& key) const
{
	return hash_(key) % bucket_size_;	//调用真正的hash函数，对bucket_size_取模求出余数，即bucket的位置
}

// rehash_if_need 函数
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
rehash_if_need(size_type n)
{
	if (static_cast<float>(size_ + n) > (float)bucket_size_ * max_load_factor())
		//如果计算出需要重新调整bucket vector大小，就调用rehash进行重新哈希
		rehash(size_ + n);
}

// copy_insert 允许元素重复
template <class T, class Hash, class KeyEqual>
template <class InputIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_multi(InputIter first, InputIter last, mystl::input_iterator_tag)
{
	rehash_if_need(mystl::distance(first, last));	//调整bucket vector大小
	for (; first != last; ++first)
		insert_multi_noresize(*first);	//遍历插入元素
}
// copy_insert 允许元素重复
template <class T, class Hash, class KeyEqual>
template <class ForwardIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_multi(ForwardIter first, ForwardIter last, mystl::forward_iterator_tag)
{
	size_type n = mystl::distance(first, last);
	rehash_if_need(n);
	for (; n > 0; --n, ++first)
		insert_multi_noresize(*first);
}

// copy_insert 不允许元素重复
template <class T, class Hash, class KeyEqual>
template <class InputIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_unique(InputIter first, InputIter last, mystl::input_iterator_tag)
{
	rehash_if_need(mystl::distance(first, last));
	for (; first != last; ++first)
		insert_unique_noresize(*first);
}
// copy_insert 不允许元素重复
template <class T, class Hash, class KeyEqual>
template <class ForwardIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_unique(ForwardIter first, ForwardIter last, mystl::forward_iterator_tag)
{
	size_type n = mystl::distance(first, last);
	rehash_if_need(n);
	for (; n > 0; --n, ++first)
		insert_unique_noresize(*first);
}

// insert_node 函数，允许元素重复
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
insert_node_multi(node_ptr np) {
	const auto n= hash(value_traits::get_key(np->value));
	auto cur = buckets_[n];
	if (cur == nullptr) {	//如果开头为空，直接在开头插入
		buckets_[n] = np;
		++size_;
		return iterator(np, this);
	}
	for (; cur; cur = cur->next) {	//进行bucket遍历
		if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value))) { //找到相等元素就插入
			np->next = cur->next;
			cur->next = np;
			++size_;
			return iterator(np, this);
		}
	}
	np->next = buckets_[n];	//否则跳到下一个bucket开头进行插入
	buckets_[n] = np;
	++size_;
	return iterator(np, this);
}

// insert_node_unique 函数，不允许元素重复
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::
insert_node_unique(node_ptr np)
{
	const auto n = hash(value_traits::get_key(np->value));
	auto cur = buckets_[n];
	if (cur == nullptr)		//bucket开头为空，直接插入
	{
		buckets_[n] = np;
		++size_;
		return mystl::make_pair(iterator(np, this), true);
	}
	for (; cur; cur = cur->next)	//遍历bucket
	{
		if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value)))
		{	//有相等元素直接返回false
			return mystl::make_pair(iterator(cur, this), false);
		}
	}
	//继续寻找下一个bucket开头进行插入
	np->next = buckets_[n];
	buckets_[n] = np;
	++size_;
	return mystl::make_pair(iterator(np, this), true);
}

// replace_bucket 函数 重置bucket大小
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
replace_bucket(size_type bucket_count) {
	bucket_type bucket(bucket_count);	//获取需要的bucket元素个数
	if (size_ != 0) {
		for (size_type i = 0; i < bucket_size_; ++i;) {	//遍历bucket vector
			for (auto first = buckets_[i]; first; first = first->next) {	//遍历bucket
				auto tmp = create_node(first->value);	//产生新节点
				const auto n=hash(value_traits::get_key(first->value), bucket_count);	//哈希计算
				auto f = bucket[n];		//bucket开头
				bool is_inserted = false;
				for (auto cur = f; cur; cur = cur->next) {	//进行遍历
					if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(first->value))) {
						//相等就插入元素
						tmp->next = cur->next;
						cur->next = tmp;
						is_inserted = true;
						break;
					}
				}
				if (!is_inserted) {
					//跳到下一个bucket
					tmp->next = f;
					bucket[n] = tmp;
				}
			}
		}
	}
	buckets_.swap(bucket);			//交换两个bucket
	bucket_size_ = buckets_.size();
}

// erase_bucket 函数
// 在第 n 个 bucket 内，删除 [first, last) 的节点
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase_bucket(size_type n, node_ptr first, node_ptr last) {
	auto cur = buckets_[n];	//bucket开头
	if (cur == first) {		//first等于bucket开头，直接删除
		erase_bucket(n, last);
	}
	else {
		node_ptr next = cur->next;	//跳到下一个bucket
		for(;next!=first;cur=next,next=cur->next){}	//遍历bucket
		while (next!=last)
		{	//遍历查找与删除
			cur->next = next->next;
			destroy_node(next);
			next = cur->next;
			--size_;
		}
	}
}

// erase_bucket 函数
// 在第 n 个 bucket 内，删除 [buckets_[n], last) 的节点
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase_bucket(size_type n, node_ptr last) {
	auto cur = buckets_[n];
	while (cur!=last)
	{	//遍历删除
		auto next = cur->next;
		destroy_node(cur);
		cur = next;
		--size_;
	}
	buckets_[n] = last;
}

// equal_to 函数，允许重复元素
template <class T, class Hash, class KeyEqual>
bool hashtable<T, Hash, KeyEqual>::equal_to_multi(const hashtable& other)
{
	if (size_ != other.size_)	//两个hashtable大小得相等
		return false;
	for (auto f = begin(), l = end(); f != l;)	//遍历hashtable
	{	//相等元素的比较
		auto p1 = equal_range_multi(value_traits::get_key(*f));
		auto p2 = other.equal_range_multi(value_traits::get_key(*f));
		if (mystl::distance(p1.first, p1.last) != mystl::distance(p2.first, p2.last) ||
			!mystl::is_permutation(p1.first, p2.last, p2.first, p2.last))
			//查找到末尾
			return false;
		f = p1.last;
	}
	return true;
}

// equal_to 函数，不允许重复元素
template <class T, class Hash, class KeyEqual>
bool hashtable<T, Hash, KeyEqual>::equal_to_unique(const hashtable& other)
{
	if (size_ != other.size_)
		return false;
	for (auto f = begin(), l = end(); f != l; ++f)
	{
		auto res = other.find(value_traits::get_key(*f));
		if (res.node == nullptr || *res != *f)
			return false;
	}
	return true;
}

// 重载 mystl 的 swap
template <class T, class Hash, class KeyEqual>
void swap(hashtable<T, Hash, KeyEqual>& lhs,
	hashtable<T, Hash, KeyEqual>& rhs) noexcept
{
	lhs.swap(rhs);
}

}	//命名空间结束符

#endif // !MYSTL_HASHTABLE_H_

