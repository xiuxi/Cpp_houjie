// 不要使用这个文件
// 该头文件仅仅是在new和delete之上一层浅包装
// 并无法实现高效率
// 并且该allocator的实现不在任何SGI头文件中
#ifndef DEFALLOC_H
#define DEFALLOC_H
#include <new.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>

template <class T>
inline T* allocate(ptrdiff_t size, T*){
    // 请求更多可使用的空间
    _set_new_handler(0);
    // 尝试分配空间
    T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
    if(temp==0){
        std::cerr<< "out of memory"<< std::endl;
        exit(1);
    }
    return tmp;
}

template <class T>
inline void deallocate(T* buffer){
    ::operator delete(buffer);
}

template <class T>
class allocator{
public:
    typedef T          value_type;
    typedef T*         pointer;
    typedef const T*   const_pointer;
    typedef T&         reference;
    typedef const T&   const_reference;
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;

// 分配内存空间
pointer allocate(size_t len){
    return ::allocate(difference_type(len), (pointer)0);
}

// 回收内存空间
void deallocate(pointer p){::deallocate(p);}

// 返回指针引用地址
pointer address(pointer p){return pointer&(p);}

// 返回不可修改的引用地址
const_pointer const_address(pointer p){return const_pointer(p);}
size_type init_page_size(){
    return max(size_type(1), size_type(4096/sizeof(T)));
}
size_type max_size() const{
    return max(size_type(1), size_type(UINT_MAX/sizeof(T)));
}

};

// allocator特化版本
class allocator<void> {
    public:
    typedef void* pointer;
};
#endif