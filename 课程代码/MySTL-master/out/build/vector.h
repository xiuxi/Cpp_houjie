#ifndef MYSTL_VECTOR_H_
#define MYSTL_VECTOR_H_
// ���ͷ�ļ�����һ��ģ���� vector
// vector : ����

// notes:
//
// �쳣��֤��
// mystl::vecotr<T> ��������쳣��֤�����ֺ������쳣��֤���������º�����ǿ�쳣��ȫ��֤��
//   * emplace
//   * emplace_back
//   * push_back
// �� std::is_nothrow_move_assignable<T>::value == true ʱ�����º���Ҳ����ǿ�쳣��֤��
//   * reserve
//   * resize
//   * insert

#include <initializer_list>		//��ʼ���б�

#include "iterator.h"			//������
#include "memory.h"				//�߼��Ķ�̬�ڴ����
#include "util.h"				//ͨ�ù���
#include "exceptdef.h"			//�쳣

namespace mystl {		//�����ռ�mystl

//���Ԥ�������Ĺ����ǣ�(#ifdef max) �����ʶ�� max �Ѿ��ú궨����й����壬��ôִ����������
//�ڱ���������ڴ�ӡ�ı� ��#undefing marco max�� (#pragma message("#undefing marco max"))
//ɾ�����ȶ���ĺ궨�� max (#undef max)
#ifdef max			
#pragma message("#undefing marco max")
#undef max
#endif // max

//Ԥ�������min��maxͬ��
#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

// ģ����: vector 
// ģ����� T ��������
template<class T>class vector {
//��̬���� static_assert,�ο����ӣ�https://blog.csdn.net/weixin_38292570/article/details/108479251
//��̬�����Ǳ����ڼ�Ķ��ԣ���˽�����̬���ԡ����﷨�ܼ򵥣�static_assert(�������ʽ����ʾ�ַ���)�������һ������
//�������ʽ��ֵΪfalse�������һ��������󣬴���λ�þ��Ǹ�static_assert��������У��ڶ����������Ǵ�����ʾ�ַ���
//�þ�̬���ԵĴ��뺬�壺�������ģ��������bool�Ļ����ͻ�������Դ��󣬲��ڱ��봰�ڴ�ӡ"vector is abandoned in mystl"
	static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");
public:
	// vector ��Ƕ���ͱ���
	typedef mystl::allocator<T>		allocator_type;	
	//ֻ�ܶ��嵽��һ�������ܶ���mystl::allocator<T>::allocate allocate;
	typedef mystl::allocator<T>		data_allocator;
	
	//������Ƕ�ĸ����ͱ�
	typedef typename allocator_type::value_type		 value_type;
	typedef typename allocator_type::pointer         pointer;
	typedef typename allocator_type::const_pointer   const_pointer;
	typedef typename allocator_type::reference       reference;
	typedef typename allocator_type::const_reference const_reference;
	typedef typename allocator_type::difference_type difference_type;
	typedef typename allocator_type::size_type       size_type;		//���ʹ�С

	//value_type������ͱ���ȡ
	typedef value_type* iterator;	//����������ָͨ�룬��Ϊvector�������洢�ռ�
	typedef const value_type* const_iterator;
	typedef mystl::reverse_iterator<iterator> reverse_iterator;		//����ָ��
	typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;

	allocator_type get_allocator() { return data_allocator(); }	//��ȡ���ݹ������

private:
	//��������ָ�빲12bytes������һ����ʼ����vector����Ĵ�С
	iterator begin_;	//��ʾĿǰʹ�ÿռ��ͷ��
	iterator end_;		// ��ʾĿǰʹ�ÿռ��β��
	iterator cap_;		// ��ʾĿǰ���ÿռ��β��

public:
	// ���졢���ơ��ƶ�����������
	vector()noexcept { try_init(); }//��������noexcept����ʾ���try_init()�������쳣�����캯��vector()һ���������쳣
	explicit vector(size_type n){ fill_init(n, value_type()); }	//���û������ͣ���int���Ĺ��캯��������ֵΪ0
	vector(size_type n, const value_type& value){fill_init(n, value);}	//fill_init�����������붨��������

	//��������Χ��ʼ��
	template <class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
	vector(Iter first, Iter last){	
		//iterator��T*�����Ǵ��ο�����const T*������ʹ����һ������Iter�������������ƶϲ�������
		MYSTL_DEBUG(!(last < first));	//MYSTL_DEBUG����������exceptdef.h��
		range_init(first, last);		//range_init���������붨��������
	}
	
	//�������캯��
	vector(const vector& rhs) {
		range_init(rhs.begin_, rhs.end_);
	}

	//�ƶ����캯����ʹ�ó�ʼֵ�б�,�ƶ����캯����Ϊ�϶���ı䴫��������ԺͿ������캯����ͬ������������const
	vector(vector&& rhs)noexcept:begin_(rhs.begin_),end_(rhs.end_),cap_(rhs.cap_) {
		//������ָ��ĸ�ֵ����û�������ڴ棬���Ұ���ֵ��ָ���ÿ�
		rhs.begin_ = nullptr;
		rhs.end_ = nullptr;
		rhs.cap_ = nullptr;
	}

	//��ʼ���б��ʼ��
	vector(std::initializer_list<value_type> ilist)
	{
		range_init(ilist.begin(), ilist.end());	//begin()���ص���const T*����
	}

	//����������
	vector& operator=(const vector& rhs);
	vector& operator=(vector&& rhs)noexcept;

	vector& operator=(std::initializer_list<value_type>ilist) {
		vector tmp(ilist.begin(), ilist.end());
		swap(tmp);
		return	*this;
	}

	//��������
	~vector() {
		destroy_and_recover(begin_, end_, cap_ - begin_);	//destroy_and_recover�����������붨��������
		begin_ = end_ = cap_ = nullptr;				//��������Ϊ��ָ��
	}

public:
	//��������ز���
	iterator begin() noexcept { return begin_; }
	const_iterator begin()const noexcept { return begin_; }
	iterator end() noexcept { return end_; }
	const_iterator end()const noexcept { return end_; }

	//�����������ز���
	reverse_iterator rbegin()noexcept { return reverse_iterator(end()); }	//rbegin()����end()
	const_reverse_iterator rbegin()const noexcept { return reverse_iterator(end()); }
	reverse_iterator rend()noexcept { return reverse_iterator(begin()); }	//rend()����begin()
	const_reverse_iterator rend()const noexcept { return reverse_iterator(begin()); }

	//const��������ز�������װ���涨��ĵ�������غ���
	const_iterator cbegin()const noexcept { return begin(); }	
	const_iterator cend()const noexcept { return end(); }
	const_reverse_iterator crbegin()const noexcept { return rbegin(); }
	const_reverse_iterator crend()const noexcept { return rend(); }

	//������ز���
	bool empty()const noexcept { return begin_ == end_; }
	//static_cast����ת������Ϊ��֪��end_��begin_��������ʲô����
	size_type size()const noexcept { return static_cast<size_type>(end_ - begin_); } //Ŀǰʹ�ÿռ��С
	size_type max_size()const noexcept { return static_cast<size_type>(-1) / sizeof(T); }
	size_type capacity()const noexcept { return static_cast<size_type>(cap_ - begin_); }	//����������С
	void reserve(size_type n);	//���·���ռ�
	void shrink_to_fit();		//��������ռ�

	 // ����Ԫ����ز���
	reference operator[](size_type n) {		//�±������������
		MYSTL_DEBUG(n < size());
		return *(begin_ + n);
	}
	const_reference operator[](size_type n)const {
		MYSTL_DEBUG(n < size());
		return *(begin_ + n);
	}

	reference at(size_type n) {			//�ж��±�����Ƿ�Ϸ����Ϸ��򷵻��±�Ԫ��
		//�±곬����Χ���׳��쳣
		THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
		return (*this)[n];
	}
	const_reference at(size_type n) const
	{
		THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
		return (*this)[n];
	}

	reference front() {			//��ȡ��Ԫ��
		MYSTL_DEBUG(!empty());
		return *begin_;
	}
	const_reference front() const
	{
		MYSTL_DEBUG(!empty());
		return *begin_;
	}
	reference back() {
		MYSTL_DEBUG(!empty());
		return *(end_ - 1);
	}
	const_reference back() const	//��ȡβԪ��
	{
		MYSTL_DEBUG(!empty());
		return *(end_ - 1);
	}
	pointer       data()	  noexcept { return begin_; }	//��ȡ������ָ��
	const_pointer data()const noexcept { return begin_; }

	// �޸�������ز���
	// assign ��ֵ����
	void assign(size_type n, const value_type& value) { fill_assign(n, value); }  // assign��vector��ֵΪn��value
	template <class Iter,typename std::enable_if<mystl::is_input_iterator<Iter>::value,int>::type=0>
	void assign(Iter first, Iter last) {	//enable_if �������ж�ģ�����������
		MYSTL_DEBUG(!(last < first));
		copy_assign(first, last, iterator_category(first));
	}
	void assign(std::initializer_list<value_type>il){
		copy_assign(il.begin(), il.end(), mystl::forward_iterator_tag{});	//��ʼ���б�ֵ
	}

	// emplace������ָ��λ�ò���Ԫ�� / emplace_back  ������ĩβ����Ԫ��
	template<class...Args>iterator emplace(const_iterator pos, Args&& ...args);
	template<class...Args>void emplace_back(Args&& ...args);

	// push_back ����ĩβ����Ԫ�� / pop_back ����ĩβɾ��Ԫ��
	void push_back(const value_type& value);
	void push_back(value_type&& value) { emplace_back(mystl::move(value)); }	//��ֵ���ò���
	void pop_back();

	// insert ����Ԫ��
	iterator insert(const_iterator pos, const value_type& value);
	iterator insert(const_iterator pos, value_type&& value) {		//��ֵ���ò���
		return emplace(pos, mystl::move(value));
	}
	iterator insert(const_iterator pos, size_type n, const value_type& value) {
		MYSTL_DEBUG(pos >= begin() && pos <= end());
		//const_castȥ������������const��volatile���ԣ�û������ȥ����
		return fill_insert(const_cast<iterator>(pos), n.value);	
	}
	template <class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
		void insert(const_iterator pos, Iter first, Iter last){
		MYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
		copy_insert(const_cast<iterator>(pos), first, last);
	}

	// erase ���ĳ��λ���ϵ�Ԫ��  / clear �������Ԫ��
	iterator erase(const_iterator pos);
	iterator erase(const_iterator first, const_iterator last);
	void clear() { erase(begin(), end()); }

	// resize ���������Ĵ�С��ʹ�����n��Ԫ�� / reverse ���ڷ�ת��[first,last)��Χ�ڵ�˳��
	void resize(size_type new_size) { return resize(new_size, value_type()); }
	void resize(size_type new_size, const value_type& value);
	void reverse() { mystl::reverse(begin(), end()); }

	// swap ����Ԫ��
	void swap(vector& rhs)noexcept;

private:
	 // helper functions����

	// initialize ��ʼ�� / destroy ����
	void try_init()noexcept;
	void init_space(size_type size, size_type cap);
	void fill_init(size_type n, const value_type& value);
	template <class Iter>void range_init(Iter first, Iter last);
	void destroy_and_recover(iterator first, iterator last, size_type n);

	// calculate the growth size����������ģ
	size_type get_new_cap(size_type add_size);

	// assign
	void fill_assign(size_type n, const value_type& value);
	template <class IIter>void copy_assign(IIter first, IIter last, input_iterator_tag);
	template <class FIter>void copy_assign(FIter first, FIter last, forward_iterator_tag);

	// reallocate ���·����ڴ�ռ�
	template <class... Args>void reallocate_emplace(iterator pos, Args&& ...args);
	void reallocate_insert(iterator pos, const value_type& value);

	// insert ���뺯��
	iterator fill_insert(iterator pos, size_type n, const value_type& value);
	template<class IIter>void copy_insert(iterator pos, IIter first, IIter last);

	// shrink_to_fit��������ռ�
	void reinsert(size_type size);

};		//ģ���������

/*****************************************************************************************/

// ���Ƹ�ֵ������
template<class T>vector<T>& vector<T>::operator=(const vector& rhs) {
	if (this != &rhs) {		//�������Լ����Լ���ֵ
		const auto len = rhs.size();
		if (len > capaticy()) {		//��ǰvector��������������Ҫ����
			vector tmp(rhs.begin(), rhs.end());		
			swap(tmp);		//this����һ��vector����
		}
		else if (size() >= len) {	//��ǰvector���ݱ�rhs������Ҫ����ǰvector�������������
			auto i = mystl::copy(rhs.begin(), rhs.end(), begin());
			data_allocator::destroy(i, end_);	//���ٷ�Χ�ڵ�Ԫ�أ����������ͷſռ�
			end_ = begin_ + len;
		}
		else {
			mystl::copy(rhs.begin(), rhs.begin() + size(), begin_);
			mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);	//�����ಿ������
			cap_ = end_ = begin_ + len;		//����ÿռ���ʹ�ÿռ����
		}
	}
	return *this;
}

