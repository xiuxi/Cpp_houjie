#ifndef MYSTL_VECTOR_H_
#define MYSTL_VECTOR_H_
// 这个头文件包含一个模板类 vector
// vector : 向量

// notes:
//
// 异常保证：
// mystl::vecotr<T> 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证：
//   * emplace
//   * emplace_back
//   * push_back
// 当 std::is_nothrow_move_assignable<T>::value == true 时，以下函数也满足强异常保证：
//   * reserve
//   * resize
//   * insert

#include <initializer_list>		//初始化列表

#include "iterator.h"			//迭代器
#include "memory.h"				//高级的动态内存管理
#include "util.h"				//通用工具
#include "exceptdef.h"			//异常

namespace mystl {		//命名空间mystl

//这段预编译代码的功能是：(#ifdef max) 如果标识符 max 已经用宏定义进行过定义，那么执行下面内容
//在编译输出窗口打印文本 “#undefing marco max” (#pragma message("#undefing marco max"))
//删除事先定义的宏定义 max (#undef max)
#ifdef max			
#pragma message("#undefing marco max")
#undef max
#endif // max

//预处理代码min与max同理
#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

// 模板类: vector 
// 模板参数 T 代表类型
template<class T>class vector {
//静态断言 static_assert,参考链接：https://blog.csdn.net/weixin_38292570/article/details/108479251
//静态断言是编译期间的断言，因此叫做静态断言。其语法很简单：static_assert(常量表达式，提示字符串)。如果第一个参数
//常量表达式的值为false，会产生一条编译错误，错误位置就是该static_assert语句所在行，第二个参数就是错误提示字符串
//该静态断言的代码含义：如果传入模板类型是bool的话，就会产生断言错误，并在编译窗口打印"vector is abandoned in mystl"
	static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");
public:
	// vector 的嵌套型别定义
	typedef mystl::allocator<T>		allocator_type;	
	//只能定义到这一级，不能定义mystl::allocator<T>::allocate allocate;
	typedef mystl::allocator<T>		data_allocator;
	
	//定义内嵌的各种型别
	typedef typename allocator_type::value_type		 value_type;
	typedef typename allocator_type::pointer         pointer;
	typedef typename allocator_type::const_pointer   const_pointer;
	typedef typename allocator_type::reference       reference;
	typedef typename allocator_type::const_reference const_reference;
	typedef typename allocator_type::difference_type difference_type;
	typedef typename allocator_type::size_type       size_type;		//类型大小

	//value_type会进行型别萃取
	typedef value_type* iterator;	//迭代器是普通指针，因为vector是连续存储空间
	typedef const value_type* const_iterator;
	typedef mystl::reverse_iterator<iterator> reverse_iterator;		//反向指针
	typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;

	allocator_type get_allocator() { return data_allocator(); }	//获取数据构造对象

private:
	//以下三个指针共12bytes，便是一个初始化的vector对象的大小
	iterator begin_;	//表示目前使用空间的头部
	iterator end_;		// 表示目前使用空间的尾部
	iterator cap_;		// 表示目前可用空间的尾部

public:
	// 构造、复制、移动、析构函数
	vector()noexcept { try_init(); }//有条件的noexcept，表示如果try_init()不发生异常，构造函数vector()一定不发生异常
	explicit vector(size_type n){ fill_init(n, value_type()); }	//调用基本类型，如int，的构造函数，所以值为0
	vector(size_type n, const value_type& value){fill_init(n, value);}	//fill_init函数的声明与定义在下面

	//迭代器范围初始化
	template <class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
	vector(Iter first, Iter last){	
		//iterator是T*，但是传参可能是const T*，所以使用另一个类型Iter，并让它自行推断参数类型
		MYSTL_DEBUG(!(last < first));	//MYSTL_DEBUG函数定义在exceptdef.h中
		range_init(first, last);		//range_init函数声明与定义在下面
	}
	
	//拷贝构造函数
	vector(const vector& rhs) {
		range_init(rhs.begin_, rhs.end_);
	}

	//移动构造函数，使用初始值列表,移动构造函数因为肯定会改变传入对象，所以和拷贝构造函数不同，参数不能是const
	vector(vector&& rhs)noexcept:begin_(rhs.begin_),end_(rhs.end_),cap_(rhs.cap_) {
		//仅仅是指针的赋值，并没有申请内存，并且把右值的指针置空
		rhs.begin_ = nullptr;
		rhs.end_ = nullptr;
		rhs.cap_ = nullptr;
	}

	//初始化列表初始化
	vector(std::initializer_list<value_type> ilist)
	{
		range_init(ilist.begin(), ilist.end());	//begin()返回的是const T*类型
	}

	//操作符重载
	vector& operator=(const vector& rhs);
	vector& operator=(vector&& rhs)noexcept;

	vector& operator=(std::initializer_list<value_type>ilist) {
		vector tmp(ilist.begin(), ilist.end());
		swap(tmp);
		return	*this;
	}

	//析构函数
	~vector() {
		destroy_and_recover(begin_, end_, cap_ - begin_);	//destroy_and_recover函数的声明与定义在下面
		begin_ = end_ = cap_ = nullptr;				//析构后置为空指针
	}

public:
	//迭代器相关操作
	iterator begin() noexcept { return begin_; }
	const_iterator begin()const noexcept { return begin_; }
	iterator end() noexcept { return end_; }
	const_iterator end()const noexcept { return end_; }

	//反向迭代器相关操作
	reverse_iterator rbegin()noexcept { return reverse_iterator(end()); }	//rbegin()调用end()
	const_reverse_iterator rbegin()const noexcept { return reverse_iterator(end()); }
	reverse_iterator rend()noexcept { return reverse_iterator(begin()); }	//rend()调用begin()
	const_reverse_iterator rend()const noexcept { return reverse_iterator(begin()); }

	//const迭代器相关操作，封装上面定义的迭代器相关函数
	const_iterator cbegin()const noexcept { return begin(); }	
	const_iterator cend()const noexcept { return end(); }
	const_reverse_iterator crbegin()const noexcept { return rbegin(); }
	const_reverse_iterator crend()const noexcept { return rend(); }

	//容量相关操作
	bool empty()const noexcept { return begin_ == end_; }
	//static_cast类型转换，因为不知道end_和begin_传进来是什么类型
	size_type size()const noexcept { return static_cast<size_type>(end_ - begin_); } //目前使用空间大小
	size_type max_size()const noexcept { return static_cast<size_type>(-1) / sizeof(T); }
	size_type capacity()const noexcept { return static_cast<size_type>(cap_ - begin_); }	//可用容量大小
	void reserve(size_type n);	//重新分配空间
	void shrink_to_fit();		//放弃多余空间

	 // 访问元素相关操作
	reference operator[](size_type n) {		//下标运算符的重载
		MYSTL_DEBUG(n < size());
		return *(begin_ + n);
	}
	const_reference operator[](size_type n)const {
		MYSTL_DEBUG(n < size());
		return *(begin_ + n);
	}

	reference at(size_type n) {			//判断下标操作是否合法，合法则返回下标元素
		//下标超出范围就抛出异常
		THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
		return (*this)[n];
	}
	const_reference at(size_type n) const
	{
		THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
		return (*this)[n];
	}

	reference front() {			//获取首元素
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
	const_reference back() const	//获取尾元素
	{
		MYSTL_DEBUG(!empty());
		return *(end_ - 1);
	}
	pointer       data()	  noexcept { return begin_; }	//获取迭代器指针
	const_pointer data()const noexcept { return begin_; }

	// 修改容器相关操作
	// assign 赋值操作
	void assign(size_type n, const value_type& value) { fill_assign(n, value); }  // assign将vector赋值为n个value
	template <class Iter,typename std::enable_if<mystl::is_input_iterator<Iter>::value,int>::type=0>
	void assign(Iter first, Iter last) {	//enable_if 都用来判断模板参数的类型
		MYSTL_DEBUG(!(last < first));
		copy_assign(first, last, iterator_category(first));
	}
	void assign(std::initializer_list<value_type>il){
		copy_assign(il.begin(), il.end(), mystl::forward_iterator_tag{});	//初始化列表赋值
	}

	// emplace在容器指定位置插入元素 / emplace_back  在容器末尾新增元素
	template<class...Args>iterator emplace(const_iterator pos, Args&& ...args);
	template<class...Args>void emplace_back(Args&& ...args);

	// push_back 容器末尾新增元素 / pop_back 容器末尾删除元素
	void push_back(const value_type& value);
	void push_back(value_type&& value) { emplace_back(mystl::move(value)); }	//右值引用参数
	void pop_back();

	// insert 插入元素
	iterator insert(const_iterator pos, const value_type& value);
	iterator insert(const_iterator pos, value_type&& value) {		//右值引用参数
		return emplace(pos, mystl::move(value));
	}
	iterator insert(const_iterator pos, size_type n, const value_type& value) {
		MYSTL_DEBUG(pos >= begin() && pos <= end());
		//const_cast去除复合类型中const和volatile属性（没有真正去除）
		return fill_insert(const_cast<iterator>(pos), n.value);	
	}
	template <class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
		void insert(const_iterator pos, Iter first, Iter last){
		MYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
		copy_insert(const_cast<iterator>(pos), first, last);
	}

	// erase 清除某个位置上的元素  / clear 清除所有元素
	iterator erase(const_iterator pos);
	iterator erase(const_iterator first, const_iterator last);
	void clear() { erase(begin(), end()); }

	// resize 调整容器的大小，使其包含n个元素 / reverse 用于反转在[first,last)范围内的顺序
	void resize(size_type new_size) { return resize(new_size, value_type()); }
	void resize(size_type new_size, const value_type& value);
	void reverse() { mystl::reverse(begin(), end()); }

	// swap 交换元素
	void swap(vector& rhs)noexcept;

private:
	 // helper functions声明

	// initialize 初始化 / destroy 析构
	void try_init()noexcept;
	void init_space(size_type size, size_type cap);
	void fill_init(size_type n, const value_type& value);
	template <class Iter>void range_init(Iter first, Iter last);
	void destroy_and_recover(iterator first, iterator last, size_type n);

	// calculate the growth size计算增长规模
	size_type get_new_cap(size_type add_size);

	// assign
	void fill_assign(size_type n, const value_type& value);
	template <class IIter>void copy_assign(IIter first, IIter last, input_iterator_tag);
	template <class FIter>void copy_assign(FIter first, FIter last, forward_iterator_tag);

	// reallocate 重新分配内存空间
	template <class... Args>void reallocate_emplace(iterator pos, Args&& ...args);
	void reallocate_insert(iterator pos, const value_type& value);

	// insert 插入函数
	iterator fill_insert(iterator pos, size_type n, const value_type& value);
	template<class IIter>void copy_insert(iterator pos, IIter first, IIter last);

	// shrink_to_fit放弃多余空间
	void reinsert(size_type size);

};		//模板类结束符

/*****************************************************************************************/

// 复制赋值操作符
template<class T>vector<T>& vector<T>::operator=(const vector& rhs) {
	if (this != &rhs) {		//不处理自己给自己赋值
		const auto len = rhs.size();
		if (len > capaticy()) {		//当前vector容量不够，就需要扩容
			vector tmp(rhs.begin(), rhs.end());		
			swap(tmp);		//this和另一个vector互换
		}
		else if (size() >= len) {	//当前vector内容比rhs长，需要将当前vector多余的内容销毁
			auto i = mystl::copy(rhs.begin(), rhs.end(), begin());
			data_allocator::destroy(i, end_);	//销毁范围内的元素，但并不会释放空间
			end_ = begin_ + len;
		}
		else {
			mystl::copy(rhs.begin(), rhs.begin() + size(), begin_);
			mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);	//将多余部分置零
			cap_ = end_ = begin_ + len;		//令可用空间与使用空间相等
		}
	}
	return *this;
}

// 移动赋值操作符
template<class T> vector<T>& vector<T>::operator=(vector&& rhs)noexcept {
	destroy_and_recover(begin_, end_, cap_ - begin_);	//要先将之前begin_指向的内存空间释放掉
	begin_ = rhs.begin_;
	end_ = rhs.end_;
	cap_ = rhs.cap_;
	rhs.begin_ = rhs.end_ = rhs.cap_ = nullptr;
	return *this;
}

// 预留空间大小，当原容量小于要求大小时，才会重新分配
template<class T>void vector<T>::reserve(size_type n) {
	if (capacity() < n) {
		THROW_LENGTH_ERROR_IF(n > max_size(),"n can not larger than max_size() in vector<T>::reserve(n)");
		const auto old_size = size();
		auto tmp = data_allocator::allocate(n);				//构造新的内存空间
		mystl::uninitialized_move(begin_, end_, tmp);		//将原vector的内容移动到新vector
		data_allocator::deallocate(begin_, cap_ - begin_);	//释放原vector
		//调整迭代器，指向新vector
		begin_ = tmp;
		end_ = tmp + old_size;
		cap_ = begin_ + n;
	}
}

// 放弃多余的容量
template<class T>void vector<T>::shrink_to_fit() {
	if (end_ < cap_) {
		reinsert(size());
	}
}

// 在 pos 位置就地构造元素，避免额外的复制或移动开销
template <class T> template <class ...Args>	typename vector<T>::iterator 
vector<T>::emplace(const_iterator pos, Args&& ...args) {
	MYSTL_DEBUG(pos >= begin() && pos <= end());
	iterator xpos = const_cast<iterator>(pos);	//去const属性
	const size_type n = xpos - begin_;
	if (end_ != cap_ && xpos == end_) {			//在vecotr末尾插入，相当于emplace_back
		//address_of()就是取对象的地址，这里直接传end_也可以
		//construct函数是在指定位置，依据传入的参数构建对象，调用的方法是::new ((void*)pos) T(args...)
		data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
		++end_;	//调整水位
	}
	else if (end_ != cap) {	//还有未使用空间
		auto new_end = end_;
		//在备用空间起始处构造一个元素，并以vector最后一个元素为其初值
		data_allocator::construct(mystl::address_of(*end_), *(end_ - 1)); 
		++new_end;	//调整水位
		mystl::copy_backward(xpos, end_ - 1, end_);	//emplace同样有数据搬运的操作
		*xpos = value_type(mystl::forward<Args>(args)...);
	}
	else {	//如果空间不够则重新分配空间，并在 pos 处就地构造元素
		reallocate_emplace(xpos, mystl::forward<Args>(args)...);
	}
	return begin() + n;
}

// 在尾部就地构造元素，避免额外的复制或移动开销
template <class T> template <class ...Args> void vector<T>::emplace_back(Args&& ...args) {
	if (end_ < cap_) {
		data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
		++end_;
	}
	else {
		reallocate_emplace(end_, mystl::forward<Args>(args)...);
	}
}

// 在尾部插入元素
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

// 弹出尾部元素
template <class T>
void vector<T>::pop_back()
{
	MYSTL_DEBUG(!empty());
	data_allocator::destroy(end_ - 1);
	--end_;		//将尾端标记往前移一格，表示将放弃尾端元素
}

// emplace和insert相比，只是因为传参而不是传对象，所以少了构建一次临时对象的步骤
// 在 pos 处插入元素
template <class T> typename vector<T>::iterator vector<T>::insert(const_iterator pos, const value_type& value) {
	MYSTL_DEBUG(pos >= begin() && pos <= end());
	iterator xpos = const_cast<iterator>(pos);	//去const
	const size_type n = pos - begin_;
	if (end_ != cap_ && xpos == end_)	//在vecotr末尾插入
	{
		//address_of()就是取对象的地址，这里直接传end_也可以
		//construct函数是在指定位置，依据传入的参数构建对象，调用的方法是::new ((void*)pos) T(args...)
		data_allocator::construct(mystl::address_of(*end_), value);
		++end_;	//调整水位
	}
	else if (end_ != cap_)	//还有未使用空间
	{
		auto new_end = end_;
		//在备用空间起始处构造一个元素，并以vector最后一个元素为其初值
		data_allocator::construct(mystl::address_of(*end_), *(end_ - 1));
		++new_end;
		auto value_copy = value;  // 避免元素因以下复制操作而被改变
		mystl::copy_backward(xpos, end_ - 1, end_);		//数据搬运的操作
		*xpos = mystl::move(value_copy);
		end_ = new_end;
	}
	else  //如果空间不够则重新分配空间，并在 pos 处就地构造元素
	{
		reallocate_insert(xpos, value);
	}
	return begin_ + n;
}

// 删除 pos 位置上的元素
template <class T> typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
	MYSTL_DEBUG(pos >= begin() && pos < end());
	iterator xpos = begin_ + (pos - begin());
	mystl::move(xpos + 1, end_, xpos);		//将pos后面的元素前移
	data_allocator::destroy(end_ - 1);		//销毁要删除的元素
	--end_;									//调整迭代器指向实际位置
	return xpos;
}

