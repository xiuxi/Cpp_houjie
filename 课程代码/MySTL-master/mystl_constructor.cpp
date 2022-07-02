#include <new.h>

template <class T1, class T2>
inline void construct(T1 *p, T2 value)
{
    return new(T1) point(value);
}

template <class T1>
inline void destroy(T1 *p)
{
    p->~T1();// 调用T1的析构函数
}

// 留个坑，trivial destroy的valuetype 还没实现
template <class T1>
inline void destroy(T1 forward, T1 end)
{
    // 判断是否为trivial destroy
    if(1) tr_destory(forward, end);
    else untr_destroy(forward, end);
}

template <class T1>
inline void untr_destroy(T1 forward, T1 end)
{
    // untrivial destroy需要调用析构函数
    for(; foward!=end; foward++)destroy(forward);
}

template <class T1>
inline void tr_destory(T1 forward, T1 end)
{
    // trivial destroy不需要调用析构
    return ;
}