// �ƶ���ֵ������
template<class T> vector<T>& vector<T>::operator=(vector&& rhs)noexcept {
	destroy_and_recover(begin_, end_, cap_ - begin_);	//Ҫ�Ƚ�֮ǰbegin_ָ����ڴ�ռ��ͷŵ�
	begin_ = rhs.begin_;
	end_ = rhs.end_;
	cap_ = rhs.cap_;
	rhs.begin_ = rhs.end_ = rhs.cap_ = nullptr;
	return *this;
}

// Ԥ���ռ��С����ԭ����С��Ҫ���Сʱ���Ż����·���
template<class T>void vector<T>::reserve(size_type n) {
	if (capacity() < n) {
		THROW_LENGTH_ERROR_IF(n > max_size(),"n can not larger than max_size() in vector<T>::reserve(n)");
		const auto old_size = size();
		auto tmp = data_allocator::allocate(n);				//�����µ��ڴ�ռ�
		mystl::uninitialized_move(begin_, end_, tmp);		//��ԭvector�������ƶ�����vector
		data_allocator::deallocate(begin_, cap_ - begin_);	//�ͷ�ԭvector
		//������������ָ����vector
		begin_ = tmp;
		end_ = tmp + old_size;
		cap_ = begin_ + n;
	}
}