// 删除[first, last)上的元素
template <class T> typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
	MYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
	const auto n = first - begin();
	iterator r = begin_ + (first - begin());	//获取first前一个位置迭代器
	//将last后面元素迁移，并析构[first, last)上的元素
	data_allocator::destroy(mystl::move(r + (last - first), end_, r), end_);	
	end_ = end_ - (last - first);	//调整尾部位置
	return begin_ + n;
}

// 重置容器大小
template <class T> void vector<T>::resize(size_type new_size, const value_type& value) {
	if (new_size < size()) {
		erase(begin() + new_size, end());	//截断多余的空间
	}
	else {
		insert(end(), new_size - size(), value);	//容器末尾添加元素
	}
}

// 与另一个 vector 交换
template <class T>void vector<T>::swap(vector<T>& rhs)noexcept {
	if (this != &rhs) {
		mystl::swap(begin_, rhs.begin_);
		mystl::swap(end_, rhs.end_);
		mystl::swap(cap_, rhs.cap_);
	}
}

/*****************************************************************************************/
// helper function定义

// try_init 函数，若分配失败则忽略，不抛出异常
template <class T>void vector<T>::try_init()noexcept {
	try {
		begin_ = data_allocator::allocate(16);	//默认分配16个元素空间，begin_指向已使用空间的开头
		end_ = begin_;							//end_指向已使用空间的尾部
		cap_ = begin_ + 16;	//cap_指向总分配空间的尾部，这里+16，而不是+16*sizeof(T)，因为指针移动是依据数据类型大小
	}
	catch (...) {
		begin_ = nullptr;
		end_ = nullptr;
		cap_ = nullptr;
	}
}

