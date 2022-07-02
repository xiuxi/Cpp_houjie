#include <cstddef>
#include <assert.h>
#ifndef LIST_H_
#define LIST_H_

namespace mylist{
    template <class _Ty>
    class List{
        private:

        struct Node{
            _Ty val;
            Node* _Next;
            Node* _Prev;
        };


        private:
        // 萃取
        typedef size_t     size_type;
        typedef Node*      _Nodeptr;
        typedef _Ty        value_type;
        typedef _Ty*       pointer_type;
        typedef const _Ty  const_value_type;
        typedef const _Ty* const_pointer_type;
        typedef const _Ty& const_reference_type;
        typedef int        difference_type;

        public:
        struct iterator{
            private:
            _Nodeptr _ptr;
            public:
            // 无参数构造函数
            iterator();
            // 带参数构造函数
            iterator(_Nodeptr P):_ptr(P){}
            iterator operator++(){
                this->_ptr = (_Nodeptr)_Acc::Next(this->_ptr);
                return *this;
            }
            iterator operator++(int){
                iterator tmp = *this;
                ++*this;
                return tmp;
            }
            iterator operator--(){
                this->_ptr = (_Nodeptr)_Acc::Prev(this->_ptr);
                return *this;
            }
            iterator operator--(int){
                iterator tmp = this->_ptr;
                --*this;
                return tmp;
            }
            const bool operator==(iterator ite){
                return ite._ptr==this->_ptr;
            }
            const bool operator!=(iterator ite){
                return !(ite==*this);
            }
            value_type& operator*(){
                return _Acc::Value(this->_ptr);
            }
        };

        struct const_iterator{
            private:
            _Nodeptr _ptr;
            public:
            const_iterator(){}
            const_iterator(_Nodeptr _N):_ptr(_N){}
            const_iterator operator++(){
                this->_ptr= _Acc::Next(this->_ptr);
                return *this;
            }
            const_iterator operator++(int){
                const_iterator _temp = *this;
                this->_ptr = _Acc::Next(this->_ptr);
                return _temp;
            }
            const_iterator operator--(){
                this->_ptr = _Acc::Prev(this->_ptr);
                return *this;
            }
            const_iterator operator--(int){
                const_iterator _temp = *this;
                this->_ptr = _Acc::Prev(this->_ptr);
                return _temp;
            }
            const bool operator==(const_iterator ite){
                return ite._ptr == this->_ptr;
            }
            const bool operator!=(const_iterator ite){
                return !(ite==*this);
            }
            const value_type& operator*(){
                return _Acc::Value(this->_ptr);
            }
        };


        struct _Acc{
            typedef _Nodeptr& _ref_Nodeptr;
            typedef _Ty& _ref_Val;

            // 返回后继节点
            static _ref_Nodeptr Next(_Nodeptr _S)
            {
                return ((_ref_Nodeptr)_S->_Next);
            }

            // 返回前向节点
            static _ref_Nodeptr Prev(_Nodeptr _S)
            {
                return ((_ref_Nodeptr)_S->_Prev);
            }

            // 返回值域
            static _ref_Val Value(_Nodeptr _S)
            {
                return (_ref_Val)(_S->val);
            }
        };

        private:
        _Nodeptr _Head;
        size_type _Size;

        protected:
        _Nodeptr _buynode(_Nodeptr _Narg=0, _Nodeptr _Parg=0){
            // malloc申请空间
            _Nodeptr _temp= (_Nodeptr)malloc(sizeof(_Nodeptr));
            assert(_temp!=NULL);
            
            _Acc::Next(_temp) = _Narg? _Narg:_temp;
            _Acc::Prev(_temp) = _Parg? _Parg:_temp;
            return _temp;
        }

        public:
        explicit List():_Head(_buynode()),_Size(0){}

        // 尾插
        void push_back(const value_type& val){
            _Nodeptr temp = _Acc::Prev(_Head);
            _Nodeptr _nnode = _buynode(_Head,temp);
            _Acc::Next(temp) = _nnode;
            _Acc::Prev(_Head) = _nnode;

            _Acc::Value(_nnode) = val;

            _Size++;
        }
        // 头插
        void push_front(const value_type& val){
            _Nodeptr temp = _Acc::Next(_Head); 
            _Nodeptr _nnode = _buynode(temp,_Head);
            _Acc::Prev(temp) = _nnode;
            _Acc::Next(_Head) = _nnode;

            _Acc::Value(_nnode) = val;

            _Size ++;
        }

        // 头删
        /*
        void pop_front(){
            _Nodeptr temp = _Head->_Next->_Next;
            _Nodeptr wait_del = _Head -> _Next;
            _Head->_Next = temp;
            temp->_Prev = _Head;
            delete wait_del;

            _Size --;
        }
        */

        // 尾删
        void pop_back(){}

        size_type size(){return _Size;}

        iterator begin(){
            return iterator(_Acc::Next(_Head));
        }

        iterator end(){
            return iterator(_Head);
        }

        const_iterator cbegin(){
            return const_iterator(_Acc::Next(_Head));
        }

        const_iterator cend(){
            return const_iterator(_Head);
        }


    };

};

#endif