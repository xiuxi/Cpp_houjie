/* 
    pointer-like classes 像指针的类
    1、关于智能指针shared_ptr
        注意有一点，箭头符号很特别，可以一直箭头指下去
        shared_ptr<Foo> sp(new Foo);
        Foo f(*sp);

        sp->method();
        ||
        px->method();
    2、关于迭代器 iteator 
        见pdf
    3、function-like classes 仿函数
        标准库中，仿函数所使用的奇特的base classes
 */