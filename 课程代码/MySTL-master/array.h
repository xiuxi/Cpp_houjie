#include <iostream>

template <typename _Tp, size_t _L>
class Array{

    typedef _Tp value_type;
    typedef _Tp* pointer_type;
    typedef _Tp& ref_type;
    public:
    typedef _Tp* iterator;

    private:
    value_type data[_L?_L:1];
    pointer_type _begin;
    pointer_type _end;

    public:
    Array():_begin(&data[0]),_end(&data[_L]){}
    iterator begin(){
        return _begin;
    }
    iterator end(){
        return _end;
    }

    ref_type operator[](const int _loc)
    {
       return (ref_type)data[_loc];
    }

};