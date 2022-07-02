#ifndef MYSTL_HASHTABLE_H_
#define MYSTL_HASHTABLE_H_

// ���ͷ�ļ�������һ��ģ���� hashtable
// hashtable : ��ϣ��ʹ�ÿ����������ͻ

#include <initializer_list>		//��ʼ���б�

#include "algo.h"				//�����㷨
#include "functional.h"			//�º������ϣ����
#include "memory.h"				//�߼��Ķ�̬�ڴ����
#include "vector.h"				//vector
#include "util.h"				//ͨ�ù���
#include "exceptdef.h"			//�쳣

namespace mystl {

// hashtable �Ľڵ㶨��
template <class T> struct hashtable_node {
	hashtable_node* next;		// ָ����һ���ڵ�
	T				value;		// ����ʵֵ

	//���졢�������ƶ�����
	hashtable_node() = default;
	hashtable_node(const T& n):next(nullptr),value(n){}

	hashtable_node(const hashtable_node& node):node(node.next),value(node.value){}
	hashtable_node(hashtable_node&& node):next(node.next),value(mystl::move(node.value)){
		node.next = nullptr;
	}
};

// value traits 
//��ȡ�����ľ���ʵ��
template <class T, bool>
struct ht_value_traits_imp
{
	typedef T key_type;			//������keyֵ
	typedef T mapped_type;		//������valueֵ
	typedef T value_type;		//������pairֵ����pair<const T,T>����ʾ����ֵ���ͱ�

	template <class Ty>
	static const key_type& get_key(const Ty& value)			//��ȡkeyֵ
	{
		return value;
	}

	template <class Ty>
	static const value_type& get_value(const Ty& value)		//��ȡvalueֵ
	{
		return value;
	}
};

//��ȡpair
template <class T>
struct ht_value_traits_imp<T, true> {
	//remove_cv�ṩ��type��ͬ�ĳ�Ա typedef T��ֻ��ɾ���������� cv �޶���,��c��const����v��volatile���޶���
	//���²ο����ͣ�https://www.cnblogs.com/5iedu/p/7767937.html
					https://blog.csdn.net/janeqi1987/article/details/100049597
	typedef typename std::remove_cv<typename T::first_type>::type   key_type;		//pair��first����keyֵ
	typedef typename T::second_type									mapped_type;	//pair��second����mappedֵ
	typedef T														value_type;		//pair���ͣ�����first��second

	template <class Ty>static const key_type& get_key(const Ty& value) {
		return value.first;		//��ȡpair��keyֵ����first
	}
	template <class Ty>static const value_type& get_value(const Ty& value) {
		return value;			//��ȡpair��valueֵ
	}
};

//��ȡ��ǰֵ
template <class T>
struct ht_value_traits {
	static constexpr bool is_map = mystl::is_pair<T>::value;	//�ж��Ƿ���map�����Ƿ���pair

	typedef ht_value_traits_imp<T, is_map>value_traits_type;	//��ȡ�ͱ���

	typedef typename value_traits_type::key_type		key_type;		//keyֵ
	typedef typename value_traits_type::mapped_type		mapped_type;	//mappedֵ
	typedef typename value_traits_type::value_type		value_type;		//valueֵ

	template <class Ty>static const key_type& get_key(const Ty& value) {
		return value_traits_type::get_key(value);		//������ȡ�ͱ��жϺ��ȡkeyֵ
	}
	template <class Ty>static const key_type& get_value(const Ty& value) {
		return value_traits_type::get_value(value);		//������ȡ�ͱ��жϺ��ȡvalueֵ
	}
};

// forward declaration	ǰ������

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
//������������class Hashָ��hash Function��class KeyEqualָ��keyֵ������
template <class T, class Hash, class KeyEqual>struct ht_iterator_base :	
	public mystl::iterator<mystl::forward_iterator_tag, T> {

	typedef mystl::hashtable<T, Hash, KeyEqual>         hashtable;		//hashtable�ͱ𣬱����������Ĺ���
	typedef ht_iterator_base<T, Hash, KeyEqual>         base;			//�����ͱ�
	typedef mystl::ht_iterator<T, Hash, KeyEqual>       iterator;		//�������ͱ�
	typedef mystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
	typedef hashtable_node<T>*							node_ptr;		//�ڵ��ͱ�
	typedef hashtable*									contain_ptr;	//ָ��hashtable��ָ���ͱ�
	typedef const node_ptr                              const_node_ptr;
	typedef const contain_ptr                           const_contain_ptr;

	typedef size_t                                      size_type;			//��С�ͱ�
	typedef ptrdiff_t                                   difference_type;	//�����ͱ�

	node_ptr	node;	// ��������ǰ��ָ�ڵ�
	contain_ptr ht;		// ָ��hashtable�����������������ᣨ��Ϊ������Ҫ��bucket����bucket��

	ht_iterator_base() = default;	//Ĭ�Ϲ��캯��

	//���������
	bool operator==(const base& rhs)const { return node == rhs.node; }
	bool operator!=(const base& rhs) const { return node != rhs.node; }
};

//��ʽ�ĵ��������̳л���������
template <class T, class Hash, class KeyEqual>
struct ht_iterator :public ht_iterator_base<T, Hash, KeyEqual> {

	typedef ht_iterator_base<T, Hash, KeyEqual> base;			//�����ͱ�
	typedef typename base::hashtable            hashtable;		
	typedef typename base::iterator             iterator;
	typedef typename base::const_iterator       const_iterator;
	typedef typename base::node_ptr             node_ptr;
	typedef typename base::contain_ptr          contain_ptr;