// init_space 函数
template <class T>
void vector<T>::init_space(size_type size, size_type cap)
{
	try
	{
		begin_ = data_allocator::allocate(cap);		//以传入的可用空间参数作为初始化大小
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

// fill_init 函数
template <class T> void vector<T>::fill_init(size_type n, const value_type& value){
	const size_type init_size = mystl::max(static_cast<size_type>(16), n);
	init_space(n, init_size);
	mystl::uninitialized_fill_n(begin_, n, value);	//从begin_位置填充n个value
}

// range_init 函数
//这里的两个class不能写到一个里面，T是类模板提供的，Iter是函数模板提供的
//在模板类中定义了一个模板函数，类外定义必须采用这样的形式
template <class T> template <class Iter> void vector<T>::range_init(Iter first, Iter last){
	const size_type init_size = mystl::max(static_cast<size_type>(last - first),static_cast<size_type>(16));
	init_space(static_cast<size_type>(last - first), init_size);
	mystl::uninitialized_copy(first, last, begin_); // 将first至last范围内的元素拷贝到begin_处
}

// destroy_and_recover 函数
template <class T> void vector<T>::destroy_and_recover(iterator first, iterator last, size_type n){
	data_allocator::destroy(first, last);		//销毁元素
	data_allocator::deallocate(first, n);		//释放空间
}

// get_new_cap 函数
template <class T> typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size){
	const auto old_size = capacity();
	THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size,"vector<T>'s size too big");
	//根据要增加的vector大小去调整vector的可用空间大小
	//旧长度的两倍，或旧长度 + 新增元素个数
	if (old_size > max_size() - old_size / 2)
	{
		return old_size + add_size > max_size() - 16? old_size + add_size : old_size + add_size + 16;
	}
	const size_type new_size = old_size == 0 ? mystl::max(add_size, static_cast<size_type>(16))
		: mystl::max(old_size + old_size / 2, old_size + add_size);
	return new_size;
}