// �������������
template<class T>void vector<T>::shrink_to_fit() {
	if (end_ < cap_) {
		reinsert(size());
	}
}

// �� pos λ�þ͵ع���Ԫ�أ��������ĸ��ƻ��ƶ�����
template <class T> template <class ...Args>	typename vector<T>::iterator 
vector<T>::emplace(const_iterator pos, Args&& ...args) {
	MYSTL_DEBUG(pos >= begin() && pos <= end());
	iterator xpos = const_cast<iterator>(pos);	//ȥconst����
	const size_type n = xpos - begin_;
	if (end_ != cap_ && xpos == end_) {			//��vecotrĩβ���룬�൱��emplace_back
		//address_of()����ȡ����ĵ�ַ������ֱ�Ӵ�end_Ҳ����
		//construct��������ָ��λ�ã����ݴ���Ĳ����������󣬵��õķ�����::new ((void*)pos) T(args...)
		data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
		++end_;	//����ˮλ
	}
	else if (end_ != cap) {	//����δʹ�ÿռ�
		auto new_end = end_;
		//�ڱ��ÿռ���ʼ������һ��Ԫ�أ�����vector���һ��Ԫ��Ϊ���ֵ
		data_allocator::construct(mystl::address_of(*end_), *(end_ - 1)); 
		++new_end;	//����ˮλ
		mystl::copy_backward(xpos, end_ - 1, end_);	//emplaceͬ�������ݰ��˵Ĳ���
		*xpos = value_type(mystl::forward<Args>(args)...);
	}
	else {	//����ռ䲻�������·���ռ䣬���� pos ���͵ع���Ԫ��
		reallocate_emplace(xpos, mystl::forward<Args>(args)...);
	}
	return begin() + n;
}

