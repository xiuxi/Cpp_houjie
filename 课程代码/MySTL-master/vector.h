#include "2jjallocator.h"
#include <iostream>

template <class T, class Alloc = >
class vector{
    typedef T value_type;
    typedef T* pointer;
    typedef T* iterator;
    typedef T& reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
protected:
// simple_alloc is space_allocator in SGI STL
typedef simple_alloc<value_type, Alloc> data_allocator;

iterator start;
iterator finish;
iterator end_of_storage;

void insert_aux(iterator position, const T& x);

void deallocate(){
    if(start)
    data_allocator::deallocate(start, end_of_storage - start);
}

void fill_initialize(size_type n, const T& value){
    data_allocator::allocate(n,value);
    finish = start+n;
    end_of_storage = finish;
}
};