	typedef ht_value_traits<T>                  value_traits;	
	typedef T                                   value_type;
	typedef value_type*							pointer;
	typedef value_type&							reference;

	//using�������������������ø���ĳ�Ա
	using base::node;
	using base::ht;

	//���졢��������ֵ����
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

	// ���ز�����
	reference operator*()const { return node->value; }
	pointer operator->()const { return &(operator*()); }

	iterator& operator++()const {		//ǰ��++
		MYSTL_DEBUG(node != nullptr);
		const node_ptr old = node;	//�ȱ��浱ǰָ��
		node = node->next;			//ǰ��һ��λ�ã�������ڣ�˵����ͬ��bucket�ڣ����ɷ���
		if (node == nullptr) {		// �����һ��λ��Ϊ�գ�������һ�� bucket ����ʼ��
			auto index = ht->hash(value_traits::get_key(old->value));	//��λ��һ��bucket��λ��
			while (!node && ++index < ht->bucket_size_) {		//���û�г���bucket��С
				node = ht->buckets_[index];		//������һ��bucket����ʼ��
			}
		}
		return *this;
	}
	iterator& operator++(int) {		//����++
		iterator tmp = *this;
		++* this;	//����ǰ��++
		return tmp;
	}

	//hashtable�ĵ�����û�к��˲�����hashtableҲû�ж������������
};

//const�汾�ĵ��������
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

	// ���ز�����
	reference operator*()  const { return node->value; }
	pointer   operator->() const { return &(operator*()); }