// ��β���͵ع���Ԫ�أ��������ĸ��ƻ��ƶ�����
template <class T> template <class ...Args> void vector<T>::emplace_back(Args&& ...args) {
	if (end_ < cap_) {
		data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
		++end_;
	}
	else {
		reallocate_emplace(end_, mystl::forward<Args>(args)...);
	}
}

// ��β������Ԫ��
template<class T>void vector<T>::push_back(const value_type& value) {
	if (end_ != cap_)
	{
		data_allocator::construct(mystl::address_of(*end_), value);
		++end_;
	}
	else
	{
		reallocate_insert(end_, value);
	}
}

// ����β��Ԫ��
template <class T>
void vector<T>::pop_back()
{
	MYSTL_DEBUG(!empty());
	data_allocator::destroy(end_ - 1);
	--end_;		//��β�˱����ǰ��һ�񣬱�ʾ������β��Ԫ��
}

// emplace��insert��ȣ�ֻ����Ϊ���ζ����Ǵ������������˹���һ����ʱ����Ĳ���
// �� pos ������Ԫ��
template <class T> typename vector<T>::iterator vector<T>::insert(const_iterator pos, const value_type& value) {
	MYSTL_DEBUG(pos >= begin() && pos <= end());
	iterator xpos = const_cast<iterator>(pos);	//ȥconst
	const size_type n = pos - begin_;
	if (end_ != cap_ && xpos == end_)	//��vecotrĩβ����
	{
		//address_of()����ȡ����ĵ�ַ������ֱ�Ӵ�end_Ҳ����
		//construct��������ָ��λ�ã����ݴ���Ĳ����������󣬵��õķ�����::new ((void*)pos) T(args...)
		data_allocator::construct(mystl::address_of(*end_), value);
		++end_;	//����ˮλ
	}
	else if (end_ != cap_)	//����δʹ�ÿռ�
	{
		auto new_end = end_;
		//�ڱ��ÿռ���ʼ������һ��Ԫ�أ�����vector���һ��Ԫ��Ϊ���ֵ
		data_allocator::construct(mystl::address_of(*end_), *(end_ - 1));
		++new_end;
		auto value_copy = value;  // ����Ԫ�������¸��Ʋ��������ı�
		mystl::copy_backward(xpos, end_ - 1, end_);		//���ݰ��˵Ĳ���
		*xpos = mystl::move(value_copy);
		end_ = new_end;
	}
	else  //����ռ䲻�������·���ռ䣬���� pos ���͵ع���Ԫ��
	{
		reallocate_insert(xpos, value);
	}
	return begin_ + n;
}

