/**
 * ģ��Ҫ�������α���
 * 1. class template, ��ģ�壬ʹ�õ�ʱ����Ҫָ��ģ�������
 * 2. function template, ����ģ�岻��Ҫָ�����ͣ����������Լ��Ƶ�
 * 3. member template����Աģ�壬ģ��Ƕ�׼�ppt
 *      ��������ָ�룬
 * 4. specialization��full specialization ȫ������, ģ���ػ���ģ����һ�ַ��������ػ�����ָ��ģ���һ�������������������
 * 5. partial specialization, ģ��ƫ�ػ�
 *    --������ƫ������������ȷ������һ��
 *    --��Χ��ƫ�����������ͣ�ȷ��ֻ��ָ��ָ��
 * 6. template template parameter��ģ��ģ�����,������ģ���д���һ�����������Լ����������
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