	const_iterator& operator++()
	{
		MYSTL_DEBUG(node != nullptr);
		const node_ptr old = node;
		node = node->next;
		if (node == nullptr)
		{ // �����һ��λ��Ϊ�գ�������һ�� bucket ����ʼ��
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

// local iterator���ڱ�bucket�ĵ�����
template <class T>
struct ht_local_iterator :public mystl::iterator<mystl::forward_iterator_tag, T> {
	//��Ƕ�ͱ�
	typedef T                          value_type;
	typedef value_type*				   pointer;
	typedef value_type&				   reference;
	typedef size_t                     size_type;
	typedef ptrdiff_t                  difference_type;
	typedef hashtable_node<T>*		   node_ptr;		//�ڵ��ͱ�

	typedef ht_local_iterator<T>       self;
	typedef ht_local_iterator<T>       local_iterator;
	typedef ht_const_local_iterator<T> const_local_iterator;

	node_ptr node;	//ָ��ǰ�ڵ�

	//���졢��������
	ht_local_iterator(node_ptr n):node(n) {}
	ht_local_iterator(const local_iterator& rhs)
		:node(rhs.node)
	{
	}
	ht_local_iterator(const const_local_iterator& rhs)
		:node(rhs.node)
	{
	}

	//����������
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

//const�汾�ı��ص�����
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

// bucket ʹ�õĴ�С

//�����VC��������GUNC��clang����������ָ���СΪ8bytes
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1			//64λϵͳ
#else
#define SYSTEM_32 1			//32λϵͳ
#endif

#ifdef SYSTEM_64			//�����64λϵͳ

#define PRIME_NUM 99		//����99������

//�������б����������
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

#else	//�������64λϵͳ������32λϵͳ

#define PRIME_NUM 44	//����44������

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

//�ҳ��������������֮�У���ӽ������ڵ��� n ���Ǹ�����
inline size_t ht_next_prime(size_t n) {
	const size_t* first = ht_prime_list;
	const size_t* last = ht_prime_list + PRIME_NUM;
	//lower_bound�Ƿ����㷨��ʹ��lower_bound()��������������
	const size_t* pos=mystl::lower_bound(first, last, n);
	return pos == last ? *(last - 1) : *pos;
}

// ģ���� hashtable
// ����һ�����������ͣ������������ϣ�����������������ֵ��ȵıȽϺ���
template <class T, class	Hash, class KeyEqual>class hashtable {
	//�����ɷ���hashtable�ĵ�������Ԫ
	friend struct mystl::ht_iterator<T, Hash, KeyEqual>;
	friend struct mystl::ht_const_iterator<T, Hash, KeyEqual>;

public:
	// hashtable ���ͱ���
	typedef ht_value_traits<T>                          value_traits;	
	typedef typename value_traits::key_type             key_type;			//key�ͱ�
	typedef typename value_traits::mapped_type          mapped_type;		//mapped�ͱ�
	typedef typename value_traits::value_type           value_type;			//pair��value�ͱ�
	typedef Hash                                        hasher;				//hashFunction�ͱ�
	typedef KeyEqual                                    key_equal;			//�ȽϺ����ͱ�

	typedef hashtable_node<T>                           node_type;			//�ڵ��ͱ�
	typedef node_type*									node_ptr;			//�ڵ�ָ���ͱ�
	typedef mystl::vector<node_ptr>                     bucket_type;		//vector bucket�ͱ�

	typedef mystl::allocator<T>                         allocator_type;		//�����ڴ��ͱ�
	typedef mystl::allocator<T>                         data_allocator;
	typedef mystl::allocator<node_type>                 node_allocator;

	typedef typename allocator_type::pointer            pointer;			//��Ƕ�ͱ�
	typedef typename allocator_type::const_pointer      const_pointer;
	typedef typename allocator_type::reference          reference;
	typedef typename allocator_type::const_reference    const_reference;
	typedef typename allocator_type::size_type          size_type;			//������С�ͱ�
	typedef typename allocator_type::difference_type    difference_type;
		
	typedef mystl::ht_iterator<T, Hash, KeyEqual>       iterator;			//�������ͱ�
	typedef mystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
	typedef mystl::ht_local_iterator<T>                 local_iterator;
	typedef mystl::ht_const_local_iterator<T>           const_local_iterator;

	allocator_type get_allocator() const { return allocator_type(); }	//��ȡ��Ӧ�ڴ��ͱ�

private:
	bucket_type buckets_;		//buckets����vector���
	size_type	bucket_size_;	//bucket��С
	size_type	size_;			//bucket vector�Ĵ�С
	float		mlf_;			
	hasher		hash_;			//��ϣ����
	key_equal	equal_;			//�ȽϺ���

private:

	//�ȽϺ���
	bool is_equal(const key_type& key1, const key_type& key2) {
		return equal_(key1, key2);
	}
	bool is_equal(const key_type& key1, const key_type& key2) const
	{
		return equal_(key1, key2);
	}

	//���ڻ�ȡ�ڵ�
	const_iterator M_cit(node_ptr node)const noexcept {
		//const_castȥ���ڵ��const���ʣ��������޸ģ���������
		return const_iterator(node, const_cast<hashtable*>(this));	
	}

	iterator M_begin()noexcept {
		for (size_type n = 0; n < bucket_size_; ++n) {
			if (buckets_[n])	 // �ҵ���һ���нڵ��λ�þͷ���
				return iterator(buckets_[n], this);
		}
		return iterator(nullptr, this);
	}

	const_iterator M_begin()noexcept {
		for (size_type n = 0; n < bucket_size_; ++n) {
			if (buckets_[n])	// �ҵ���һ���нڵ��λ�þͷ���
				return M_cit(buckets_[n]);
		}
		return M_cit(nullptr);
	}

public:
	// ���졢���ơ��ƶ�����������
	explicit hashtable(size_type bucket_count, const Hash& hash = hash(), const KeyEqual& euqal = KeyEqual())
		:size_(0), mlf_(1.0f), hash_(hash), equal_(equal) {
		init(bucket_count);		//����init()
	}

	template <class Iter, typename std::enable_if<		//enable_if���ݵ���������Ѱ�����ƥ�亯��
		mystl::is_input_iterator<Iter>::value, int>::type = 0>
		hashtable(Iter first, Iter last,
			size_type bucket_count,
			const Hash& hash = Hash(),
			const KeyEqual& equal = KeyEqual())
		: size_(mystl::distance(first, last)), mlf_(1.0f), hash_(hash), equal_(equal)
	{	//��ʼ�����ֵ
		init(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))));
	}

	hashtable(const hashtable& rhs)					//��������
		:hash_(rhs.hash_), equal_(rhs.equal_)
	{
		copy_init(rhs);
	}

	hashtable(hashtable&& rhs) noexcept				//�ƶ�����
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

	hashtable& operator=(const hashtable& rhs);			//��ֵ����
	hashtable& operator=(hashtable&& rhs) noexcept;

	~hashtable() { clear(); }							//��������

	// ��������ز���
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

	// ������ز���
	bool      empty()    const noexcept { return size_ == 0; }
	size_type size()     const noexcept { return size_; }
	//npos��һ����������ʾsize_t�����ֵ
	//����������ṩ���������������ʾ�����ڵ�λ�ã�����һ����std::container_type::size_type
	//static const size_type npos = -1�൱��static_cast<size_type>(-1),ע�������size_type��allocator�������޷�����
	size_type max_size() const noexcept { return static_cast<size_type>(-1); }

	// �޸�������ز���

	// emplace / empalce_hint

	template <class ...Args>
	iterator emplace_multi(Args&& ...args);					//�����ظ�Ԫ�صĲ���

	template <class ...Args>
	pair<iterator, bool> emplace_unique(Args&& ...args);	//�������ظ�Ԫ�صĲ���

	// [note]: hint ���� hash_table ��ʵû�����壬��Ϊ��ʹ�ṩ�� hint��ҲҪ��һ�� hash��
	// ��ȷ�� hash_table �����ʣ�����ѡ�������
	template <class ...Args>
	iterator emplace_multi_use_hint(const_iterator /*hint*/, Args&& ...args)
	{
		return emplace_multi(mystl::forward<Args>(args)...);	//forward��֤������������ת��
	}

	template <class ...Args>
	iterator emplace_unique_use_hint(const_iterator /*hint*/, Args&& ...args)
	{
		return emplace_unique(mystl::forward<Args>(args)...).first;	//forward��֤������������ת��
	}

	// insert

	iterator             insert_multi_noresize(const value_type& value);	//�����ظ�Ԫ�صĲ���
	pair<iterator, bool> insert_unique_noresize(const value_type& value);	//�������ظ�Ԫ�صĲ���

	//�����ظ�Ԫ�صĲ���
	iterator insert_multi(const value_type& value)	
	{
		rehash_if_need(1);		//����rehash�������нڵ�����·���
		return insert_multi_noresize(value);	//Ԫ�ز���
	}
	iterator insert_multi(value_type&& value)
	{
		return emplace_multi(mystl::move(value));	//Ԫ�ز���
	}

	//�������ظ�Ԫ�صĲ���
	pair<iterator, bool> insert_unique(const value_type& value)
	{
		rehash_if_need(1);		//����rehash��������bucket vector�����·���
		return insert_unique_noresize(value);		//Ԫ�ز���
	}
	pair<iterator, bool> insert_unique(value_type&& value)
	{
		return emplace_unique(mystl::move(value));	//Ԫ�ز���
	}

	// [note]: ͬ emplace_hint
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

	//�������������
	template <class InputIter>
	void insert_multi(InputIter first, InputIter last)		//�����ظ�Ԫ�ز���
	{
		copy_insert_multi(first, last, iterator_category(first));
	}

	template <class InputIter>
	void insert_unique(InputIter first, InputIter last)		//�������ظ�Ԫ�ز���
	{
		copy_insert_unique(first, last, iterator_category(first));
	}

	// erase / clear

	void      erase(const_iterator position);		//���ĳ��Ԫ��
	void      erase(const_iterator first, const_iterator last);		//���һ������

	size_type erase_multi(const key_type& key);		//�����ظ�Ԫ�����
	size_type erase_unique(const key_type& key);	//�������ظ�Ԫ�ص����

	void      clear();			//���ȫ��Ԫ��

	void      swap(hashtable& rhs) noexcept;	//����Ԫ��

	// ������ز���

	size_type                            count(const key_type& key) const;	//������keyֵ���Ԫ�صĸ���

	iterator                             find(const key_type& key);
	const_iterator                       find(const key_type& key) const;

	//�����ظ�Ԫ������key��ȵ�Ԫ��
	pair<iterator, iterator>             equal_range_multi(const key_type& key);
	pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const;

	//���ҷ��ظ�Ԫ������key��ȵ�Ԫ��
	pair<iterator, iterator>             equal_range_unique(const key_type& key);
	pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const;

	// bucket interface
	//��ȡĳ��bucket����ʼλ��
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
	
	//��ȡĳ��bucket��ĩβ����Ϊ��
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

	//��ȡbucket�ĸ�������bucket vector �Ĵ�С
	size_type bucket_count() const noexcept { return bucket_size_; }

	//��ȡbucket�����ֵ���������б��е��������-1(�±��0��ʼ)
	size_type max_bucket_count() const noexcept {
		return ht_prime_list[PRIME_NUM - 1];
	}

	size_type bucket_size(size_type n)       const noexcept;	//��ĳ�� bucket �ڵ�ĸ���
	size_type bucket(const key_type& key)    const
	{
		return hash(key);	//���ù�ϣ����
	}

	// hash policy��ϣ����
	float load_factor() const noexcept	//����Ԫ��
	{
		//bucket_size��Ϊ0������������С/bucket��С������Ϊ0
		return bucket_size_ != 0 ? (float)size_ / bucket_size_ : 0.0f;
	}
	float max_load_factor() const noexcept	//����������
	{
		return mlf_;
	}
	void max_load_factor(float ml)
	{	//�׳��쳣"��Ч�Ĺ�ϣ��������"
		THROW_OUT_OF_RANGE_IF(ml != ml || ml < 0, "invalid hash load factor");
		mlf_ = ml;
	}

	void rehash(size_type count);		//�ؽ���ϣ������ڵ�Ӧ�����ĸ�λ��

	void reserve(size_type count)		//�ؽ�bucket vector
	{
		rehash(static_cast<size_type>((float)count / max_load_factor() + 0.5f));
	}

	hasher    hash_fcn() const { return hash_; }	//��ϣ����
	key_equal key_eq()   const { return equal_; }	//�ȽϺ���

private:
	// hashtable ��Ա����
	// init
	void      init(size_type n);
	void      copy_init(const hashtable& ht);

	// node
	template  <class ...Args>
	node_ptr  create_node(Args&& ...args);	//�����ڵ�
	void      destroy_node(node_ptr n);		//���ٽڵ�

	// hash
	size_type next_size(size_type n) const;	//��һ�ڵ��С
	size_type hash(const key_type& key, size_type n) const;		//hash����
	size_type hash(const key_type& key) const;
	void      rehash_if_need(size_type n);	//����hash

	// insert
	//������ͬԪ��
	template <class InputIter>
	void copy_insert_multi(InputIter first, InputIter last, mystl::input_iterator_tag);
	template <class ForwardIter>
	void copy_insert_multi(ForwardIter first, ForwardIter last, mystl::forward_iterator_tag);
	//��������ͬԪ��
	template <class InputIter>
	void copy_insert_unique(InputIter first, InputIter last, mystl::input_iterator_tag);
	template <class ForwardIter>
	void copy_insert_unique(ForwardIter first, ForwardIter last, mystl::forward_iterator_tag);
	
	// insert node
	pair<iterator, bool> insert_node_unique(node_ptr np);	//��������ͬԪ��
	iterator             insert_node_multi(node_ptr np);	//������ͬԪ��

	 // bucket operator
	void replace_bucket(size_type bucket_count);			//bucket������
	void erase_bucket(size_type n, node_ptr first, node_ptr last);	//bucket�����
	void erase_bucket(size_type n, node_ptr last);

	// comparision �ȽϺ���
	bool equal_to_multi(const hashtable& other);
	bool equal_to_unique(const hashtable& other);
};

/*****************************************************************************************/

// ���Ƹ�ֵ�����
template <class T, class Hash, class KeyEqual>
hashtable<T, Hash, KeyEqual>&
hashtable<T, Hash, KeyEqual>::
operator=(const hashtable& rhs) {
	if (this != rhs) {
		hashtable tmp(rhs);
		swap(tmp);	//����this��tmp
	}
	return *this;
}

// �ƶ���ֵ�����
template <class T, class Hash, class KeyEqual>
hashtable<T, Hash, KeyEqual>&
hashtable<T, Hash, KeyEqual>::
operator=(hashtable&& rhs) noexcept {
	hashtable tmp(mystl::move(rhs));
	swap(tmp);
	return *this;
}

// �͵ع���Ԫ�أ���ֵ�����ظ�
// ǿ�쳣��ȫ��֤
template <class T, class Hash, class KeyEqual>
template <class ...Args>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
emplace_multi(Args&& ...args) {
	auto np = create_node(mystl::forward<Args>(args)...);	//forward��֤������ȫת��
	try {
		//��Ԫ�ظ���(������Ԫ�ؼ����)��bucket vector��С�Աȣ����ǰ�ߴ��ں��ߣ����ؽ�bucket vector
		if ((float)(size + 1) > (float)bucket_size_ * max_load_factor())	
			rehash(size_ + 1);
	}
	catch (...) {
		destroy_node(np);	//ʧ�ܾ�����Ԫ��
		throw;
	}
	return insert_node_multi(np);	//�ؽ���ϣ�����Ԫ�أ������ظ�Ԫ��
}

// �͵ع���Ԫ�أ���ֵ�������ظ�
// ǿ�쳣��ȫ��֤
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

// �ڲ���Ҫ�ؽ���������²����½ڵ㣬��ֵ�������ظ�
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::
insert_unique_noresize(const value_type& value) {
	const auto n = hash(value_traits::get_key(value));	//����valueӦλ�ڵ�n��bucket
	auto first = buckets_[n];	//��firstָ��bucket��Ӧ֮����ͷ��
	//���bucket[n]�ѱ�ռ�ã���ʱfirst��Ϊ0�����ǽ���ѭ�����߹�bucket����Ӧ֮��������
	for (auto cur = first; cur; cur = cur->next) {
		if(is_equal(value_traits::get_key(cur->value), value_traits::get_key(value)))
			//��������������е�ĳ��ֵ��ͬ���Ͳ����룬���̷���
			return mystl::make_pair(iterator(cur, this), false);
	}
	// �뿪����ѭ��(�����δ����ѭ��)����firstָ��bucket��ָ�����ͷ���ڵ�
	auto tmp = create_node(value);
	tmp->next = first;
	buckets_[n] = tmp;		//���½ڵ��Ϊ����ĵ�һ���ڵ�
	++size_;				//�ڵ�����ۼ�1
	return mystl::make_pair(iterator(tmp, this), true);		//���ز���ɹ�
}

// �ڲ���Ҫ�ؽ���������²����½ڵ㣬��ֵ�����ظ�
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
insert_multi_noresize(const value_type& value) {
	const auto n = hash(value_traits::get_key(value));		//����valueӦλ�ڵ�n��bucket
	auto first = buckets_[n];		//��firstָ��bucket��Ӧ֮����ͷ��
	auto tmp = create_node(value);	//�����½ڵ�
	//���bucket[n]�ѱ�ռ�ã���ʱfirst��Ϊ0�����ǽ���ѭ�����߹�bucket����Ӧ֮��������
	for (auto cur = first; cur; cur = cur->next) {
		if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value))) {
			// ��������д�����ͬ��ֵ�Ľڵ�����ϲ��룬Ȼ�󷵻�
			tmp->next = cur->next;	//���½ڵ����Ŀǰλ��
			cur->next = tmp;		
			++size_;				//�ڵ�����ۼ�1
			return	iterator(tmp, this);	//����һ����������ָ�������ڵ�
		}	
	}
	// �������ˣ���ʾû�з����ظ��ļ�ֵ�����ǲ���������ͷ��
	tmp->next = first;	//���½ڵ����������ͷ��
	buckets_[n] = tmp;
	++size_;			//�ڵ�����ۼ�1
	return iterator(tmp, this);	// ����һ����������ָ�������ڵ�
}