// ɾ�� pos λ���ϵ�Ԫ��
template <class T> typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
	MYSTL_DEBUG(pos >= begin() && pos < end());
	iterator xpos = begin_ + (pos - begin());
	mystl::move(xpos + 1, end_, xpos);		//��pos�����Ԫ��ǰ��
	data_allocator::destroy(end_ - 1);		//����Ҫɾ����Ԫ��
	--end_;									//����������ָ��ʵ��λ��
	return xpos;
}

// ɾ��[first, last)�ϵ�Ԫ��
template <class T> typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
	MYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
	const auto n = first - begin();
	iterator r = begin_ + (first - begin());	//��ȡfirstǰһ��λ�õ�����
	//��last����Ԫ��Ǩ�ƣ�������[first, last)�ϵ�Ԫ��
	data_allocator::destroy(mystl::move(r + (last - first), end_, r), end_);	
	end_ = end_ - (last - first);	//����β��λ��
	return begin_ + n;
}

// ����������С
template <class T> void vector<T>::resize(size_type new_size, const value_type& value) {
	if (new_size < size()) {
		erase(begin() + new_size, end());	//�ض϶���Ŀռ�
	}
	else {
		insert(end(), new_size - size(), value);	//����ĩβ���Ԫ��
	}
}

// ����һ�� vector ����
template <class T>void vector<T>::swap(vector<T>& rhs)noexcept {
	if (this != &rhs) {
		mystl::swap(begin_, rhs.begin_);
		mystl::swap(end_, rhs.end_);
		mystl::swap(cap_, rhs.cap_);
	}
}

/*****************************************************************************************/
// helper function����

// try_init ������������ʧ������ԣ����׳��쳣
template <class T>void vector<T>::try_init()noexcept {
	try {
		begin_ = data_allocator::allocate(16);	//Ĭ�Ϸ���16��Ԫ�ؿռ䣬begin_ָ����ʹ�ÿռ�Ŀ�ͷ
		end_ = begin_;							//end_ָ����ʹ�ÿռ��β��
		cap_ = begin_ + 16;	//cap_ָ���ܷ���ռ��β��������+16��������+16*sizeof(T)����Ϊָ���ƶ��������������ʹ�С
	}
	catch (...) {
		begin_ = nullptr;
		end_ = nullptr;
		cap_ = nullptr;
	}
}