// fill_assign 函数
template <class T> void vector<T>::fill_assign(size_type n, const value_type& value){
	if (n > capacity())		//可用空间不足，就重新构造vector并将元素交换过来
	{
		vector tmp(n, value);
		swap(tmp);
	}
	else if (n > size())	//大于当前使用空间，就在结尾新增元素
	{
		mystl::fill(begin(), end(), value);
		end_ = mystl::uninitialized_fill_n(end_, n - size(), value);
	}
	else
	{
		erase(mystl::fill_n(begin_, n, value), end_);	//删除末尾多出来的内容
	}
}

// copy_assign 函数
template <class T> template <class IIter> void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag){
	auto cur = begin_;
	for (; first != last && cur != end_; ++first, ++cur)	//遍历first到last的元素
	{
		*cur = *first;										//将[first，last)拷贝进去
	}
	if (first == last)
	{
		erase(cur, end_);									//删除多出来的元素，仅保留[first，last)之间的元素
	}
	else
	{
		insert(end_, first, last);							//在当前使用空间结尾新增[first，last)之间的元素
	}
}

// 用 [first, last) 为容器赋值
template <class T> template <class FIter> void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag){
	const size_type len = mystl::distance(first, last);	//获取新增元素长度
	if (len > capacity())		//新增元素大于可用空间，重新构造vector并交换元素
	{
		vector tmp(first, last);
		swap(tmp);
	}
	else if (size() >= len)		//容器现有元素长度大于新增元素长度
	{
		auto new_end = mystl::copy(first, last, begin_);	
		data_allocator::destroy(new_end, end_);		//销毁新尾端标记与旧尾端标记之间元素
		end_ = new_end;			//尾端标记前移
	}
	else
	{
		auto mid = first;		
		mystl::advance(mid, size());	//重新分配一段大小。以满足新增元素需求
		mystl::copy(first, mid, begin_);	//元素拷贝
		auto new_end = mystl::uninitialized_copy(mid, last, end_);	
		end_ = new_end;
	}
}