// ɾ����������ָ�Ľڵ�
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase(const_iterator position) {
	auto p = position.node;
	if (p) {
		const auto n = hash(value_traits::get_key(p->value));	//����valueӦλ�ڵ�n��bucket
		auto cur = buckets_[n];			//��curָ��bucket��Ӧ֮����ͷ��
		if (cur == p) {					// p λ������ͷ��
			buckets_[n] = cur->next;	//��bucket[n]��Ϊ��ǰԪ�ص���һ��
			destroy_node(next);			//����Ҫɾ����Ԫ��
			--size_;					//�ڵ�����ۼ�1
			break;
		}
		else {	//��Ϊ����ͷ��ʱ
			auto next = cur->next;		//������
			while (next)
			{
				if (next == p) {		//������Ҫ��ɾ����Ԫ�أ�����ɾ��
					cur->next = next->next;
					destroy_node(next);
					--size_;
					break;
				}
				else {				//����������
					cur = next;
					next = cur->next;
				}
			}
		}
		
	}
}

// ɾ��[first, last)�ڵĽڵ�
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase(const_iterator first, const_iterator last) {
	if (first.node == last.node)	//�ж�first��last�Ƿ�ͬһ�ڵ�
		return;
	//��ȡfirst����bucket
	auto first_bucket = first.node	
		? hash(value_traits::get_key(first.node->value))
		: bucket_size_;
	//��ȡlast����bucket
	auto last_bucket = last.node
		? hash(value_traits::get_key(last.node->value))
		: bucket_size_;
	if (first_bucket == last_bucket) {
		// ����� bucket ��ͬһ��λ��
		erase_bucket(first_bucket, first.node, last.node);
	}
	else {
		erase_bucket(first_bucket, first.node, nullptr);	//ɾ��first bucket����Ԫ��
		for (auto n = first_bucket + 1; n < last_bucket; ++n) {	//����[first��last]����ɾ��
			if (buckets_[n] != nullptr)
				erase_bucket(n, nullptr);
		}
		if (last_bucket != bucket_size_) {		//ɾ��last bucket
			erase_bucket(last_bucket_last.node);
		}
	}
}

