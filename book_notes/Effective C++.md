# Effective C++

## 条款02：编译器替换预处理器

>尽量以const，enum，inline替换 #define

## 条款03：尽可能使用const

* const与指针

    `const*` & `*const`
* const与迭代器

    ```c++
    //const_iterator -> const*
    const std::vector<int> vec = {1, 2, 3, 4};
    std::vector<int>::iterator it = const_cast<std::vector<int>*>(&vec)->begin();
    *it = 10;   // 这样可以修改元素 1 为 10
    ```

    ```c++
    //const iterator -> *const
    const std::vector<int>::iterator iter = vec.begin();
    *iter = 10;
    ++iter;     //wrong
    ```

* const与函数返回值

  ```c++
  const Rational operator* (const Rational &lhs, const Rational &rhs)

  //避免一些意想不到的错误，比如if( a * b = c ) 
  ```

* const与函数参数
* const与函数自身
  * `bitwise const` & `logica const`

## 条款04：确定对象被使用前已经被初始化

## 条款05：了解C++默认编写并调用哪些函数

* 关于空类会默认产生的函数：
  * 构造函数
  * 析构函数
  * 拷贝构造函数
  * 赋值运算符
  * 取址运算符
  * 取址运算符const

* 空类的大小会占用 `1byt`
* 如果空类作为基类，类的大小会被优化为0，“空白基类最优化”

## 条款06：不想使用编译器自动生成的函数，应该明确拒绝

## 条款07：为多态基类声明虚析构函数

## 条款08：别让异常逃离析构函数

## 条款09：不要再构造函数与析构函数中调用虚函数

* 不符合虚函数语义，子类还没有构造或者子类已经析构

## 条款10：`operator =` 返回 `reference to *this`

* 可以处理连锁赋值情况，类似的 += 、-= 都需要返回引用

## 条款11：在`operator =`中处理自我赋值

### Copy And Swap

首先，我们来了解一下**类值语义**和**类引用语义**。对于一个类，如果它的任意两个对象可以进行值比较，我们就称它拥有类值语义；如果任意两个对象之间有复杂的关系，例如指向同一块内存，我们则称它拥有类引用语义。

使用类值语义时，赋值运算符应该创建一个新的独立的对象，使得原对象和目标对象的内容不再关联。这通常需要深拷贝原对象的数据成员，然后把这些数据成员赋值给目标对象。

`Copy and swap` 技术的思想是先创建一个副本，然后交换这个副本与当前对象之间的所有数据成员，最后销毁这个副本。这样可以避免重复的代码，也可以保证在发生异常时能正常回滚操作。

```c++
class MyClass {
public:
  // ...
  MyClass& operator=(MyClass other) {
    // 使用副本构造函数创建一个副本
    swap(*this, other); // 交换副本和当前对象之间的数据成员
    return *this; // 返回当前对象
  }
private:
  // ...
  friend void swap(MyClass& first, MyClass& second) { // 交换函数必须是友元
    // 交换first和second之间的所有数据成员
  } 
};
```

**注意，这里的参数 other 是以值传递的方式传入的，这意味着它将会进行复制构造**

## 条款12：复制对象时勿忘其每一个成分

## 条款13：以对象管理资源

### RAII

## 条款14: 在资源管理类中，小心复制行为

对于资源管理类的`copying`行为，可能需要按照以下方面来决定：

* 禁止复制。有的`RAII`对象复制并不合理，如资源管理类中管理的锁
* 对底层资源祭出“引用计数法”。有时候，我们希望保持资源，直到它的最后一个使用者被销毁，

## 条款15:在资源管理类中提供对原始资源的访问