// init_space ����
template <class T>
void vector<T>::init_space(size_type size, size_type cap)
{
	try
	{
		begin_ = data_allocator::allocate(cap);		//�Դ���Ŀ��ÿռ������Ϊ��ʼ����С
		end_ = begin_ + size;
		cap_ = begin_ + cap;
	}
	catch (...)
	{
		begin_ = nullptr;
		end_ = nullptr;
		cap_ = nullptr;
		throw;
	}
}

// fill_init ����
template <class T> void vector<T>::fill_init(size_type n, const value_type& value){
	const size_type init_size = mystl::max(static_cast<size_type>(16), n);
	init_space(n, init_size);
	mystl::uninitialized_fill_n(begin_, n, value);	//��begin_λ�����n��value
}

// range_init ����
//���������class����д��һ�����棬T����ģ���ṩ�ģ�Iter�Ǻ���ģ���ṩ��
//��ģ�����ж�����һ��ģ�庯�������ⶨ����������������ʽ
template <class T> template <class Iter> void vector<T>::range_init(Iter first, Iter last){
	const size_type init_size = mystl::max(static_cast<size_type>(last - first),static_cast<size_type>(16));
	init_space(static_cast<size_type>(last - first), init_size);
	mystl::uninitialized_copy(first, last, begin_); // ��first��last��Χ�ڵ�Ԫ�ؿ�����begin_��
}

// destroy_and_recover ����
template <class T> void vector<T>::destroy_and_recover(iterator first, iterator last, size_type n){
	data_allocator::destroy(first, last);		//����Ԫ��
	data_allocator::deallocate(first, n);		//�ͷſռ�
}

// get_new_cap ����
template <class T> typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size){
	const auto old_size = capacity();
	THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size,"vector<T>'s size too big");
	//����Ҫ���ӵ�vector��Сȥ����vector�Ŀ��ÿռ��С
	//�ɳ��ȵ���������ɳ��� + ����Ԫ�ظ���
	if (old_size > max_size() - old_size / 2)
	{
		return old_size + add_size > max_size() - 16? old_size + add_size : old_size + add_size + 16;
	}
	const size_type new_size = old_size == 0 ? mystl::max(add_size, static_cast<size_type>(16))
		: mystl::max(old_size + old_size / 2, old_size + add_size);
	return new_size;
}

// fill_assign ����
template <class T> void vector<T>::fill_assign(size_type n, const value_type& value){
	if (n > capacity())		//���ÿռ䲻�㣬�����¹���vector����Ԫ�ؽ�������
	{
		vector tmp(n, value);
		swap(tmp);
	}
	else if (n > size())	//���ڵ�ǰʹ�ÿռ䣬���ڽ�β����Ԫ��
	{
		mystl::fill(begin(), end(), value);
		end_ = mystl::uninitialized_fill_n(end_, n - size(), value);
	}
	else
	{
		erase(mystl::fill_n(begin_, n, value), end_);	//ɾ��ĩβ�����������
	}
}

// copy_assign ����
template <class T> template <class IIter> void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag){
	auto cur = begin_;
	for (; first != last && cur != end_; ++first, ++cur)	//����first��last��Ԫ��
	{
		*cur = *first;										//��[first��last)������ȥ
	}
	if (first == last)
	{
		erase(cur, end_);									//ɾ���������Ԫ�أ�������[first��last)֮���Ԫ��
	}
	else
	{
		insert(end_, first, last);							//�ڵ�ǰʹ�ÿռ��β����[first��last)֮���Ԫ��
	}
}