// ɾ����ֵΪ key �Ľڵ㣬�����ֵ�ظ�
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
erase_multi(const key_type& key) {
	auto p=equal_range_multi(key);	//��ȡ���ֵkey��ȵĽڵ�
	if (p.first.node != nullptr) {
		erase(p.first, p.second);	//����ɾ��
		return mystl::distance(p.first, p.second);
	}
	return 0;
}

// ɾ����ֵΪ key �Ľڵ㣬������Ԫ���ظ�
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
erase_unique(const key_type& key) {
	const auto n = hash(key);		//��ϣ����key���ڵ�bucket
	auto first = buckets_[n];		//��firstָ��bucket����ʼ��
	if (first) {
		if (is_equal(value_traits::get_key(first->value), key)) {	
			//������ҵ���keyֵ��ȵ�Ԫ�أ��ͽ���ɾ��
			buckets_[n] = first->next;
			destroy_node(first);
			--size_;
			return 1;
		}
		else {	//�����bucket���Ҳ�������������һ��bucket���в���
			auto next = first->next;
			while (next)
			{
				if (is_equal(value_traits::get_key(next->value), key)) {
					//������ҵ���keyֵ��ȵ�Ԫ�أ��ͽ���ɾ��
					first->next = next->next;
					destroy_node(next);
					--size_;
					return 1;
				}
				//���Ҳ����ͼ�����Ծ����һ��bucket
				first = next;
				next = first->next;
			}
		}
	}
	return 0;
}

