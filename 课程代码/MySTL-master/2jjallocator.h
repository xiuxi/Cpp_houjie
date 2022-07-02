// 作者给出的简单版空间配置器,接口不整齐，不适用于SGI的vector等容器
#ifndef _JJALLOCATOR_
#define _JJALLOCATOR_


#include <new>// for placement new
/*
new 头文件是动态内存管理库的一部分
提供低层内存管理
*/
#include <cstdlib>// for exit()
/*
cstdlib 头文件提供杂项工具，定义多个标准库组件使用的宏
*/
#include <cstddef>// for size_t ptrdiff_t
/*
提供NULL/size_t/ptrdiff_t/等特殊类型的定义
NULL : 空指针常量
offsetof : 从标准布局类型的起始到其指定成员的字节偏移
size_t : 一般用来表示容器长度(sizeof的返回类型),与机器相关的unsigned整型类型，其大小足以保证存储内存中的对象大小
ptrdiff_t: 用来存放两个指针相减的结果，是有符号整型
*/
#include <iostream>// for cerr,处理错误流
#include <climits>// for UINT_MAX
/*
数值极限接口，提供各种数值类型的最大最小值的宏定义
*/
#endif

namespace JJ{
    template <class T>
    inline T* _allocate(ptrdiff_t size, T*){
        // 配置空间，足以存储size个T对象
        std::set_new_handler(0);
        /*
        原型: new_handler set_new_handler (new_handler new_p) throw();
        new_handler是一个无参数无返回值的函数，set_new_handler作用是将new_p函数置为new_handler函数。
        当new和delete失败时，new_handler会被调用
        new_handler会尝试为新的分配空间请求申请更多可用的空间，当且仅当申请成功时该函数会返回，否则，该函数会抛出异常或者结束程序。
        */
       T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));// 申请size_t个T元素的地址
       if(tmp == 0){
           // 如果申请失败输出错误流
           std::cerr<<"out of memory"<<std::endl;
           exit(1);
       }
       return tmp;
    }

    template <class T>
    inline void _deallocate(T* buffer){
        ::operator delete[](buffer);
    }

    template <class T1, class T2>
    inline void _construct(T1* p, const T2& value){
        new(p) T1(value);// 使用T1的ctor和新值value构造新对象
        // TIPS: ctor是constructor的缩写
    }

    template <class T>
    inline void _destroy(T* ptr){
        ptr->~T();// 调用T对象的析构
    }

    template <class T>
    class allocator{
        public:
        // 定义各种数据类型
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        template <class U>
        struct rebind{
            typedef allocator<U> other;
        };

        pointer allocate(size_type n, const void* hint = 0)
        {
            // 这边 hint 不懂的什么意思
            // hint 是指针
            // 该函数申请配置连续空间
            return _allocate((difference_type)n, (pointer)0);
        }
        void deallocate(pointer p, size_type n){
            // 没有函数调用参数n，为什么传了个n进来？
            _deallocate(p);
        }
        void construct(pointer p, const T& value){
            // 重构一个对象
            _construct(p, value);
        }
        void destroy(pointer p){
            // 析构一个对象
            _destroy(p);
        }
        pointer address(reference x){
            // 返回一个引用的地址
            return (pointer)&x;
        }
        const_pointer const_address(const_reference x){
            return (const_pointer)&x;
        }
        size_type max_size() const{
            return size_type(UINT_MAX/sizeof(T));
        }
    };
}// end of declaration of jj