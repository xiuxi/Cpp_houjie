//////Inheritance with virtual functions//////


#include <iostream>
using namespace std;

class Shape{
    public:
        virtual void draw() const=0;                 //纯虚函数
        virtual void error(const std::string& msg);  //
        int objectID() const;                        //非虚函数
};
/////非虚函数  ///不希望子类重新定义它
/////虚函数    ///希望子类重新定义它
/////纯虚函数  ///希望子类一定重新定义

class Rectangle:public Shape{};
class Ellipse:public Shape{};

//////虚函数继承。。。///子类对象可以调用父类函数   ////Template Method（模板函数？Method，Java术语：函数） 23种设计模式之一
//////


//继承+复合关系下的构造和析构
//一个类既有父类继承又有复合    待验证。
//一个类既有子类继承又有复合   构造：复合->父类->子类。析构相反。

//Delegation委托+继承
//被委托类被继承，被继承的父类的子类也可以被委托