// clear ��� hashtable
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::clear() {
	if (size_ != 0) {
		//���ÿ��bucket
		for (size_type i = 0; i < bucket_size_; ++i) {
			node_ptr cur = buckets_[i];
			//��bucket list�е�ÿ���ڵ�ɾ��
			while (cur!=nullptr)
			{
				node_ptr next = cur->next;
				destroy_node(cur);
				cur = next;
			}
			buckets_[i] = nullptr;	//��bucket����Ϊnullptr
		}
		size_ = 0;	//���ܽڵ����Ϊ0
	}
	//ע�⣺bucket vector��δ�ͷŵ��ռ䣬�Ա���ԭ����С
}

// ��ĳ�� bucket �ڵ�ĸ���
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
bucket_size(size_type n) const noexcept {
	size_type result = 0;
	for (auto cur = buckets_[n]; cur = cur->next) {		//����bucket
		++result;	//�����ۼ�
	}
	return result;
}

// ���¶�Ԫ�ؽ���һ���ϣ�����뵽�µ�λ��
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
rehash(size_type count) {
	auto n = ht_next_prime(count);	//nΪ��һ����count�������
	if (n > bucket_size_) {			//���n���ڵ�ǰbucket_size�ͽ���bucket vector������
		replace_bucket(n);
	}
	else {
		if ((float)size_ / (float)n < max_load_factor() - 0.25f &&
			(float)n < (float)bucket_size_ * 0.75)  // worth rehash
		{	//���ֵ�����¹�ϣ��Ҳͬ������bucket vector������
			replace_bucket(n);
		}

	}
}

// ���Ҽ�ֵΪ key �Ľڵ㣬�����������
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
find(const key_type& key) {
	const auto n = hash(key);		//��ϣ����keyҪ�����ĸ�bucket
	node_ptr first = buckets_[n];	//��firstָ��bucket��ʼ��
	for(; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {}	//���в���
	return iterator(first, this);	//����ָ��first�ĵ�����
}

// ���Ҽ�ֵΪ key �Ľڵ㣬�������������const�汾
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::const_iterator
hashtable<T, Hash, KeyEqual>::
find(const key_type& key) const {	
	const auto n = hash(key);		//��ϣ����keyҪ�����ĸ�bucket
	node_ptr first = buckets_[n];	//��firstָ��bucket��ʼ��
	for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {} //���в���
	return M_cit(first);	//����ȥ����const���Եĵ�����
}

// ���Ҽ�ֵΪ key ���ֵĴ���
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
count(const key_type& key) const {
	const auto n = hash(key);		//��ϣ����keyҪ�����ĸ�bucket
	size_type result = 0;
	for (node_ptr cur = buckets_[n]; cur; cur = cur->next) {	//����bucket
		if (is_equal(value_traits::get_key(cur->value), key))	//���ҵ����ֵΪkey��ͬ��Ԫ��
			++result;	//�����ۼ�
	}
	return result;
}

// �������ֵ key ��ȵ����䣬����һ�� pair��ָ������������β������Ԫ���ظ�
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator,
	typename hashtable<T, Hash, KeyEqual>::iterator>
	hashtable<T, Hash, KeyEqual>::
	equal_range_multi(const key_type& key) {
	const auto n = hash(key);	//��ϣ����keyҪ�����ĸ�bucket
	for (node_ptr first = buckets_[n]; first; first = first->next) {	//����bucket
		if (is_equal(value_traits::get_key(first->value), key)) {
			// ���������ȵļ�ֵ
			for (node_ptr second = first->next; second; second = second->next) {
				if(!is_equal(value_traits::get_key(second->value), key))
					// ���������ȵ�valueֵ
					return mystl::make_pair(iterator(first, this), iterator(second, this));
			}
			for (auto m = n + 1; m < bucket_size_; ++m) {
				// ����������ȣ�������һ��������ֵ�λ��
				if(buckets_[m])
					return mystl::make_pair(iterator(first, this), iterator(buckets_[m], this));
			}
			return mystl::make_pair(iterator(first, this), end());
		}
	}
	return mystl::make_pair(end(), end());	//���Ҳ���������ĩβ������
}

//�������ֵ key ��ȵ����䣬����һ�� pair��ָ������������β������Ԫ���ظ���const�汾
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
			{ // ����������ȣ�������һ��������ֵ�λ��
				if (buckets_[m])
					return mystl::make_pair(M_cit(first), M_cit(buckets_[m]));
			}
			return mystl::make_pair(M_cit(first), cend());
		}
	}
	return mystl::make_pair(cend(), cend());
}