// �� [first, last) Ϊ������ֵ
template <class T> template <class FIter> void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag){
	const size_type len = mystl::distance(first, last);	//��ȡ����Ԫ�س���
	if (len > capacity())		//����Ԫ�ش��ڿ��ÿռ䣬���¹���vector������Ԫ��
	{
		vector tmp(first, last);
		swap(tmp);
	}
	else if (size() >= len)		//��������Ԫ�س��ȴ�������Ԫ�س���
	{
		auto new_end = mystl::copy(first, last, begin_);	
		data_allocator::destroy(new_end, end_);		//������β�˱�����β�˱��֮��Ԫ��
		end_ = new_end;			//β�˱��ǰ��
	}
	else
	{
		auto mid = first;		
		mystl::advance(mid, size());	//���·���һ�δ�С������������Ԫ������
		mystl::copy(first, mid, begin_);	//Ԫ�ؿ���
		auto new_end = mystl::uninitialized_copy(mid, last, end_);	
		end_ = new_end;
	}
}

// ���·���ռ䲢�� pos ���͵ع���Ԫ��
template <class T> template <class ...Args> void vector<T>::reallocate_emplace(iterator pos, Args&& ...args){
	const auto new_size = get_new_cap(1);
	auto new_begin = data_allocator::allocate(new_size);
	auto new_end = new_begin;
	try
	{
		new_end = mystl::uninitialized_move(begin_, pos, new_begin);
		data_allocator::construct(mystl::address_of(*new_end), mystl::forward<Args>(args)...);
		++new_end;
		new_end = mystl::uninitialized_move(pos, end_, new_end);
	}
	catch (...)
	{
		data_allocator::deallocate(new_begin, new_size);
		throw;
	}
	destroy_and_recover(begin_, end_, cap_ - begin_);
	begin_ = new_begin;
	end_ = new_end;
	cap_ = new_begin + new_size;
}

// ���·���ռ䲢�� pos ������Ԫ��
template <class T> void vector<T>::reallocate_insert(iterator pos, const value_type& value){
	const auto new_size = get_new_cap(1);
	auto new_begin = data_allocator::allocate(new_size);
	auto new_end = new_begin;
	const value_type& value_copy = value;
	try
	{
		new_end = mystl::uninitialized_move(begin_, pos, new_begin);
		data_allocator::construct(mystl::address_of(*new_end), value_copy);
		++new_end;
		new_end = mystl::uninitialized_move(pos, end_, new_end);
	}
	catch (...)
	{
		data_allocator::deallocate(new_begin, new_size);
		throw;
	}
	destroy_and_recover(begin_, end_, cap_ - begin_);
	begin_ = new_begin;
	end_ = new_end;
	cap_ = new_begin + new_size;
}

// fill_insert ����
template <class T> typename vector<T>::iterator vector<T>::
fill_insert(iterator pos, size_type n, const value_type& value){
	if (n == 0)
		return pos;
	const size_type xpos = pos - begin_;
	const value_type value_copy = value;  // ���ⱻ����
	if (static_cast<size_type>(cap_ - end_) >= n)
	{ // ������ÿռ���ڵ������ӵĿռ�
		const size_type after_elems = end_ - pos;	//��������֮�������Ԫ�ظ���
		auto old_end = end_;
		if (after_elems > n)	//�����֮�������Ԫ�ظ�����������Ԫ�ظ���
		{
			mystl::uninitialized_copy(end_ - n, end_, end_);	//�����ÿռ�����������ʹ�ÿռ���
			end_ += n;	//��vectorβ�˱�Ǻ���
			mystl::move_backward(pos, old_end - n, old_end);	//����ƶ������֮���Ԫ�أ��ճ��ռ��Ԫ�ز���
			mystl::uninitialized_fill_n(pos, n, value_copy);	//�Ӳ���㿪ʼ������ֵ
		}
		else					//�����֮�������Ԫ�ظ���С�ڵ�������Ԫ�ظ���
		{
			end_ = mystl::uninitialized_fill_n(end_, n - after_elems, value_copy);	//�ڱ��ÿռ��������Ԫ��
			end_ = mystl::uninitialized_move(pos, old_end, end_);				//�����֮�������Ԫ���ƶ������ÿռ�
			mystl::uninitialized_fill_n(pos, after_elems, value_copy);			//�����֮���������Ԫ��
		}
	}
	else { // ������ÿռ䲻�㣬С������Ԫ�ظ���
		//���Ⱦ����³��ȣ��ɳ��ȵ���������ɳ���+����Ԫ�ظ���
		const auto new_size = get_new_cap(n);
		//�����µ�vector�ռ�
		auto new_begin = data_allocator::allocate(new_size);
		auto new_end = new_begin;
		try
		{
			//����vector�Ĳ����֮ǰ��Ԫ���ƶ����¿ռ�
			new_end = mystl::uninitialized_move(begin_, pos, new_begin);
			//������Ԫ��(��ֵ��Ϊn)�����¿ռ�
			new_end = mystl::uninitialized_fill_n(new_end, n, value);
			//����vector�Ĳ����֮���Ԫ�ظ��Ƶ��¿ռ�
			new_end = mystl::uninitialized_move(pos, end_, new_end);
		}
		catch (...)
		{
			destroy_and_recover(new_begin, new_end, new_size);
			throw;
		}
		//������ͷžɵ�vector
		data_allocator::deallocate(begin_, cap_ - begin_);
		//����������λ��
		begin_ = new_begin;
		end_ = new_end;
		cap_ = begin_ + new_size;
	}
	return begin_ + xpos;
}