// 重新分配空间并在 pos 处就地构造元素
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

// 重新分配空间并在 pos 处插入元素
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

// fill_insert 函数
template <class T> typename vector<T>::iterator vector<T>::
fill_insert(iterator pos, size_type n, const value_type& value){
	if (n == 0)
		return pos;
	const size_type xpos = pos - begin_;
	const value_type value_copy = value;  // 避免被覆盖
	if (static_cast<size_type>(cap_ - end_) >= n)
	{ // 如果备用空间大于等于增加的空间
		const size_type after_elems = end_ - pos;	//计算插入点之后的现有元素个数
		auto old_end = end_;
		if (after_elems > n)	//插入点之后的现有元素个数大于新增元素个数
		{
			mystl::uninitialized_copy(end_ - n, end_, end_);	//将备用空间纳入容器的使用空间内
			end_ += n;	//将vector尾端标记后移
			mystl::move_backward(pos, old_end - n, old_end);	//向后移动插入点之后的元素，空出空间给元素插入
			mystl::uninitialized_fill_n(pos, n, value_copy);	//从插入点开始填入新值
		}
		else					//插入点之后的现有元素个数小于等于新增元素个数
		{
			end_ = mystl::uninitialized_fill_n(end_, n - after_elems, value_copy);	//在备用空间插入新增元素
			end_ = mystl::uninitialized_move(pos, old_end, end_);				//插入点之后的现有元素移动到备用空间
			mystl::uninitialized_fill_n(pos, after_elems, value_copy);			//插入点之后插入新增元素
		}
	}
	else { // 如果备用空间不足，小于新增元素个数
		//首先决定新长度：旧长度的两倍，或旧长度+新增元素个数
		const auto new_size = get_new_cap(n);
		//配置新的vector空间
		auto new_begin = data_allocator::allocate(new_size);
		auto new_end = new_begin;
		try
		{
			//将旧vector的插入点之前的元素移动到新空间
			new_end = mystl::uninitialized_move(begin_, pos, new_begin);
			//将新增元素(初值皆为n)填入新空间
			new_end = mystl::uninitialized_fill_n(new_end, n, value);
			//将旧vector的插入点之后的元素复制到新空间
			new_end = mystl::uninitialized_move(pos, end_, new_end);
		}
		catch (...)
		{
			destroy_and_recover(new_begin, new_end, new_size);
			throw;
		}
		//清除并释放旧的vector
		data_allocator::deallocate(begin_, cap_ - begin_);
		//调整迭代器位置
		begin_ = new_begin;
		end_ = new_end;
		cap_ = begin_ + new_size;
	}
	return begin_ + xpos;
}

// copy_insert 函数，拷贝[first,last）
template <class T> template <class IIter> void vector<T>::copy_insert(iterator pos, IIter first, IIter last){
	if (first == last)
		return;
	const auto n = mystl::distance(first, last);
	if ((cap_ - end_) >= n)
	{ // 如果备用空间大小足够
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
	else{ // 备用空间不足
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

// reinsert 函数
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
// 重载比较操作符

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

// 重载 mystl 的 swap
template <class T>void swap(vector<T>& lhs, vector<T>& rhs){
	lhs.swap(rhs);
}

}		//命名空间结束符

#endif // !MYSTL_VECTOR_H_