//�������ֵ key ��ȵ����䣬����һ�� pair��ָ������������β��������Ԫ���ظ�
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
			if (first->next)	//ʹ��nextָ����б���
				return mystl::make_pair(iterator(first, this), iterator(first->next, this));
			for (auto m = n + 1; m < bucket_size_; ++m)
			{ // ����������ȣ�������һ��������ֵ�λ��
				if (buckets_[m])
					return mystl::make_pair(iterator(first, this), iterator(buckets_[m], this));
			}
			return mystl::make_pair(iterator(first, this), end());
		}
	}
	return mystl::make_pair(end(), end());
}

//�������ֵ key ��ȵ����䣬����һ�� pair��ָ������������β��������Ԫ���ظ���const�汾
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
			{ // ����������ȣ�������һ��������ֵ�λ��
				if (buckets_[m])
					return mystl::make_pair(M_cit(first), M_cit(buckets_[m]));
			}
			return mystl::make_pair(M_cit(first), cend());
		}
	}
	return mystl::make_pair(cend(), cend());
}

// ���� hashtable
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

// init ����
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
init(size_type n) {
	const auto bucket_nums = next_size(n);	//��ȡ��һ��������С����Ϊbucket�ĸ���
	try {
		buckets_.reserve(bucket_nums);		//����bucket
		buckets.assign(bucket_nums, nullptr);	//��ֵΪ��ָ��
	}
	catch (...)
	{
		bucket_size_ = 0;
		size_ = 0;
		throw;
	}
	bucket_size_ = buckets_.size();
}

// copy_init ����
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
copy_init(const hashtable& ht) {
	bucket_size_ = 0;
	buckets_.reserve(ht.bucket_size_);
	buckets_.assign(ht.bucket_size_, nullptr);
	try {
		for (size_type i = 0; i < ht.bucket_size_; ++i) {	//����bucket
			node_ptr cur = ht.buckets_[i];	//ָ��bucket��ʼ��
			if (cur) {
				// ���ĳ bucket ��������
				auto copy = create_node(cur->value);
				buckets_[i] = copy;
				for (auto next = cur->next; next; cur = next, next = cur->next) {	//����bucket�µ�list
					//��������
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

// create_node ����
template <class T, class Hash, class KeyEqual>
template <class ...Args>
typename hashtable<T, Hash, KeyEqual>::node_ptr
hashtable<T, Hash, KeyEqual>::
create_node(Args&& ...args) {
	node_ptr tmp = node_allocator::allocate(1);	//���ÿռ�
	try
	{
		data_allocator::construct(mystl::address_of(tmp->value), mystl::forward<Args>(args)...);	//����Ԫ��
		tmp->next = nullptr;
	}
	catch (...)
	{
		node_allocator::deallocate(tmp);
		throw;
	}
	return tmp;
}

// destroy_node ����
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
destroy_node(node_ptr node)
{
	data_allocator::destroy(mystl::address_of(node->value));	//����Ԫ��
	node_allocator::deallocate(node);	//�ͷſռ�
	node = nullptr;	
}

// next_size ����
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::next_size(size_type n) const
{
	return ht_next_prime(n);	//�����б���n�������
}

// hash ����
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
hash(const key_type& key, size_type n) const
{
	return hash_(key) % n;	//����������hash��������nȡģ�����������bucket��λ��
}

template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::
hash(const key_type& key) const
{
	return hash_(key) % bucket_size_;	//����������hash��������bucket_size_ȡģ�����������bucket��λ��
}

// rehash_if_need ����
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
rehash_if_need(size_type n)
{
	if (static_cast<float>(size_ + n) > (float)bucket_size_ * max_load_factor())
		//����������Ҫ���µ���bucket vector��С���͵���rehash�������¹�ϣ
		rehash(size_ + n);
}

// copy_insert ����Ԫ���ظ�
template <class T, class Hash, class KeyEqual>
template <class InputIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_multi(InputIter first, InputIter last, mystl::input_iterator_tag)
{
	rehash_if_need(mystl::distance(first, last));	//����bucket vector��С
	for (; first != last; ++first)
		insert_multi_noresize(*first);	//��������Ԫ��
}
// copy_insert ����Ԫ���ظ�
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

// copy_insert ������Ԫ���ظ�
template <class T, class Hash, class KeyEqual>
template <class InputIter>
void hashtable<T, Hash, KeyEqual>::
copy_insert_unique(InputIter first, InputIter last, mystl::input_iterator_tag)
{
	rehash_if_need(mystl::distance(first, last));
	for (; first != last; ++first)
		insert_unique_noresize(*first);
}
// copy_insert ������Ԫ���ظ�
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

// insert_node ����������Ԫ���ظ�
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::
insert_node_multi(node_ptr np) {
	const auto n= hash(value_traits::get_key(np->value));
	auto cur = buckets_[n];
	if (cur == nullptr) {	//�����ͷΪ�գ�ֱ���ڿ�ͷ����
		buckets_[n] = np;
		++size_;
		return iterator(np, this);
	}
	for (; cur; cur = cur->next) {	//����bucket����
		if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value))) { //�ҵ����Ԫ�ؾͲ���
			np->next = cur->next;
			cur->next = np;
			++size_;
			return iterator(np, this);
		}
	}
	np->next = buckets_[n];	//����������һ��bucket��ͷ���в���
	buckets_[n] = np;
	++size_;
	return iterator(np, this);
}

// insert_node_unique ������������Ԫ���ظ�
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::
insert_node_unique(node_ptr np)
{
	const auto n = hash(value_traits::get_key(np->value));
	auto cur = buckets_[n];
	if (cur == nullptr)		//bucket��ͷΪ�գ�ֱ�Ӳ���
	{
		buckets_[n] = np;
		++size_;
		return mystl::make_pair(iterator(np, this), true);
	}
	for (; cur; cur = cur->next)	//����bucket
	{
		if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value)))
		{	//�����Ԫ��ֱ�ӷ���false
			return mystl::make_pair(iterator(cur, this), false);
		}
	}
	//����Ѱ����һ��bucket��ͷ���в���
	np->next = buckets_[n];
	buckets_[n] = np;
	++size_;
	return mystl::make_pair(iterator(np, this), true);
}

