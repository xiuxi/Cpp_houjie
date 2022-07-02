#ifndef FORWARDLIST_H
#define FORWARDLIST_H
#include <iostream>
#include <stdexcept>
#define _throw_invalid_pos() std::runtime_error("invalid position")

template <class _Ty>
class Forward_List
{
private:
    // Node define
    struct Node{
        _Ty val;
        Node*   Next;
    };

    // basic typedef 
    typedef _Ty         _ValueType;
    typedef _Ty*        _Valueptr;
    typedef _Ty&        _ValueRef;
    typedef const _Ty   const_value_type;
    typedef const _Ty*  const_value_ptr;
    typedef const _Ty&  const_value_ref;
    typedef Node*       _Nodeptr;
    typedef int         difference_type;
    typedef size_t      _Location;
    typedef size_t      _size;

    // Head define
    _Nodeptr _Head;
    _Nodeptr _Cur;
    _size _Size;

    // API to visit node
    struct _Acc
    {
        typedef _Nodeptr&   _refnode;
        typedef _ValueRef   _refval;

        static _refnode _next(_Nodeptr _n){return (_refnode)_n->Next;}
        static _refval  _val(_Nodeptr _n){return (_refval)_n->val;}
        // offer a normal version for begin
        static _Nodeptr _nornext(_Nodeptr _n){return _n->Next;}
    };

public:
    struct const_iterator{
        protected:
        _Nodeptr _ptr;
        public:
        const_iterator(){}
        const_iterator(_Nodeptr _P):_ptr(_P){}
        const_iterator(const const_iterator& _P):_ptr(_P._ptr){}

        bool const operator==(const const_iterator& _I){
            return _I._ptr == _ptr;
        }

        bool const operator!=(const const_iterator& _I){
            return !(*this==_I);
        }
        const_value_type const operator*() {
            return (const_value_ref)_Acc::_val(this->_ptr);
        }

        const_iterator operator++(){
            this->_ptr = _Acc::_next(this->_ptr);            
            return *this;
        }

        // hard to understand
        // "this" is a pointer point to self
        const_iterator operator++(int){
            const_iterator _tmp = *this;
            ++*this;
            return _tmp;
        }
    };

    struct iterator:public const_iterator{        
        protected:
        _Nodeptr _ptr;
        public:
        iterator(){}
        iterator(_Nodeptr _P):_ptr(_P){}
        iterator(const iterator& _P):_ptr(_P._ptr){}

        _ValueRef& operator*() {
            return _Acc::_val(_ptr);
        }
        bool const operator==(iterator _I){
            return (this->_ptr == _I._ptr);
        }
        bool const operator!=(iterator _I){
            return !(_I == *this);
        }
        iterator operator++(){
            this->_ptr = _Acc::_next(this->_ptr);
            return *this;
        }
        iterator operator++(int){
            iterator _tmp = *this;
            ++*this;
            return _tmp;
        }
    };

private:
    // create new node, ignore value
    _Nodeptr const buy_node ()
    {
        _Nodeptr p = (_Nodeptr)malloc(sizeof(Node));
        assert(p!=NULL);
        return p;
    }

    // delete a node
    void const del_node(_Nodeptr n){
        delete n;
    }

public:
    // ctr
    explicit Forward_List():_Head(buy_node())
    ,_Cur(_Head),_Size(0){}

    ~Forward_List(){
        _Nodeptr temp = _Head;
        _Nodeptr t;
        while(temp != _Cur){
            t = temp;
            temp = _Acc::_nornext(temp);
            del_node(t);
            --_Size;
        }
        del_node(temp);
        --_Size;
    }

    void push_back(const _ValueType& v){
        _Nodeptr p = buy_node();
        _Acc::_next(_Cur) = p;
        _Acc::_val(p) = v;
        _Cur = p;
        ++ _Size;
    }

    void const traverse(){
        _Nodeptr temp = _Acc::_nornext(_Head);
        while(temp!=_Acc::_next(_Cur)){
            std::cout<<_Acc::_val(temp)<<std::endl;
            temp = _Acc::_nornext(temp);
        }
    }

    void insert(_Location loc,const _ValueType& v){
        if(loc>_Size) _throw_invalid_pos(); 
        // buy new node
        _Nodeptr new_node = buy_node();
        _Acc::_val(new_node)= v;

        _Nodeptr temp = _Head;
        _Nodeptr t;
        // find position
        while(loc>0){
            temp = _Acc::_nornext(temp);
            --loc;
        }
        // head insert
        t = _Acc::_nornext(temp);
        _Acc::_next(temp) = new_node;
        _Acc::_next(new_node) = t;
        ++_Size;
    }

    void erase(_Location loc){
        if(loc>_Size) _throw_invalid_pos(); 
        _Nodeptr temp = _Acc::_nornext(_Head);
        _Nodeptr t;
        // find position
        while(loc>1){
            temp = _Acc::_nornext(temp);
            --loc;
        }
        // head delete
        t = _Acc::_nornext(_Acc::_nornext(temp));
        del_node(_Acc::_nornext(temp));
        _Acc::_next(temp) = t;
    }

    // iterator need definately define
    // need to define iterator plusplus

    iterator begin(){
        return iterator(_Acc::_nornext(_Head));
    }

    iterator end(){
        return iterator(_Acc::_nornext(_Cur));
    }

    // const version
    const_iterator const cbegin(){
        return const_iterator(_Acc::_nornext(_Head));
    }
    const_iterator const cend(){
        return const_iterator(_Acc::_nornext(_Cur));
    }

    _size const size(){
        return _Size;
    }

    bool const empty()
    {
        return _Size?true:false;
    }
    
};

#endif