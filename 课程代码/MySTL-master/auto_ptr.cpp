template <class T>
class auto_ptr{
    private:
    T* pointee;
    public:

    // 构造函数获取一个指针，将私有成员指针指向该地址
    explicit auto_ptr(T* p = 0):pointee(p){}

    // 猜测release是删除指针并返回地址
    template<class U>
    auto_ptr(auto_ptr<U>& rhs):pointee(rhs.release()){}

    // 析构函数
    ~auto_ptr{delete pointee;}

    template<class U>
    auto_ptr<T>& operator=(auto_ptr<U>& rhs){
        if(this!= &rhs)reset(rhs.release());
        return *this;
    }

    T& operator*() const {return *pointee;}
    T* operator->() const {return pointee;}
    T* get() const {return pointee;}
};