// replace_bucket ���� ����bucket��С
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
replace_bucket(size_type bucket_count) {
	bucket_type bucket(bucket_count);	//��ȡ��Ҫ��bucketԪ�ظ���
	if (size_ != 0) {
		for (size_type i = 0; i < bucket_size_; ++i;) {	//����bucket vector
			for (auto first = buckets_[i]; first; first = first->next) {	//����bucket
				auto tmp = create_node(first->value);	//�����½ڵ�
				const auto n=hash(value_traits::get_key(first->value), bucket_count);	//��ϣ����
				auto f = bucket[n];		//bucket��ͷ
				bool is_inserted = false;
				for (auto cur = f; cur; cur = cur->next) {	//���б���
					if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(first->value))) {
						//��ȾͲ���Ԫ��
						tmp->next = cur->next;
						cur->next = tmp;
						is_inserted = true;
						break;
					}
				}
				if (!is_inserted) {
					//������һ��bucket
					tmp->next = f;
					bucket[n] = tmp;
				}
			}
		}
	}
	buckets_.swap(bucket);			//��������bucket
	bucket_size_ = buckets_.size();
}

// erase_bucket ����
// �ڵ� n �� bucket �ڣ�ɾ�� [first, last) �Ľڵ�
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase_bucket(size_type n, node_ptr first, node_ptr last) {
	auto cur = buckets_[n];	//bucket��ͷ
	if (cur == first) {		//first����bucket��ͷ��ֱ��ɾ��
		erase_bucket(n, last);
	}
	else {
		node_ptr next = cur->next;	//������һ��bucket
		for(;next!=first;cur=next,next=cur->next){}	//����bucket
		while (next!=last)
		{	//����������ɾ��
			cur->next = next->next;
			destroy_node(next);
			next = cur->next;
			--size_;
		}
	}
}

// erase_bucket ����
// �ڵ� n �� bucket �ڣ�ɾ�� [buckets_[n], last) �Ľڵ�
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
erase_bucket(size_type n, node_ptr last) {
	auto cur = buckets_[n];
	while (cur!=last)
	{	//����ɾ��
		auto next = cur->next;
		destroy_node(cur);
		cur = next;
		--size_;
	}
	buckets_[n] = last;
}

// equal_to �����������ظ�Ԫ��
template <class T, class Hash, class KeyEqual>
bool hashtable<T, Hash, KeyEqual>::equal_to_multi(const hashtable& other)
{
	if (size_ != other.size_)	//����hashtable��С�����
		return false;
	for (auto f = begin(), l = end(); f != l;)	//����hashtable
	{	//���Ԫ�صıȽ�
		auto p1 = equal_range_multi(value_traits::get_key(*f));
		auto p2 = other.equal_range_multi(value_traits::get_key(*f));
		if (mystl::distance(p1.first, p1.last) != mystl::distance(p2.first, p2.last) ||
			!mystl::is_permutation(p1.first, p2.last, p2.first, p2.last))
			//���ҵ�ĩβ
			return false;
		f = p1.last;
	}
	return true;
}

// equal_to �������������ظ�Ԫ��
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

// ���� mystl �� swap
template <class T, class Hash, class KeyEqual>
void swap(hashtable<T, Hash, KeyEqual>& lhs,
	hashtable<T, Hash, KeyEqual>& rhs) noexcept
{
	lhs.swap(rhs);
}

}	//�����ռ������

#endif // !MYSTL_HASHTABLE_H_

