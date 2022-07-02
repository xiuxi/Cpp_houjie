/**
 * 模板要经过两次编译
 * 1. class template, 类模板，使用的时候需要指明模板的类型
 * 2. function template, 函数模板不需要指明类型，函数可以自己推导
 * 3. member template，成员模板，模板嵌套见ppt
 *      例：智能指针，
 * 4. specialization（full specialization 全泛化）, 模板特化（模板是一种泛化），特化就是指定模板的一个特例，可以由任意个
 * 5. partial specialization, 模板偏特化
 *    --个数的偏，两个参数，确定其中一个
 *    --范围的偏，从任意类型，确定只能指向指针
 * 6. template template parameter，模板模板参数,可以向模板中传入一个任意容器以及其参数类型
 * template<typename T, template <typename T> class Container>
 * class XCls
 * {
 *  private:
 *      Container<T> c;
 *  public:
 *      ......
 * }
 * 
 * template<typename T>
 * using Lst = list<T,allocator<T>>;
 * XCls<string,Lst> mtlist;
**/

