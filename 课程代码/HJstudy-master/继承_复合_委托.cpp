template <class T>   //composition(复合)   has-a
class queue {
    ...
    protected:
        deque<T> c;
    public:
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        reference front() { return c.front(); }
        reference back() { return c.back(); }

        void push(const value_type& x) { c.push_back(x) };
        void pop() { c.pop_front(); }
};
///复合////构造由内而外   ///析构 由外而内 /////


//////委托Delegation   ///composition by reference////
//file String.hpp
class StringRep;
class String {
    public:
        String();
        String(const char* s);
        String(const String& s);
        String &operator=(const String& s);
        ~String();

    private:
        StringRep* rep;     /////委托StringRep去做一些工作
};
////file String.cpp
class StringRep{
    friend class String;
        StringRep(const char);
        ~StringRep();
        int count;
        char* rep;
};

/////继承/////////////   is-a   //////////////
struct _List_node_base
{
    _List_node_base* _M_next;
    _List_node_base* _M_prev;
};

template<typename _Tp>
struct _List_node
    :public _List_node_base
{
    _Tp _M_data;
};