// copy_insert ����������[first,last��
template <class T> template <class IIter> void vector<T>::copy_insert(iterator pos, IIter first, IIter last){
	if (first == last)
		return;
	const auto n = mystl::distance(first, last);
	if ((cap_ - end_) >= n)
	{ // ������ÿռ��С�㹻
		const auto after_elems = end_ - pos;
		auto old_end = end_;
		if (after_elems > n)
		{
			end_ = mystl::uninitialized_copy(end_ - n, end_, end_);
			mystl::move_backward(pos, old_end - n, old_end);
			mystl::uninitialized_copy(first, last, pos);
		}
		else
		{
			auto mid = first;
			mystl::advance(mid, after_elems);
			end_ = mystl::uninitialized_copy(mid, last, end_);
			end_ = mystl::uninitialized_move(pos, old_end, end_);
			mystl::uninitialized_copy(first, mid, pos);
		}
	}
	else{ // ���ÿռ䲻��
		const auto new_size = get_new_cap(n);
		auto new_begin = data_allocator::allocate(new_size);
		auto new_end = new_begin;
		try
		{
			new_end = mystl::uninitialized_move(begin_, pos, new_begin);
			new_end = mystl::uninitialized_copy(first, last, new_end);
			new_end = mystl::uninitialized_move(pos, end_, new_end);
		}
		catch (...)
		{
			destroy_and_recover(new_begin, new_end, new_size);
			throw;
		}
		data_allocator::deallocate(begin_, cap_ - begin_);
		begin_ = new_begin;
		end_ = new_end;
		cap_ = begin_ + new_size;
	}
}

// reinsert ����
template <class T> void vector<T>::reinsert(size_type size){
	auto new_begin = data_allocator::allocate(size);
	try
	{
		mystl::uninitialized_move(begin_, end_, new_begin);
	}
	catch (...)
	{
		data_allocator::deallocate(new_begin, size);
		throw;
	}
	data_allocator::deallocate(begin_, cap_ - begin_);
	begin_ = new_begin;
	end_ = begin_ + size;
	cap_ = begin_ + size;
}

/*****************************************************************************************/
// ���رȽϲ�����

template <class T>bool operator==(const vector<T>& lhs, const vector<T>& rhs){
	return lhs.size() == rhs.size() &&mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>bool operator<(const vector<T>& lhs, const vector<T>& rhs){
	return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), lhs.end());
}

template <class T>bool operator!=(const vector<T>& lhs, const vector<T>& rhs){
	return !(lhs == rhs);
}

template <class T>bool operator>(const vector<T>& lhs, const vector<T>& rhs){
	return rhs < lhs;
}

template <class T>bool operator<=(const vector<T>& lhs, const vector<T>& rhs){
	return !(rhs < lhs);
}

template <class T>bool operator>=(const vector<T>& lhs, const vector<T>& rhs){
	return !(lhs < rhs);
}

// ���� mystl �� swap
template <class T>void swap(vector<T>& lhs, vector<T>& rhs){
	lhs.swap(rhs);
}

}		//�����ռ������

#endif // !MYSTL_VECTOR_H_

