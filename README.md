
# 📑 目录

* [➕ C/C++](#cc)
* [💻 操作系统](#os)
* [☁️ 计算机网络](#computer-network)
* [💾 数据库](#database)
* [🔧 实用工具](#usefultools)
* [〽️ 数据结构](#data-structure)
* [❓ 算法](#algorithm)
* [📏 设计模式](#design-pattern)
* [⚙️ 链接装载库](#link-loading-library)
* [📚 书籍](#books)
* [🔱 C/C++ 项目](#cc-project)
* [💯 复习刷题网站](#review-of-brush-questions-website)
* [📝 面试题目经验](#interview-questions-experience)
* [📆 招聘时间岗位](#recruitment-time-post)
* [👍 内推](#recommend)

<a id="cc"></a>

## ➕ C/C++

### `const`

#### 作用

1. 修饰变量，说明该变量`不可改变`；
2. 修饰指针，分为指向常量的指针（pointer to const）和自身是常量的指针（常量指针，const pointer）；
3. 修饰引用，指向常量的引用（reference to const），用于形参类型，即避免了拷贝，又避免了函数对值的修改；
4. 修饰成员函数，说明该成员函数内不能修改成员变量。

#### const 的指针与引用

* 指针
  * 指向常量的指针（pointer to const） `const*`
  * 自身是常量的指针（常量指针，const pointer）`*const`
* 引用
  * 指向常量的引用（reference to const）
  * 没有 const reference，因为引用只是对象的别名，引用不是对象，不能用 const 修饰

```cpp
// 类
class A
{
private:
    const int a;                // 常对象成员，可以使用初始化列表或者类内初始化

public:
    // 构造函数
    A() : a(0) { };
    A(int x) : a(x) { };        // 初始化列表

    // const可用于对重载函数的区分
    int getValue();             // 普通成员函数
    int getValue() const;       // 常成员函数，不得修改类中的任何数据成员的值
};

void function()
{
    // 对象
    A b;                        // 普通对象，可以调用全部成员函数
    const A a;                  // 常对象，只能调用常成员函数
    const A *p = &a;            // 指针变量，指向常对象
    const A &q = a;             // 指向常对象的引用

    // 指针
    char greeting[] = "Hello";
    char* p1 = greeting;                // 指针变量，指向字符数组变量
    const char* p2 = greeting;          // 指针变量，指向字符数组常量（const 后面是 char，说明指向的字符（char）不可改变）
    char* const p3 = greeting;          // 自身是常量的指针，指向字符数组变量（const 后面是 p3，说明 p3 指针自身不可改变）
    const char* const p4 = greeting;    // 自身是常量的指针，指向字符数组常量
}

// 函数
void function1(const int Var);           // 传递过来的参数在函数内不可变
void function2(const char* Var);         // 参数指针所指内容为常量
void function3(char* const Var);         // 参数指针为常量
void function4(const int& Var);          // 引用参数在函数内为常量

// 函数返回值
const int function5();      // 返回一个常数
const int* function6();     // 返回一个指向常量的指针变量，使用：const int *p = function6();
int* const function7();     // 返回一个指向变量的常指针，使用：int* const p = function7();

```

#### #define和 const 的比较

|#define|const 常量|
:--:|:--:
宏定义，相当于字符替换|常量声明
预处理器处理|编译器处理
无类型安全检查|有类型安全检查
不分配内存|要分配内存
存储在代码段|存储在数据段
可通过 `#undef` 取消|不可取消

### 引用

#### 左值引用

常规引用，一般表示对象的身份。

#### 右值引用

右值引用就是必须绑定到右值（一个临时对象、将要销毁的对象）的引用。

右值引用可实现转移语义和完美转发，它的主要目的有两个方面：

* 消除两个对象交互时不必要的对象拷贝，节省运算存储资源，提高效率。
* 能够更简洁明确地定义泛型函数。
  
#### 移动语义

`std::move` 的底层实现非常简单，它就是将对象的地址返回，不会进行任何实际的复制或移动操作。由于返回的是一个右值引用，所以可以将其绑定到右值对象、移动构造函数或移动赋值函数中，实现对象的移动操作。

```c++
template <typename T>
typename remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(arg);
}
```

```c++
//remove_reference的STL实现
template <typename T>
struct remove_reference {
    using type = T;
};

template <typename T>
struct remove_reference<T&> {
    using type = T;
};

template <typename T>
struct remove_reference<T&&> {
    using type = T;
};
```

```c++
std::string s1 = "Hello";
//1. 创建一个指向字符串 “Hello” 的指针（C 风格字符串）。
//2. 使用字符串指针创建 std::string 对象 s1，将字符串数据复制到 s1 的内部存储区域中。
//3. 释放字符串指针。
std::string s2 = std::move(s1);
// 1. 调用 std::move(s1) 将 s1 转换为右值（std::string&& 类型）。
// 2. 调用 std::string 类的移动构造函数，创建一个新的 std::string 对象 s2，并将右值引用中的内部数据指针 move 到 s2 中。此时，s1 的内部数据指针已经成为了空指针。
// 3. s1 对象被销毁，其内部存储区域被释放。

//简单的移动构造函数示例
string::string(string&& other) noexcept
 : _M_data(other._M_data),
   _M_size(other._M_size)
{
    // 将右值引用对象的指针设为空
    other._M_data = nullptr;
    other._M_size = 0;
}

```

#### 完美转发

`forward`使用示例

```c++
#include <iostream>
#include <utility>

void foo(int& x) {
    std::cout << "lvalue: " << x << std::endl;
}

void foo(int&& x) {
    std::cout << "rvalue: " << x << std::endl;
}

template <typename T>
void bar(T&& x) {
    foo(std::forward<T>(x));
}

int main() {
    int a = 1;
    bar(a);        // lvalue: 1
    bar(2);        // rvalue: 2
    bar(std::move(a));  // rvalue: 1
    return 0;
}
```

`forward`实现原理

```c++
#include <iostream>
#include <type_traits>

template <typename T>
T&& my_forward(typename std::remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}

template <typename T>
T&& my_forward(typename std::remove_reference<T>::type&& arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "lvalue cannot be forwarded as rvalue");

    return static_cast<T&&>(arg);
}

void func(int&& value) {
    std::cout << "rvalue: " << value << std::endl;
}

void func(int& value) {
    std::cout << "lvalue: " << value << std::endl;
}

template <typename T>
void wrapper(T&& arg) {
    func(my_forward<T>(arg));
}

int main() {
    int value = 42;

    wrapper(value);    // lvalue: 42
    wrapper(std::move(value));    // rvalue: 42

    return 0;
}
```

#### 引用折叠

* `X& &`、`X& &&`、`X&& &` 可折叠成 `X&`
* `X&& &&` 可折叠成 `X&&`

### 函数指针

* 概念：每个函数都有一个入口地址，该入口地址就是函数指针的指向

* 应用场景：回调（别人的库里面调用我们的函数，就叫做CallBack）

```c++
void qsort(void *base,     //原始数组
           size_t nmemb,   //数据数量
           size_t size,    //单个数据空间
           int (*compar)(const void *, const void *)
           );

int cmp_int(const void *_a, const void * _b) {
    int *a = (int*)_a;
    int *b = (int*)_b;
    return *a - *b;
}
```

### `this` 指针

1. `this` 指针是一个隐含于每一个非静态成员函数中的特殊指针。它指向调用该成员函数的那个对象。
2. 当对一个对象调用成员函数时，编译程序先将对象的地址赋给 `this` 指针，然后调用成员函数，每次成员函数存取数据成员时，都隐式使用 `this` 指针。
3. 当一个成员函数被调用时，自动向它传递一个隐含的参数，该参数是一个指向这个成员函数所在的对象的指针。
4. `this` 指针被隐含地声明为: `ClassName *const this`，这意味着不能给 `this` 指针赋值；在 `ClassName` 类的 `const` 成员函数中，`this` 指针的类型为：`const ClassName* const`，这说明不能对 `this` 指针所指向的这种对象是不可修改的（即不能对这种对象的数据成员进行赋值操作）；
5. `this` 并不是一个常规变量，而是个右值，所不能取得 `this` 的地址（不能 `&this`）。
6. `this`在程序运行时动态进行绑定

使用实例

```c++
//实现链式引用
class MyClass {
public:
    MyClass& setX(int x) {
        this->x = x;  // 使用 this 指针实现链式调用
        return *this;
    }
    MyClass& setY(int y) {
        this->y = y;  // 使用 this 指针实现链式调用
        return *this;
    }
private:
    int x;
    int y;
};

int main() {
    MyClass obj;
    obj.setX(1).setY(2);  // 链式调用
    return 0;
}
```

#### nullptr可以调用成员函数吗

* 对象在创建时就已经绑定了其所属类的函数地址，即类的成员函数地址。这是通过类的静态信息（如 class 关键字定义的属性和方法）来实现的。当类的成员函数被定义时，编译器会根据函数定义的语法将其转化为相应的函数指针类型，并将该函数指针加入到类的信息中，同时，编译器也会为该类分配一块内存作为虚函数表（vtable），并将该表的指针存储到该类的各个对象中。在运行时，当对象调用其成员函数时，编译器会根据 vtable 中相应函数指针的地址，调用对应的函数实现。
* 需要注意的是，对于虚函数来说，它的地址并不是在编译期确定的，而是在运行时动态绑定的。虚函数的动态绑定是由虚函数表（vtable）实现的。虚函数表在对象的内存布局中存在一个指向虚函数表的指针，这个指针通常被称为虚指针（vptr），它指向了该对象的虚函数表，从而实现在运行时动态绑定虚函数的功能。当对象调用虚函数时，会先根据虚指针找到其所属类的虚函数表，并根据虚函数在表中的位置找到实际需要调用的函数实现。
* 因此，在编译阶段，对象已经绑定了其所属类的函数地址，但在运行时，虚函数的绑定是动态的，实现了多态的特性。
* 因此，调用成员函数和指针空不空没有关系

```c++
class animal{
public:
    void sleep(){ cout << "animal sleep" << endl; }
    void breathe(){ cout << "animal breathe haha" << endl; }
};
class fish :public animal{
public:
    void breathe(){ cout << "fish bubble" << endl; }
};
int main(){
    animal *pAn=nullptr;
    pAn->breathe();   // 输出：animal breathe haha
    fish *pFish = nullptr;
    pFish->breathe(); // 输出：fish bubble
}
```

### `static`

1. 修饰普通变量，修改变量的存储区域和生命周期，使变量存储在静态区，在 main 函数运行前就分配了空间，如果有初始值就用初始值初始化它，如果没有初始值系统用默认值初始化它。
2. 修饰普通函数，表明函数的作用范围，仅在定义该函数的文件内才能使用。在多人开发项目时，为了防止与他人命名空间里的函数重名，可以将函数定位为 static。
3. 修饰成员变量，修饰成员变量使所有的对象只保存一个该变量，而且不需要生成对象就可以访问该成员。
4. 修饰成员函数，修饰成员函数使得不需要生成对象就可以访问该函数，但是在 static 函数内不能访问非静态成员。

#### 在成员函数内部声明静态变量

在成员函数内部声明的静态变量是该函数的局部静态变量。这种局部静态变量只会在第一次调用该函数时初始化，并且在整个程序生命周期中仅存在一个实例。

```c++
class MyClass {
public:
    void myFunction() {
        static int myStaticVar = 0;
        cout << "myStaticVar = " << ++myStaticVar << endl;
    }
};

int main() {
    MyClass obj1, obj2;
    obj1.myFunction(); // 输出：myStaticVar = 1
    obj1.myFunction(); // 输出：myStaticVar = 2
    obj2.myFunction(); // 输出：myStaticVar = 3
    obj2.myFunction(); // 输出：myStaticVar = 4
}
```

需要注意的是，局部静态变量必须初始化为字面值常量或者使用常量表达式，例如：

```c++
void myFunction() {
    static int myStaticVar = 5; // 正确，初始化为字面值常量
    static int anotherStaticVar = myStaticVar + 2; // 错误，不能使用非常量表达式初始化
}
```

### `inline`

#### inline特征

* 相当于把内联函数里面的内容写在调用内联函数处；
* 相当于不用执行进入函数的步骤，直接执行函数体；
* 相当于宏，却比宏多了类型检查，真正具有函数特性；
* 编译器一般不内联包含循环、递归、switch 等复杂操作的内联函数；
* 在类声明中定义的函数，除了虚函数的其他函数都会自动隐式地当成内联函数。

#### inline使用

```cpp
// 声明1（加 inline，建议使用）
inline int functionName(int first, int second,...);

// 声明2（不加 inline）
int functionName(int first, int second,...);

// 定义
inline int functionName(int first, int second,...) {/****/};

// 类内定义，隐式内联
class A {
    int doA() { return 0; }         // 隐式内联
}

// 类外定义，需要显式内联
class A {
    int doA();
}
inline int A::doA() { return 0; }   // 需要显式内联
```

#### 编译器对 inline 函数的处理步骤

1. 将 inline 函数体复制到 inline 函数调用点处；
2. 为所用 inline 函数中的局部变量分配内存空间；
3. 将 inline 函数的的输入参数和返回值映射到调用方法的局部变量空间中；
4. 如果 inline 函数有多个返回点，将其转变为 inline 函数代码块末尾的分支（使用 GOTO）。

#### inline优缺点

优点

1. 内联函数同宏函数一样将在被调用处进行代码展开，省去了参数压栈、栈帧开辟与回收，结果返回等，从而提高程序运行速度。
2. 内联函数相比宏函数来说，在代码展开时，会做安全检查或自动类型转换（同普通函数），而宏定义则不会。
3. 在类中声明同时定义的成员函数，自动转化为内联函数，因此内联函数可以访问类的成员变量，宏定义则不能。
4. 内联函数在运行时可调试，而宏定义不可以。

缺点

1. 代码膨胀。内联是以代码膨胀（复制）为代价，消除函数调用带来的开销。如果执行函数体内代码的时间，相比于函数调用的开销较大，那么效率的收获会很少。另一方面，每一处内联函数的调用都要复制代码，将使程序的总代码量增大，消耗更多的内存空间。
2. inline 函数无法随着函数库升级而升级。inline函数的改变需要重新编译，不像 non-inline 可以直接链接。
3. 是否内联，程序员不可控。内联函数只是对编译器的建议，是否对函数内联，决定权在于编译器。

#### 虚函数（virtual）可以是内联函数（inline）吗？

* 虚函数可以是内联函数，内联是可以修饰虚函数的，但是当虚函数表现多态性的时候不能内联。
* 内联是在编译期建议编译器内联，而虚函数的多态性在运行期，编译器无法知道运行期调用哪个代码，因此虚函数表现为多态性时（运行期）不可以内联。
* `inline virtual` 唯一可以内联的时候是：编译器知道所调用的对象是哪个类（如 `Base::who()`），这只有在编译器具有实际对象而不是对象的指针或引用时才会发生。

虚函数内联使用

```cpp
#include <iostream>  
using namespace std;
class Base
{
public:
    inline virtual void who()
    {
        cout << "I am Base\n";
    }
    virtual ~Base() {}
};
class Derived : public Base
{
public:
    inline void who()  // 不写inline时隐式内联
    {
        cout << "I am Derived\n";
    }
};

int main()
{
    // 此处的虚函数 who()，是通过类（Base）的具体对象（b）来调用的，编译期间就能确定了，所以它可以是内联的，但最终是否内联取决于编译器。 
    Base b;
    b.who();

    // 此处的虚函数是通过指针调用的，呈现多态性，需要在运行时期间才能确定，所以不能为内联。  
    Base *ptr = new Derived();
    ptr->who();

    // 因为Base有虚析构函数（virtual ~Base() {}），所以 delete 时，会先调用派生类（Derived）析构函数，再调用基类（Base）析构函数，防止内存泄漏。
    delete ptr;
    ptr = nullptr;

    system("pause");
    return 0;
} 
```

### `mutable`

>用于修饰类中的成员变量，表示这个成员变量可以被修改，即使是在一个`const`成员函数中。

但是，**mutable关键字不能用于修饰static类型变量**，因为静态变量的值是存储在类的静态存储区中的，不是存储在对象中的。静态变量与类的任何对象都没有直接关系，所以它没有”被const限制的修改”这个问题。

### `volatile`

```cpp
volatile int i = 10; 
```

用来告诉编译器该变量的值可能会在程序的执行期间被意外改变，因此需要重新读取该变量的值，而不是使用缓存中的旧值。其主要作用是防止编译器的优化，保证程序的正确性。

具体来说，当一个对象被声明为 volatile 时，编译器在生成代码时会假定该对象的值可能会在程序运行期间发生变化，因此每次访问该对象时必须从内存中读取最新的值，而不是使用之前缓存的值。同样地，如果程序要写入一个 volatile 变量的值，编译器也不会使用缓存的旧值，而是直接写入内存，以保证该值的可靠性和正确性。

使用 volatile 关键字的情况包括：

* 并发编程：多线程或多进程访问同一个变量时，需要使用 volatile 保证变量值的正确性。
* 中断处理：中断处理程序中使用的变量需要使用 volatile，以确保它们的值能够及时更新。
* 内存映射的硬件寄存器：需要使用 volatile 关键字来防止编译器对寄存器的优化，确保每次读写都能直接与硬件进行交互。

值得注意的是，volatile 并不能保证并发程序的正确性，它仅仅提醒编译器该变量可能会在程序执行过程中被改变，在实际应用中仍需要使用互斥量、原子操作等并发编程技术来保证程序的正确性。

* volatile 关键字是一种类型修饰符，用它声明的类型变量表示可以被某些编译器未知的因素（操作系统、硬件、其它线程等）更改。所以使用 volatile 告诉编译器不应对这样的对象进行优化。
* volatile 关键字声明的变量，每次访问时都必须从内存中取出值（没有被 volatile 修饰的变量，可能由于编译器的优化，从 CPU 寄存器中取值）
* const 可以是 volatile （如只读的状态寄存器）
* 指针可以是 volatile

### `assert`

是宏，而非函数。可以通过定义 `NDEBUG` 来关闭 assert，但是需要在`include <assert.h>` 之前。

assert的内部实现

```cpp
#ifdef NDEBUG
    #define assert(condition) ((void)0)
#else
    #define assert(condition) ((condition) ? (void)0 : \
            __assert(__FILE__, __LINE__, #condition))
#endif

void __assert(const char *file, unsigned int line, const char *expr) {
    // 输出错误信息，并终止程序运行
    fprintf(stderr, "Assertion failed: %s, file %s, line %u\n", expr, file, line);
    abort();
}
```

> #是一个预处理器运算符，它会把宏参数转换成字符串字面量。在 # 后面的宏参数会被转换成一个字符串，在宏的展开中。

assert 使用

```cpp
#define NDEBUG          // 加上这行，则 assert 不可用
#include <assert.h>

assert( p != NULL );    // assert 不可用
```

### `constexpr`

* 对于一个变量或者是对象，我们可以将其定义为 constexpr 类型，这样就能够在编译期确定该变量或者是对象的值，这样可以提高代码的效率。

* 对于一个函数，我们可以将其声明为 constexpr 函数，这样就表示该函数可以在编译期执行，可以用于计算编译期间的常量表达式，从而提高程序的效率，同时也可以在编译期就检查是否满足某些条件。

需要注意的是，一个 constexpr 函数中只能有一条 return 语句，函数体内只能有一行语句，而且这个语句应该是 return 语句。另外，constexpr 函数中的所有形参和局部变量必须为 constexpr。但需要注意的是，如果将 constexpr 关键字应用在不合适的地方，可能会降低代码的性能。

```c++

#include <iostream>

constexpr int square(int x) { // 声明为 constexpr 函数
    return x * x;
}

int main() {
    constexpr int N = square(5); // 声明为 constexpr 变量
    std::cout << N << std::endl; // 输出 25
    return 0;
}

```

### `pragma pack(n)`

设定结构体、联合以及类成员变量以 n 字节方式对齐

 #pragma pack(n) 使用

```cpp
#pragma pack(push)  // 保存对齐状态
#pragma pack(4)     // 设定为 4 字节对齐

struct test
{
    char m1;
    double m4;
    int m3;
};

#pragma pack(pop)  `// 恢复对齐状态
```

### 位域

```cpp
//在 C 或 C++ 中，位域提供了一种优化内存使用的方式。它允许在结构体中仅用一定数量的比特位来存储字段，而不是使用整个字节或更多空间来存储一个小值。

//例如，假设我们需要一个数据结构来存储颜色的三个成分：红色、绿色和蓝色。如果我们使用结构体来表示它：

struct Color {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

//这将占用 3 个字节的内存，因为每个成分使用一个字节。但如果我们使用位域，可以只使用一个无符号整数类型：

struct Color {
    unsigned int red : 4;
    unsigned int green : 4;
    unsigned int blue : 4;
};

//在这种情况下，每个成分只占用了 4 位（半个字节），总共只需要 2 个字节，可以减少一半的内存开销。
```

类可以将其（非静态）数据成员定义为位域（bit-field），在一个位域中含有一定数量的二进制位。当一个程序需要向其他程序或硬件设备传递二进制数据时，通常会用到位域。

* 位域在内存中的布局是与机器有关的
* 位域的类型必须是整型或枚举类型，带符号类型中的位域的行为将因具体实现而定
* 取地址运算符（&）不能作用于位域，任何指针都无法指向类的位域

### `extern "C"`

* 被 `extern "C"` 修饰的变量和函数是按照 C 语言方式编译和链接的

* `extern "C"` 的作用是让 C++ 编译器将 `extern "C"` 声明的代码当作 C 语言代码处理，可以避免 C++ 因符号修饰导致代码不能和C语言库中的符号进行链接的问题。

extern "C" 使用

```cpp
#ifdef __cplusplus
extern "C" {
#endif

// C 函数定义
void c_function(int arg) {
    printf("%d\n"` arg);
}

#ifdef __cplusplus
}
#endif

// C++ 代码中调用 C 函数
int main() {
    c_function(42);
    return 0;
}
```

### 宏解释

`#`

```c++
//转化为字符串
#include <stdio.h>
#define LOGS(x)       printf("%s--%d\n",#x,x);
int main()
{
     int a=10;
     LOGS(a); //a--10
     return 0;
}
```

`##`

```c++
//连接字符串
#include <stdio.h>
#define LOG(x)    out_##x()
void out_1(void)
{
     printf("This is out_1 \n");
}
void out_2(void)
{
     printf("This is out_2 \n");
}
int main()
{
     LOG(1) ;   //This is out_1
     return 0;
}
```

`__VA_ARGS__`

```c++
//样可变参
#include <stdio.h>
#define LOGV(...)       printf(__VA_ARGS__);

int main()
{
      int a=10,b=11;
      LOGV("a=%d--b=%d\r\n",a,b);   //a=10--b=11
     return 0;
}
```

`##__VA_ARGS__`

### `function`

`std::function` 是 C++11 中的标准库函数对象包装器，它可以用来存储和调用任何可调用对象

```c++
#include <iostream>
#include <functional>

void print_message(const std::string& message) {
    std::cout << "Message: " << message << std::endl;
}

class MessageSender {
public:
    void send_message(const std::string& message) const {
        std::cout << "Sent: " << message << std::endl;
    }
};

int main() {
    // 1. 普通函数
    std::function<void(const std::string&)> func1 = print_message;
    func1("Hello, world!"); // 输出 "Message: Hello, world!"

    // 2. 成员函数
    MessageSender sender;
    std::function<void(MessageSender&, const std::string&)> func2 = &MessageSender::send_message;
    func2(sender, "Hello, world!"); // 输出 "Sent: Hello, world!"

    // 3. Lambda 表达式
    auto lambda = [](const std::string& message) -> void { 
        std::cout << "Lambda message: " << message << std::endl; 
    };
    std::function<void(const std::string&)> func3 = lambda;
    func3("Hello, world!"); // 输出 "Lambda message: Hello, world!"

    return 0;
}
```


### `union`

联合（union）是一种节省空间的特殊的类，一个 union 可以有多个数据成员，但是在任意时刻只有一个数据成员可以有值。当某个成员被赋值后其他成员变为未定义状态。联合有如下特点：

* 默认访问控制符为 public
* 可以含有构造函数、析构函数
* 不能含有引用类型的成员
* 不能继承自其他类，不能作为基类
* 不能含有虚函数
* 匿名 union 在定义所在作用域可直接访问 union 成员
* 匿名 union 不能包含 protected 成员或 private 成员
* 全局匿名联合必须是静态（static）的

union 使用

```cpp
#include<iostream>

union UnionTest {
    UnionTest() : i(10) {};
    int i;
    double d;
};

static union {
    int i;
    double d;
};

int main() {
    UnionTest u;

    union {
        int i;
        double d;
    };

    std::cout << u.i << std::endl;  // 输出 UnionTest 联合的 10

    ::i = 20;
    std::cout << ::i << std::endl;  // 输出全局静态匿名联合的 20

    i = 30;
    std::cout << i << std::endl;    // 输出局部匿名联合的 30

    return 0;
}
```

### `explicit`

* explicit 修饰构造函数时，可以防止隐式转换和复制初始化
* explicit 修饰转换函数时，可以防止隐式转换，但 [按语境转换](https://zh.cppreference.com/w/cpp/language/implicit_conversion) 除外

explicit 使用

```cpp
struct A
{
    A(int) { }
    operator bool() const { return true; }
};

struct B
{
    explicit B(int) {}
    explicit operator bool() const { return true; }
};

void doA(A a) {}

void doB(B b) {}

int main()
{
    A a1(1);        // OK：直接初始化
    A a2 = 1;       // OK：复制初始化
    A a3{ 1 };      // OK：直接列表初始化
    A a4 = { 1 };       // OK：复制列表初始化
    A a5 = (A)1;        // OK：允许 static_cast 的显式转换 
    doA(1);         // OK：允许从 int 到 A 的隐式转换
    if (a1);        // OK：使用转换函数 A::operator bool() 的从 A 到 bool 的隐式转换
    bool a6(a1);        // OK：使用转换函数 A::operator bool() 的从 A 到 bool 的隐式转换
    bool a7 = a1;       // OK：使用转换函数 A::operator bool() 的从 A 到 bool 的隐式转换
    bool a8 = static_cast<bool>(a1);  // OK ：static_cast 进行直接初始化

    B b1(1);        // OK：直接初始化
    B b2 = 1;       // 错误：被 explicit 修饰构造函数的对象不可以复制初始化
    B b3{ 1 };      // OK：直接列表初始化
    B b4 = { 1 };       // 错误：被 explicit 修饰构造函数的对象不可以复制列表初始化
    B b5 = (B)1;        // OK：允许 static_cast 的显式转换
    doB(1);         // 错误：被 explicit 修饰构造函数的对象不可以从 int 到 B 的隐式转换
    if (b1);        // OK：被 explicit 修饰转换函数 B::operator bool() 的对象可以从 B 到 bool 的按语境转换
    bool b6(b1);        // OK：被 explicit 修饰转换函数 B::operator bool() 的对象可以从 B 到 bool 的按语境转换
    bool b7 = b1;       // 错误：被 explicit 修饰转换函数 B::operator bool() 的对象不可以隐式转换
    bool b8 = static_cast<bool>(b1);  // OK：static_cast 进行直接初始化

    return 0;
}
```

### friend 友元类和友元函数

* 能访问私有成员  
* 破坏封装性
* 友元关系不可传递
* 友元关系的单向性
* 友元声明的形式及数量不受限制

### using

#### using 声明

一条 `using 声明` 语句一次只引入命名空间的一个成员。它使得我们可以清楚知道程序中所引用的到底是哪个名字。如：

```cpp
using namespace_name::name;
```

#### 构造函数的 using 声明

在 C++11 中，派生类能够重用其直接基类定义的构造函数。

```cpp
class Derived : Base {
public:
    using Base::Base;
    /* ... */
};
```

如上 using 声明，对于基类的每个构造函数，编译器都生成一个与之对应（形参列表完全相同）的派生类构造函数。生成如下类型构造函数：

```cpp
Derived(parms) : Base(args) { }
```

#### using 指示

`using 指示` 使得某个特定命名空间中所有名字都可见，这样我们就无需再为它们添加任何前缀限定符了。如：

```cpp
using namespace_name name;
```

#### 尽量少使用 `using 指示` 污染命名空间

> 一般说来，使用 using 命令比使用 using 编译命令更安全，这是由于它**只导入了指定的名称**。如果该名称与局部名称发生冲突，编译器将**发出指示**。using编译命令导入所有的名称，包括可能并不需要的名称。如果与局部名称发生冲突，则**局部名称将覆盖名称空间版本**，而编译器**并不会发出警告**。另外，名称空间的开放性意味着名称空间的名称可能分散在多个地方，这使得难以准确知道添加了哪些名称。

using 使用

尽量少使用 `using 指示`

```cpp
using namespace std;
```

应该多使用 `using 声明`

```cpp
int x;
std::cin >> x ;
std::cout << x << std::endl;
```

或者

```cpp
using std::cin;
using std::cout;
using std::endl;
int x;
cin >> x;
cout << x << endl;
```

### :: 范围解析运算符

#### 分类

1. 全局作用域符（`::name`）：用于类型名称（类、类成员、成员函数、变量等）前，表示作用域为全局命名空间
2. 类作用域符（`class::name`）：用于表示指定类型的作用域范围是具体某个类的
3. 命名空间作用域符（`namespace::name`）:用于表示指定类型的作用域范围是具体某个命名空间的

:: 使用

```cpp
int count = 11;         // 全局（::）的 count

class A {
public:
    static int count;   // 类 A 的 count（A::count）
};
int A::count = 21;

void fun()
{
    int count = 31;     // 初始化局部的 count 为 31
    count = 32;         // 设置局部的 count 的值为 32
}

int main() {
    ::count = 12;       // 测试 1：设置全局的 count 的值为 12

    A::count = 22;      // 测试 2：设置类 A 的 count 为 22

    fun();              // 测试 3

    return 0;
}
```

### enum

#### 限定作用域的枚举类型

```cpp
enum class open_modes { input, output, append };

enum class Size : long long { S = 100, M = 200, L = 300 };
enum class Status : char { OK = '0', Failed = '1' };
enum class State { On = 1, Off = 0, Unknown = -1 };

Size s = Size::M;
Status status = Status::OK;
State state = State::On;
```

#### 不限定作用域的枚举类型

```cpp
enum color { red, yellow, green };
enum { floatPrec = 6, doublePrec = 10 };
```

### decltype

decltype 关键字用于检查实体的声明类型或表达式的类型及值分类。语法：

```cpp
decltype ( expression )
```

decltype 使用

```cpp
// 尾置返回允许我们在参数列表之后声明返回类型
template <typename It>
auto fcn(It beg, It end) -> decltype(*beg)
{
    // 处理序列
    return *beg;    // 返回序列中一个元素的引用
}
// 为了使用模板参数成员，必须用 typename
template <typename It>
auto fcn2(It beg, It end) -> typename remove_reference<decltype(*beg)>::type
{
    // 处理序列
    return *beg;    // 返回序列中一个元素的拷贝
}
```

### 成员初始化列表

好处

* 更高效：少了一次调用默认构造函数的过程。
* 有些场合必须要用初始化列表：
  1. 常量成员，因为常量只能初始化不能赋值，所以必须放在初始化列表里面
  2. 引用类型，引用必须在定义的时候初始化，并且不能重新赋值，所以也要写在初始化列表里面
  3. 没有默认构造函数的类类型，因为使用初始化列表可以不必调用默认构造函数来初始化

### initializer_list 列表初始化

用花括号初始化器列表初始化一个对象，其中对应构造函数接受一个 `std::initializer_list` 参数.

initializer_list 使用

```cpp
#include <iostream>
#include <vector>
#include <initializer_list>
 
template <class T>
struct S {
    std::vector<T> v;
    S(std::initializer_list<T> l) : v(l) {
         std::cout << "constructed with a " << l.size() << "-element list\n";
    }
    void append(std::initializer_list<T> l) {
        v.insert(v.end(), l.begin(), l.end());
    }
    std::pair<const T*, std::size_t> c_arr() const {
        return {&v[0], v.size()};  // 在 return 语句中复制列表初始化
                                   // 这不使用 std::initializer_list
    }
};
 
template <typename T>
void templated_fn(T) {}
 
int main()
{
    S<int> s = {1, 2, 3, 4, 5}; // 复制初始化
    s.append({6, 7, 8});      // 函数调用中的列表初始化
 
    std::cout << "The vector size is now " << s.c_arr().second << " ints:\n";
 
    for (auto n : s.v)
        std::cout << n << ' ';
    std::cout << '\n';
 
    std::cout << "Range-for over brace-init-list: \n";
 
    for (int x : {-1, -2, -3}) // auto 的规则令此带范围 for 工作
        std::cout << x << ' ';
    std::cout << '\n';
 
    auto al = {10, 11, 12};   // auto 的特殊规则
 
    std::cout << "The list bound to auto has size() = " << al.size() << '\n';
 
//    templated_fn({1, 2, 3}); // 编译错误！“ {1, 2, 3} ”不是表达式，
                             // 它无类型，故 T 无法推导
    templated_fn<std::initializer_list<int>>({1, 2, 3}); // OK
    templated_fn<std::vector<int>>({1, 2, 3});           // 也 OK
}
```

### struct 和 class

1. struct 更适合看成是一个数据结构的实现体，class 更适合看成是一个对象的实现体。
2. struct的默认访问控制权限是public，而class的默认访问控制权限是private的
3. 在继承关系中，struct默认共有继承，class默认私有继承
4. class关键字可以用来定义模版参数

#### c++和c中结构体的比较

| |c|c++|
--|--|--
|成员函数|不能|可以
|静态成员|不能|可以
|访问控制|默认public，不能修改|public / private / protected
|继承关系|不可以继承|可以从类或者其他结构体继承
|初始化|不能直接初始化数据成员|可以

### C++从代码到可执行二进制文件的过程

>分为四个步骤：预编译、编译、汇编、链接 (装载、运行)

1. 预编译
   1. 将所有#define删除，展开所有的宏定义
   2. 处理所有的条件预编译指令，#if，#ifnef
   3. 处理#include预编译指令，
   4. 过滤所有注释
   5. 添加行号和文件名标识
2. 编译
   1. 词法分析
   2. 语法分析
   3. 语义分析
   4. 代码优化
   5. 生成汇编代码
3. 汇编
   1. 将汇编代码转变为可执行的机器指令
4. 链接：将不同的源文件产生的目标文件进行链接，形成可执行程序

#### 静态链接与动态链接

* 静态链接，是在链接的时候就已经把要调用的函数或者过程链接到了生成的可执行文件中，就算你在去把静态库删除也不会影响可执行程序的执行；生成的静态链接库，Linux下以.a为后缀。

* 动态链接，是在链接的时候没有把调用的函数代码链接进去，而是在执行的过程中，再去找要链接的函数，生成的可执行文件中没有函数代码，只包含函数的重定位信息，所以当你删除动态库时，可执行程序就不能运行。生成的动态链接库，Linux下以.so为后缀。

### 程序分区

>堆区、栈区、bss、text、data

```c++
char str1[] = "abcd";
char str2[] = "abcd";

const char str3[] = "abcd";
const char str4[] = "abcd";

const char *str5 = "abcd";
const char *str6 = "abcd";

char *str7 = "abcd";
char *str8 = "abcd";


cout << ( str1 == str2 ) << endl;
cout << ( str3 == str4 ) << endl;
//存储在栈区
cout << ( str5 == str6 ) << endl;
cout << ( str7 == str8 ) << endl;
//存储在静态区
```

![存储区](/images/%E5%AD%98%E5%82%A8%E5%8C%BA.png)

### 面向对象

面向对象程序设计（Object-oriented programming，OOP）是种具有对象概念的程序编程典范，同时也是一种程序开发的抽象方针。

![面向对象特征](https://gitee.com/huihut/interview/raw/master/images/面向对象基本特征.png)

面向对象三大特征 —— 封装、继承、多态

#### 封装

把客观事物封装成抽象的类，并且类可以把自己的数据和方法只让可信的类或者对象操作，对不可信的进行信息隐藏。关键字：public, protected, private。不写默认为 private。

* `public` 成员：可以被任意实体访问
* `protected` 成员：只允许被子类及本类的成员函数访问
* `private` 成员：只允许被本类的成员函数、友元类或友元函数访问

#### 继承

* 基类（父类）——&gt; 派生类（子类）

#### 多态

* 多态，即多种状态（形态）。简单来说，我们可以将多态定义为消息以多种形式显示的能力。
* 多态是以封装和继承为基础的。
* C++ 多态分类及实现：
    1. 重载多态（Ad-hoc Polymorphism，编译期）：函数重载、运算符重载
    2. 子类型多态（Subtype Polymorphism，运行期）：虚函数
    3. 参数多态性（Parametric Polymorphism，编译期）：类模板、函数模板
    4. 强制多态（Coercion Polymorphism，编译期/运行期）：基本类型转换、自定义类型转换

> [The Four Polymorphisms in C++](https://catonmat.net/cpp-polymorphism)

#### C 实现 C++ 类

C 实现 C++ 的面向对象特性（封装、继承、多态）

* 封装：使用函数指针把属性与方法封装到结构体中
* 继承：结构体嵌套
* 多态：父类与子类方法的函数指针不同

> [C语言实现面向对象](https://stackoverflow.com/a/351745)

#### 静态多态（编译期/早绑定）

函数重载

```cpp
class A
{
public:
    void do(int a);
    void do(int a, int b);
};
```

操作符重载

* 类型强转运算符 `operator target_type() const;`

```c++
#include <iostream>

class MyInt {
public:
    // 构造函数
    MyInt(int value) : m_value(value) {}

    // 类型转换运算符
    operator int() const {
        return m_value;
    }

private:
    int m_value;
};

int main() {
    MyInt myInt(123);
    int x = myInt;  // 相当于 int x = myInt.operator int();
    std::cout << "x = " << x << std::endl;

    return 0;
}
```

#### 动态多态（运行期期/晚绑定）

* 虚函数：用 virtual 修饰成员函数，使其成为虚函数
* 动态绑定：当使用基类的引用或指针调用一个虚函数时将发生动态绑定

**注意：**

* 可以将派生类的对象赋值给基类的指针或引用，反之不可
* 普通函数（非类成员函数）不能是虚函数
* 静态函数（static）不能是虚函数
* 构造函数不能是虚函数（因为在调用构造函数时，虚表指针并没有在对象的内存空间中，必须要构造函数调用完成后才会形成虚表指针）
* 内联函数不能是表现多态性时的虚函数，解释见：[虚函数（virtual）可以是内联函数（inline）吗？](https://github.com/huihut/interview#%E8%99%9A%E5%87%BD%E6%95%B0virtual%E5%8F%AF%E4%BB%A5%E6%98%AF%E5%86%85%E8%81%94%E5%87%BD%E6%95%B0inline%E5%90%97)

#### 虚析构函数

虚析构函数是一个在基类中声明为虚函数的析构函数。它的作用是当使用基类的指针删除派生类的对象时，能够正确地调用派生类的析构函数，防止内存泄漏或者资源没有被正确释放的问题。

通常情况下，如果没有定义虚析构函数，在使用基类指针删除派生类对象时，只会调用基类的析构函数，而不会调用派生类的析构函数，导致派生类的资源没有被释放。这就是虚析构函数存在的意义。

虚析构函数使用

```cpp
class Shape
{
public:
    Shape();                    // 构造函数不能是虚函数
    virtual double calcArea();
    virtual ~Shape();           // 虚析构函数
};
class Circle : public Shape     // 圆形类
{
public:
    virtual double calcArea();
    ...
};
int main()
{
    Shape * shape1 = new Circle(4.0);
    shape1->calcArea();    
    delete shape1;  // 因为Shape有虚析构函数，所以delete释放内存时，先调用子类析构函数，再调用基类析构函数，防止内存泄漏。
    shape1 = NULL;
    return 0；
}
```

#### 纯虚函数

纯虚函数是一种特殊的虚函数，在基类中不能对虚函数给出有意义的实现，而把它声明为纯虚函数，它的实现留给该基类的派生类去做。

```cpp
virtual int A() = 0;
```

#### 虚函数、纯虚函数

* 类里如果声明了虚函数，这个函数是实现的，哪怕是空实现，它的作用就是为了能让这个函数在它的子类里面可以被覆盖（override），这样的话，编译器就可以使用后期绑定来达到多态了。纯虚函数只是一个接口，是个函数的声明而已，它要留到子类里去实现。
* 虚函数在子类里面可以不重写；但纯虚函数必须在子类实现才可以实例化子类。
* 虚函数的类用于 “实作继承”，继承接口的同时也继承了父类的实现。纯虚函数关注的是接口的统一性，实现由子类完成。
* 带纯虚函数的类叫抽象类，这种类不能直接生成对象，而只有被继承，并重写其虚函数后，才能使用。抽象类被继承后，子类可以继续是抽象类，也可以是普通类。
* 虚基类是虚继承中的基类，具体见下文虚继承。

> [CSDN . C++ 中的虚函数、纯虚函数区别和联系](https://blog.csdn.net/u012260238/article/details/53610462)

#### 虚函数指针、虚函数表

* 虚函数指针：在含有虚函数类的对象中，指向虚函数表，在运行时确定。
* 虚函数表：在程序只读数据段（`.rodata section`，见：[目标文件存储结构](#%E7%9B%AE%E6%A0%87%E6%96%87%E4%BB%B6%E5%AD%98%E5%82%A8%E7%BB%93%E6%9E%84)），存放虚函数指针，如果派生类实现了基类的某个虚函数，则在虚表中覆盖原本基类的那个虚函数指针，在编译时根据类的声明创建。

> [C++中的虚函数(表)实现机制以及用C语言对其进行的模拟实现](https://blog.twofei.com/496/)

#### 虚继承

虚继承用于解决多继承条件下的菱形继承问题（浪费存储空间、存在二义性）。

底层实现原理与编译器相关，一般通过**虚基类指针**和**虚基类表**实现，每个虚继承的子类都有一个虚基类指针（占用一个指针的存储空间，4字节）和虚基类表（不占用类对象的存储空间）（需要强调的是，虚基类依旧会在子类里面存在拷贝，只是仅仅最多存在一份而已，并不是不在子类里面了）；当虚继承的子类被当做父类继承时，虚基类指针也会被继承。

实际上，vbptr 指的是虚基类表指针（virtual base table pointer），该指针指向了一个虚基类表（virtual table），虚表中记录了虚基类与本类的偏移地址；通过偏移地址，这样就找到了虚基类成员，而虚继承也不用像普通多继承那样维持着公共基类（虚基类）的两份同样的拷贝，节省了存储空间。

#### 虚继承、虚函数

* 相同之处：都利用了虚指针（均占用类的存储空间）和虚表（均不占用类的存储空间）
* 不同之处：
  * 虚继承
    * 虚基类依旧存在继承类中，只占用存储空间
    * 虚基类表存储的是虚基类相对直接继承类的偏移
  * 虚函数
    * 虚函数不占用存储空间
    * 虚函数表存储的是虚函数地址

#### 模板类、成员模板、虚函数

* 模板类中可以使用虚函数
* 一个类（无论是普通类还是类模板）的成员模板（本身是模板的成员函数）不能是虚函数

#### 抽象类、接口类、聚合类

* 抽象类：含有纯虚函数的类
* 接口类：仅含有纯虚函数的抽象类
* 聚合类：用户可以直接访问其成员，并且具有特殊的初始化语法形式。满足如下特点：
  * 所有成员都是 public
  * 没有定义任何构造函数
  * 没有类内初始化
  * 没有基类，也没有 virtual 函数

### 内存分配和管理

#### malloc、calloc、realloc、alloca

* malloc：申请指定字节数的内存。申请到的内存中的初始值不确定。
* calloc：为指定长度的对象，分配能容纳其指定个数的内存。申请到的内存的每一位都初始化为 0。

    ```c++
    void* calloc(size_t nmemb, size_t size); //函数原型
    ```

* realloc：更改以前分配的内存长度（增加或减少）。当增加长度时，可能需将以前分配区的内容移到另一个足够大的区域，而新增区域内的初始值则不确定。

    ```c++
    void* realloc(void* ptr, size_t size);
    ```

* alloca：在栈上申请内存。程序在出栈的时候，会自动释放内存。但是需要注意的是，alloca 不具可移植性, 而且在没有传统堆栈的机器上很难实现。alloca 不宜使用在必须广泛移植的程序中。C99 中支持变长数组 (VLA)，可以用来替代 alloca。

#### malloc、free

用于分配、释放内存

malloc、free 使用

申请内存，确认是否申请成功

```cpp
char *str = (char*) malloc(100);
assert(str != nullptr);
```

释放内存后指针置空

```cpp
free(p); 
p = nullptr;
```

#### new、delete

1. new / new[]：完成两件事，先底层调用 malloc 分配了内存，然后调用构造函数（创建对象）。
2. delete/delete[]：也完成两件事，先调用析构函数（清理资源），然后底层调用 free 释放空间。
3. new 在申请内存时会自动计算所需字节数，而 malloc 则需我们自己输入申请内存空间的字节数。

new、delete 使用

申请内存，确认是否申请成功

```cpp
int main()
{
    T* t = new T();     // 先内存分配 ，再构造函数
    delete t;           // 先析构函数，再内存释放
    return 0;
}
```

#### placement new

`placement new`允许我们向 new 传递额外的地址参数，从而在预先指定的内存区域创建对象。

```cpp
new (place_address) type
new (place_address) type (initializers)
new (place_address) type [size]
new (place_address) type [size] { braced initializer list }
```

* `place_address` 是个指针
* `initializers` 提供一个（可能为空的）以逗号分隔的初始值列表

#### new & mallo的区别

1. new是操作符，而malloc是函数。
2. new在调用的时候先分配内存，在调用构造函数，释放的时候调用析构函数；而malloc没有构造函数和析构函数。
3. malloc需要给定申请内存的大小，返回的指针需要强转；new会调用构造函数，不用指定内存的大小，返回指针不用强转。
4. new可以被重载；malloc不行
5. new分配内存更直接和安全。
6. new发生错误抛出异常，malloc返回null

#### malloc底层实现

#### new底层实现

#### 内存泄漏

> 简单来说就是申请了一块内存空间，使用完毕后没有释放掉

* new和malloc申请资源后没有使用delete和free
* 子类继承父类时，父类析构函数不是虚函数

### delete this 合法吗？

> [Is it legal (and moral) for a member function to say delete this?](https://isocpp.org/wiki/faq/freestore-mgmt#delete-this)

合法，但：

1. 必须保证 this 对象是通过 `new`（不是 `new[]`、不是 placement new、不是栈上、不是全局、不是其他对象成员）分配的
2. 必须保证调用 `delete this` 的成员函数是最后一个调用 this 的成员函数
3. 必须保证成员函数的 `delete this` 后面没有调用 this 了
4. 必须保证 `delete this` 后没有人使用了

### 如何定义一个只能在堆上（栈上）生成对象的类？

> [如何定义一个只能在堆上（栈上）生成对象的类?](https://www.nowcoder.com/questionTerminal/0a584aa13f804f3ea72b442a065a7618)

#### 只能在堆上

方法：将析构函数设置为私有

原因：C++ 是静态绑定语言，编译器管理栈上对象的生命周期，编译器在为类对象分配栈空间时，会先检查类的析构函数的访问性。若析构函数不可访问，则不能在栈上创建对象。

#### 只能在栈上

方法：将 new 和 delete 重载为私有

原因：在堆上生成对象，使用 new 关键词操作，其过程分为两阶段：第一阶段，使用 new 在堆上寻找可用内存，分配给对象；第二阶段，调用构造函数生成对象。将 new 操作设置为私有，那么第一阶段就无法完成，就不能够在堆上生成对象。

### 智能指针

#### C++ 标准库（STL）中

头文件：`#include <memory>`

#### C++ 98

```cpp
std::auto_ptr<std::string> ps (new std::string(str))；
```

#### C++ 11

* shared_ptr
  
    shared_ptr 实现共享式拥有概念。多个智能指针指向相同对象，该对象和其相关资源会在 “最后一个 reference 被销毁” 时被释放。为了在结构较复杂的情景中执行上述工作，标准库提供 weak_ptr、bad_weak_ptr 和 enable_shared_from_this 等辅助类。
    >[shared_ptr实现](/STL/code/shared_ptr.cpp)
* unique_ptr
  
    unique_ptr 实现独占式拥有或严格拥有概念，保证同一时间内只有一个智能指针可以指向该对象。你可以移交拥有权。它对于避免内存泄漏 ( 如 new 后忘记 delete ) 特别有用。
    >[unique_ptr实现](/STL/code/unqiue_ptr.cpp)
* weak_ptr

    weak_ptr 允许你共享但不拥有某对象，一旦最末一个拥有该对象的智能指针失去了所有权，任何 weak_ptr 都会自动成空（empty）。因此，在 default 和 copy 构造函数之外，weak_ptr 只提供 “接受一个 shared_ptr” 的构造函数。

  * 可打破环状引用（cycles of references，两个其实已经没有被使用的对象彼此互指，使之看似还在 “被使用” 的状态）的问题

### 强制类型转换运算符

> [MSDN . 强制转换运算符](https://msdn.microsoft.com/zh-CN/library/5f6c9f8h.aspx)

#### static_cast

* 用于非多态类型的转换
* 不执行运行时类型检查（转换安全性不如 dynamic_cast）
* 通常用于转换数值数据类型（如 float -> int）
* 可以在整个类层次结构中移动指针，子类转化为父类安全（向上转换），父类转化为子类不安全（因为子类可能有不在父类的字段或方法）

```c++
// 将 double 转换为 int
double d = 3.14;
int i = static_cast<int>(d);

// 将 char* 转换为 void*
char str[] = "Hello world!";
void* p = static_cast<void*>(&str[0]);

// 将 int* 转换为 char*
int arr[] = { 1, 2, 3 };
char* pch = static_cast<char*>(static_cast<void*>(&arr[0]));
```

> 向上转换是一种隐式转换。

#### dynamic_cast

* 用于多态类型的转换
* 执行行运行时类型检查
* 只适用于指针或引用
* 对不明确的指针的转换将失败（返回 nullptr），但不引发异常
* 可以在整个类层次结构中移动指针，包括向上转换、向下转换

```c++
// 将基类指针转换为派生类指针
class Animal {
public:
    virtual void speak() {}
};
class Dog : public Animal {
public:
    void speak() { cout << "Woof!" << endl; }
};
class Cat : public Animal {
public:
    void speak() { cout << "Meow!" << endl; }
};

Animal* ptr = new Dog;
Dog* dog = dynamic_cast<Dog*>(ptr); // 将 Animal* 转换为 Dog*

// 将基类引用转换为派生类引用
Animal& obj = *ptr;
Dog& d = dynamic_cast<Dog&>(obj);

// 将基类指针转换为无关类指针
class Shape {
public:
    virtual void draw() {}
};
class Circle : public Shape {
public:
    void draw() { cout << "Draw a circle" << endl; }
};

Shape* sptr = new Circle;
int* iptr = dynamic_cast<int*>(sptr); // 返回空指针
```

#### const_cast

* 用于删除 const、volatile 和 __unaligned 特性（如将 const int 类型转换为 int 类型 ）

```c++
// 将 const int* 转换为 int*
const int val = 123;
const int* p = &val;
int* ptr = const_cast<int*>(p);

// 将 const char* 转换为 char*
const char str[] = "Hello world!";
const char* pstr = str;
char* ch = const_cast<char*>(pstr);
*ch = 'h'; // 修改字符串的第一个字符
cout << str << endl; // 输出 "hello world!"
```

#### reinterpret_cast

* 用于位的简单重新解释
* 滥用 reinterpret_cast 运算符可能很容易带来风险。 除非所需转换本身是低级别的，否则应使用其他强制转换运算符之一。
* 允许将任何指针转换为任何其他指针类型（如 `char*` 到 `int*` 或 `One_class*` 到 `Unrelated_class*` 之类的转换，但其本身并不安全）
* 也允许将任何整数类型转换为任何指针类型以及反向转换。
* reinterpret_cast 运算符不能丢掉 const、volatile 或 __unaligned 特性。
* reinterpret_cast 的一个实际用途是在哈希函数中，即，通过让两个不同的值几乎不以相同的索引结尾的方式将值映射到索引。

#### bad_cast

* 由于强制转换为引用类型失败，dynamic_cast 运算符引发 bad_cast 异常。

```cpp
try {  
    Circle& ref_circle = dynamic_cast<Circle&>(ref_shape);   
}  
catch (bad_cast b) {  
    cout << "Caught: " << b.what();  
} 
```

### 运行时类型信息 (RTTI)

#### typeid

* typeid 运算符允许在运行时确定对象的类型
* typeid 返回一个 typeinfo 对象的引用
* 如果想通过基类的指针获得派生类的数据类型，基类必须带有虚函数
* 只能获取对象的实际类型

#### type_info

* type_info 类描述编译器在程序中生成的类型信息。 此类的对象可以有效存储指向类型的名称的指针。 type_info 类还可存储适合比较两个类型是否相等或比较其排列顺序的编码值。 类型的编码规则和排列顺序是未指定的，并且可能因程序而异。
* 头文件：`typeinfo`

typeid、type_info 使用

```cpp
#include <iostream>
using namespace std;

class Flyable                       // 能飞的
{
public:
    virtual void takeoff() = 0;     // 起飞
    virtual void land() = 0;        // 降落
};
class Bird : public Flyable         // 鸟
{
public:
    void foraging() {...}           // 觅食
    virtual void takeoff() {...}
    virtual void land() {...}
    virtual ~Bird(){}
};
class Plane : public Flyable        // 飞机
{
public:
    void carry() {...}              // 运输
    virtual void takeoff() {...}
    virtual void land() {...}
};

class type_info
{
public:
    const char* name() const;
    bool operator == (const type_info & rhs) const;
    bool operator != (const type_info & rhs) const;
    int before(const type_info & rhs) const;
    virtual ~type_info();
private:
    ...
};

void doSomething(Flyable *obj)                 // 做些事情
{
    obj->takeoff();

    cout << typeid(*obj).name() << endl;        // 输出传入对象类型（"class Bird" or "class Plane"）

    if(typeid(*obj) == typeid(Bird))            // 判断对象类型
    {
        Bird *bird = dynamic_cast<Bird *>(obj); // 对象转化
        bird->foraging();
    }

    obj->land();
}

int main(){
    Bird *b = new Bird();
    doSomething(b);
    delete b;
    b = nullptr;
    return 0;
}
```

### 模版元编程

>模板元编程的基本思想是利用模板进行静态计算和泛型编程：在编译器读取和翻译源代码时，所有涉及模板的计算都会被展开，并生成对应的字节码，而这些计算的结果可以直接嵌入最终的可执行程序中。

```c++
template<int N>
struct Fibonacci {
    static const int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template<>
struct Fibonacci<0> {
    static const int value = 0;
};

template<>
struct Fibonacci<1> {
    static const int value = 1;
};

int main() {
    std::cout << Fibonacci<10>::value << std::endl; // 输出 55
    return 0;
}
```

```c++
template<typename T>
class Demo {
public:
    void print(T value) {
        std::cout << value << std::endl;
    }
};

// 仅对浮点数类型进行处理
template<typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type
function(T x) {
    Demo<T> d;
    d.print(x * 10); // 输出浮点数乘以 10 的值
}

// 仅对整数类型进行处理
template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type
function(T x) {
    Demo<T> d;
    d.print(x * 2); // 输出整数乘以 2 的值
}

int main() {
    function(1.23); // 输出 12.3（浮点数乘以 10 的值）
    function(4); // 输出 8（整数乘以 2 的值）
    return 0;
}
```

```c++
//enable_if源码
template<bool B, typename T = void> 
struct enable_if {};

template<typename T>
struct enable_if<true, T> { using type = T; };

template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;
```

#### SFINAE技术

SFINAE 是指 `Substitutio Failure Is Not An Error`，即 “ 替换失败不是一个错误 ” 的缩写。它是 C++ 模板编程中一个非常强大的特性，针对模板参数匹配过程中因类型不匹配、函数不存在等导致的错误，使用 SFINAE 技术可以让编译器跳过这些错误，并继续寻找符合条件的模板。

### Google C++ Style Guide

* 英文：[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
* 中文：[C++ 风格指南](https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/contents/)

<a id="os"></a>

## 💻 操作系统

### 进程与线程

对于有线程系统：

* 进程是资源分配的独立单位
* 线程是资源调度的独立单位

对于无线程系统：

* 进程是资源调度、分配的独立单位

#### 进程调度算法

#### 进程之间的通信方式以及优缺点

* 管道（PIPE）
  * 有名管道：一种半双工的通信方式，它允许无亲缘关系进程间的通信
    * 优点：可以实现任意关系的进程间的通信
    * 缺点：
        1. 长期存于系统中，使用不当容易出错
        2. 缓冲区有限
  * 无名管道：一种半双工的通信方式，只能在具有亲缘关系的进程间使用（父子进程）
    * 优点：简单方便
    * 缺点：
        1. 局限于单向通信
        2. 只能创建在它的进程以及其有亲缘关系的进程之间
        3. 缓冲区有限

* 信号量（Semaphore）：一个计数器，可以用来控制多个线程对共享资源的访问
  * 优点：可以同步进程
  * 缺点：信号量有限
  
* 信号（Signal）：一种比较复杂的通信方式，用于通知接收进程某个事件已经发生
  
* 消息队列（Message Queue）：是消息的链表，存放在内核中并由消息队列标识符标识
  * 优点：可以实现任意进程间的通信，并通过系统调用函数来实现消息发送和接收之间的同步，无需考虑同步问题，方便
  * 缺点：信息的复制需要额外消耗 CPU 的时间，不适宜于信息量大或操作频繁的场合
  
* 共享内存（Shared Memory）：映射一段能被其他进程所访问的内存，这段共享内存由一个进程创建，但多个进程都可以访问
  * 优点：无须复制，快捷，信息量大
  * 缺点：
      1. 通信是通过将共享空间缓冲区直接附加到进程的虚拟地址空间中来实现的，因此进程间的读写操作的同步问题
      2. 利用内存缓冲区直接交换信息，内存的实体存在于计算机中，只能同一个计算机系统中的诸多进程共享，不方便网络通信

* 套接字（Socket）：可用于不同计算机间的进程通信
  * 优点：
      1. 传输数据为字节级，传输数据可自定义，数据量小效率高
      2. 传输数据时间短，性能高
      3. 适合于客户端和服务器端之间信息实时交互
      4. 可以加密,数据安全性强
  * 缺点：需对传输的数据进行解析，转化成应用级的数据。

#### 线程之间的通信方式

* 锁机制：
  * 互斥锁/量（mutex）：提供了以排他方式防止数据结构被并发修改的方法。
  * 读写锁（reader-writer lock）：允许多个线程同时读共享数据，而对写操作是互斥的。
  * 自旋锁（spin lock）与互斥锁类似，都是为了保护共享资源。互斥锁是当资源被占用，申请者进入睡眠状态；而自旋锁则循环检测保持者是否已经释放锁。
  * 条件变量（condition）：可以以原子的方式阻塞进程，直到某个特定条件为真为止。对条件的测试是在互斥锁的保护下进行的。条件变量始终与互斥锁一起使用。
  * 信号量机制(Semaphore)
    * 无名线程信号量
    * 命名线程信号量
* 信号机制(Signal)：类似进程间的信号处理
* 屏障（barrier）：屏障允许每个线程等待，直到所有的合作线程都达到某一点，然后从该点继续执行。

线程间的通信目的主要是用于线程同步，所以线程没有像进程通信中的用于数据交换的通信机制  

> 进程之间的通信方式以及优缺点来源于：[进程线程面试题总结](http://blog.csdn.net/wujiafei_njgcxy/article/details/77098977)

#### 进程之间私有和共享的资源

* 私有：地址空间、堆、全局变量、栈、寄存器
* 共享：代码段，公共数据，进程目录，进程 ID

#### 线程之间私有和共享的资源

* 私有：线程栈，寄存器，程序计数器
* 共享：堆，地址空间，全局变量，静态变量

#### 多进程与多线程间的对比、优劣与选择

##### 对比

对比维度 | 多进程 | 多线程
---|---|---
数据共享、同步|数据共享复杂，需要用 IPC；数据是分开的，同步简单|因为共享进程数据，数据共享简单，但也是因为这个原因导致同步复杂|
内存、CPU|占用内存多，切换复杂，CPU 利用率低|占用内存少，切换简单，CPU 利用率高|
创建销毁、切换|创建销毁、切换复杂，速度慢|创建销毁、切换简单，速度很快|
编程、调试|编程简单，调试简单|编程复杂，调试复杂|
可靠性|进程间不会互相影响|一个线程挂掉将导致整个进程挂掉|
分布式|适应于多核、多机分布式；如果一台机器不够，扩展到多台机器比较简单|适应于多核分布式|

##### 优劣

优劣|多进程|多线程
---|---|---
优点|编程、调试简单，可靠性较高|创建、销毁、切换速度快，内存、资源占用小
缺点|创建、销毁、切换速度慢，内存、资源占用大|编程、调试复杂，可靠性较差

##### 选择

* 需要频繁创建销毁的优先用线程
* 需要进行大量计算的优先使用线程
* 强相关的处理用线程，弱相关的处理用进程
* 可能要扩展到多机分布的用进程，多核分布的用线程
* 都满足需求的情况下，用你最熟悉、最拿手的方式

> 多进程与多线程间的对比、优劣与选择来自：[多线程还是多进程的选择及区别](https://blog.csdn.net/lishenglong666/article/details/8557215)

### Linux 内核的同步方式

#### 原因

在现代操作系统里，同一时间可能有多个内核执行流在执行，因此内核其实像多进程多线程编程一样也需要一些同步机制来同步各执行单元对共享数据的访问。尤其是在多处理器系统上，更需要一些同步机制来同步不同处理器上的执行单元对共享的数据的访问。

#### 同步方式

* 原子操作
* 信号量（semaphore）
* 读写信号量（rw_semaphore）
* 自旋锁（spinlock）
* 大内核锁（BKL，Big Kernel Lock）
* 读写锁（rwlock）
* 大读者锁（brlock-Big Reader Lock）
* 读-拷贝修改(RCU，Read-Copy Update)
* 顺序锁（seqlock）

> 来自：[Linux 内核的同步机制，第 1 部分](https://www.ibm.com/developerworks/cn/linux/l-synch/part1/)、[Linux 内核的同步机制，第 2 部分](https://www.ibm.com/developerworks/cn/linux/l-synch/part2/)

### 死锁

#### 死锁原因

* 系统资源不足
* 资源分配不当
* 进程运行推进顺序不合适

#### 产生条件

* 互斥
* 请求和保持
* 不剥夺
* 环路

#### 预防

* 打破互斥条件：改造独占性资源为虚拟资源，大部分资源已无法改造。
* 打破不可抢占条件：当一进程占有一独占性资源后又申请一独占性资源而无法满足，则退出原占有的资源。
* 打破占有且申请条件：采用资源预先分配策略，即进程运行前申请全部资源，满足则运行，不然就等待，这样就不会占有且申请。
* 打破循环等待条件：实现资源有序分配策略，对所有设备实现分类编号，所有进程只能采用按序号递增的形式申请资源。
* 有序资源分配法
* 银行家算法

### 文件系统

* Windows：FCB 表 + FAT + 位图
* Unix：inode + 混合索引 + 成组链接

### 主机字节序与网络字节序

#### 主机字节序（CPU 字节序）

可以这样判断自己 CPU 字节序是大端还是小端：

```cpp
#include <iostream>
using namespace std;

int main()
{
    int i = 0x12345678;

    if (*((char*)&i) == 0x12)
        cout << "大端" << endl;
    else
        cout << "小端" << endl;

    return 0;
}
```

#### 各架构处理器的字节序

* x86（Intel、AMD）、MOS Technology 6502、Z80、VAX、PDP-11 等处理器为小端序；
* Motorola 6800、Motorola 68000、PowerPC 970、System/370、SPARC（除 V9 外）等处理器为大端序；
* ARM（默认小端序）、PowerPC（除 PowerPC 970 外）、DEC Alpha、SPARC V9、MIPS、PA-RISC 及 IA64 的字节序是可配置的。

#### 网络字节序

网络字节顺序是 TCP/IP 中规定好的一种数据表示格式，它与具体的 CPU 类型、操作系统等无关，从而可以保证数据在不同主机之间传输时能够被正确解释。

网络字节顺序采用：大端（Big Endian）排列方式。

### 内存管理

### 页面置换算法

在地址映射过程中，若在页面中发现所要访问的页面不在内存中，则产生缺页中断。当发生缺页中断时，如果操作系统内存中没有空闲页面，则操作系统必须在内存选择一个页面将其移出内存，以便为即将调入的页面让出空间。而用来选择淘汰哪一页的规则叫做页面置换算法。

#### 置换分类

* 全局置换：在整个内存空间置换
* 局部置换：在本进程中进行置换

#### 算法

全局：

* 工作集算法
* 缺页率置换算法

局部：

* 最佳置换算法（OPT）
* 先进先出置换算法（FIFO）
* [最近最久未使用（LRU）算法](/os/LRU.cpp)
* [最近最少使用（LFU）算法](/os/LFU.cpp)
* 时钟（Clock）置换算法

<a id="computer-network"></a>

## ☁️ 计算机网络

> 本节部分知识点来自《计算机网络（第 7 版）》

计算机网络体系结构：

![计算机网络体系结构](https://gitee.com/huihut/interview/raw/master/images/计算机网络体系结构.png)

### 各层作用及协议

分层 | 作用 | 协议
---|---|---
物理层 | 通过媒介传输比特，确定机械及电气规范（比特 Bit） | RJ45、CLOCK、IEEE802.3（中继器，集线器）
数据链路层|将比特组装成帧和点到点的传递（帧 Frame）| PPP、FR、HDLC、VLAN、MAC（网桥，交换机）
网络层|负责数据包从源到宿的传递和网际互连（包 Packet）|IP、ICMP、ARP、RARP、OSPF、IPX、RIP、IGRP（路由器）
运输层|提供端到端的可靠报文传递和错误恢复（ 段Segment）|TCP、UDP、SPX
会话层|建立、管理和终止会话（会话协议数据单元 SPDU）|NFS、SQL、NETBIOS、RPC
表示层|对数据进行翻译、加密和压缩（表示协议数据单元 PPDU）|JPEG、MPEG、ASII
应用层|允许访问OSI环境的手段（应用协议数据单元 APDU）|FTP、DNS、Telnet、SMTP、HTTP、WWW、NFS

### 物理层

* 传输数据的单位：比特
* 数据传输系统：源系统（源点、发送器） --> 传输系统 --> 目的系统（接收器、终点）

通道：

* 单向通道（单工通道）：只有一个方向通信，没有反方向交互，如广播
* 双向交替通信（半双工通信）：通信双方都可发消息，但不能同时发送或接收
* 双向同时通信（全双工通信）：通信双方可以同时发送和接收信息

通道复用技术：

* 频分复用（FDM，Frequency Division Multiplexing）：不同用户在不同频带，所用用户在同样时间占用不同带宽资源
* 时分复用（TDM，Time Division Multiplexing）：不同用户在同一时间段的不同时间片，所有用户在不同时间占用同样的频带宽度
* 波分复用（WDM，Wavelength Division Multiplexing）：光的频分复用
* 码分复用（CDM，Code Division Multiplexing）：不同用户使用不同的码，可以在同样时间使用同样频带通信

### 数据链路层

主要信道：

* 点对点信道
* 广播信道

#### 点对点信道

* 数据单元：帧

三个基本问题：

* 封装成帧：把网络层的 IP 数据报封装成帧，`SOH - 数据部分 - EOT`
* 透明传输：不管数据部分什么字符，都能传输出去；可以通过字节填充方法解决（冲突字符前加转义字符）
* 差错检测：降低误码率（BER，Bit Error Rate），广泛使用循环冗余检测（CRC，Cyclic Redundancy Check）

点对点协议（Point-to-Point Protocol）：

* 点对点协议（Point-to-Point Protocol）：用户计算机和 ISP 通信时所使用的协议

#### 广播信道

广播通信：

* 硬件地址（物理地址、MAC 地址）
* 单播（unicast）帧（一对一）：收到的帧的 MAC 地址与本站的硬件地址相同
* 广播（broadcast）帧（一对全体）：发送给本局域网上所有站点的帧
* 多播（multicast）帧（一对多）：发送给本局域网上一部分站点的帧

### 网络层

* IP（Internet Protocol，网际协议）是为计算机网络相互连接进行通信而设计的协议。
* ARP（Address Resolution Protocol，地址解析协议）
* ICMP（Internet Control Message Protocol，网际控制报文协议）
* IGMP（Internet Group Management Protocol，网际组管理协议）

#### IP 网际协议

IP 地址分类：

* `IP 地址 ::= {<网络号>,<主机号>}`

IP 地址类别 | 网络号 | 网络范围 | 主机号 | IP 地址范围
---|---|---|---|---
A 类 | 8bit，第一位固定为 0 | 0 —— 127 | 24bit | 1.0.0.0 —— 127.255.255.255
B 类 | 16bit，前两位固定为  10 | 128.0 —— 191.255 | 16bit | 128.0.0.0 —— 191.255.255.255
C  类 | 24bit，前三位固定为  110 | 192.0.0 —— 223.255.255 | 8bit | 192.0.0.0 —— 223.255.255.255
D  类 | 前四位固定为 1110，后面为多播地址
E  类 | 前五位固定为 11110，后面保留为今后所用

IP 数据报格式：

![IP 数据报格式](https://gitee.com/huihut/interview/raw/master/images/IP数据报格式.png)

#### ICMP 网际控制报文协议

ICMP 报文格式：

![ICMP 报文格式](https://gitee.com/huihut/interview/raw/master/images/ICMP报文格式.png)

应用：

* PING（Packet InterNet Groper，分组网间探测）测试两个主机之间的连通性
* TTL（Time To Live，生存时间）该字段指定 IP 包被路由器丢弃之前允许通过的最大网段数量

#### 内部网关协议

* RIP（Routing Information Protocol，路由信息协议）
* OSPF（Open Sortest Path First，开放最短路径优先）

#### 外部网关协议

* BGP（Border Gateway Protocol，边界网关协议）

#### IP多播

* IGMP（Internet Group Management Protocol，网际组管理协议）
* 多播路由选择协议

#### VPN 和 NAT

* VPN（Virtual Private Network，虚拟专用网）
* NAT（Network Address Translation，网络地址转换）

#### 路由表包含什么？

1. 网络 ID（Network ID, Network number）：就是目标地址的网络 ID。
2. 子网掩码（subnet mask）：用来判断 IP 所属网络
3. 下一跳地址/接口（Next hop / interface）：就是数据在发送到目标地址的旅途中下一站的地址。其中 interface 指向 next hop（即为下一个 route）。一个自治系统（AS, Autonomous system）中的 route 应该包含区域内所有的子网络，而默认网关（Network id: `0.0.0.0`, Netmask: `0.0.0.0`）指向自治系统的出口。

根据应用和执行的不同，路由表可能含有如下附加信息：

1. 花费（Cost）：就是数据发送过程中通过路径所需要的花费。
2. 路由的服务质量
3. 路由中需要过滤的出/入连接列表

### 运输层

协议：

* TCP（Transmission Control Protocol，传输控制协议）
* UDP（User Datagram Protocol，用户数据报协议）

端口：

应用程序 | FTP | TELNET | SMTP | DNS | TFTP | HTTP | HTTPS | SNMP  
--- | --- | --- |--- |--- |--- |--- |--- |---
端口号 | 21 | 23 | 25 | 53 | 69 | 80 | 443 | 161  

#### TCP

* TCP（Transmission Control Protocol，传输控制协议）是一种面向连接的、可靠的、基于字节流的传输层通信协议，其传输的单位是报文段。

特征：

* 面向连接
* 只能点对点（一对一）通信
* 可靠交互
* 全双工通信
* 面向字节流

TCP 如何保证可靠传输：

* 确认和超时重传
* 数据合理分片和排序
* 流量控制
* 拥塞控制
* 数据校验

TCP 报文结构

![TCP 报文](https://gitee.com/huihut/interview/raw/master/images/TCP报文.png)

TCP 首部

![TCP 首部](https://gitee.com/huihut/interview/raw/master/images/TCP首部.png)

TCP：状态控制码（Code，Control Flag），占 6 比特，含义如下：

* URG：紧急比特（urgent），当 `URG＝1` 时，表明紧急指针字段有效，代表该封包为紧急封包。它告诉系统此报文段中有紧急数据，应尽快传送(相当于高优先级的数据)， 且上图中的 Urgent Pointer 字段也会被启用。
* ACK：确认比特（Acknowledge）。只有当 `ACK＝1` 时确认号字段才有效，代表这个封包为确认封包。当 `ACK＝0` 时，确认号无效。
* PSH：（Push function）若为 1 时，代表要求对方立即传送缓冲区内的其他对应封包，而无需等缓冲满了才送。
* RST：复位比特(Reset)，当 `RST＝1` 时，表明 TCP 连接中出现严重差错（如由于主机崩溃或其他原因），必须释放连接，然后再重新建立运输连接。
* SYN：同步比特(Synchronous)，SYN 置为 1，就表示这是一个连接请求或连接接受报文，通常带有 SYN 标志的封包表示『主动』要连接到对方的意思。
* FIN：终止比特(Final)，用来释放一个连接。当 `FIN＝1` 时，表明此报文段的发送端的数据已发送完毕，并要求释放运输连接。

#### UDP

* UDP（User Datagram Protocol，用户数据报协议）是 OSI（Open System Interconnection 开放式系统互联） 参考模型中一种无连接的传输层协议，提供面向事务的简单不可靠信息传送服务，其传输的单位是用户数据报。

特征：

* 无连接
* 尽最大努力交付
* 面向报文
* 没有拥塞控制
* 支持一对一、一对多、多对一、多对多的交互通信
* 首部开销小

UDP 报文结构

![UDP 报文](https://gitee.com/huihut/interview/raw/master/images/UDP报文.png)

UDP 首部

![UDP 首部](https://gitee.com/huihut/interview/raw/master/images/UDP首部.png)

> TCP/UDP 图片来源于：<https://github.com/JerryC8080/understand-tcp-udp>

#### TCP 与 UDP 的区别

1. TCP 面向连接，UDP 是无连接的；
2. TCP 提供可靠的服务，也就是说，通过 TCP 连接传送的数据，无差错，不丢失，不重复，且按序到达；UDP 尽最大努力交付，即不保证可靠交付
3. TCP 的逻辑通信信道是全双工的可靠信道；UDP 则是不可靠信道
4. 每一条 TCP 连接只能是点到点的；UDP 支持一对一，一对多，多对一和多对多的交互通信
5. TCP 面向字节流（可能出现黏包问题），实际上是 TCP 把数据看成一连串无结构的字节流；UDP 是面向报文的（不会出现黏包问题）
6. UDP 没有拥塞控制，因此网络出现拥塞不会使源主机的发送速率降低（对实时应用很有用，如 IP 电话，实时视频会议等）
7. TCP 首部开销20字节；UDP 的首部开销小，只有 8 个字节

#### TCP 黏包问题

##### 黏包原因

TCP 是一个基于字节流的传输服务（UDP 基于报文的），“流” 意味着 TCP 所传输的数据是没有边界的。所以可能会出现两个数据包黏在一起的情况。

##### 解决

* 发送定长包。如果每个消息的大小都是一样的，那么在接收对等方只要累计接收数据，直到数据等于一个定长的数值就将它作为一个消息。
* 包头加上包体长度。包头是定长的 4 个字节，说明了包体的长度。接收对等方先接收包头长度，依据包头长度来接收包体。
* 在数据包之间设置边界，如添加特殊符号 `\r\n` 标记。FTP 协议正是这么做的。但问题在于如果数据正文中也含有 `\r\n`，则会误判为消息的边界。
* 使用更加复杂的应用层协议。

#### TCP 流量控制

##### 概流量控制念

流量控制（flow control）就是让发送方的发送速率不要太快，要让接收方来得及接收。

##### 方法

利用可变窗口进行流量控制

![1](https://gitee.com/huihut/interview/raw/master/images/利用可变窗口进行流量控制举例.png)

#### TCP 拥塞控制

##### 拥塞控制概念

拥塞控制就是防止过多的数据注入到网络中，这样可以使网络中的路由器或链路不致过载。

##### 拥塞控制方法

* 慢开始( slow-start )
* 拥塞避免( congestion avoidance )
* 快重传( fast retransmit )
* 快恢复( fast recovery )

TCP的拥塞控制图

![1](https://gitee.com/huihut/interview/raw/master/images/TCP拥塞窗口cwnd在拥塞控制时的变化情况.png)
![2](https://gitee.com/huihut/interview/raw/master/images/快重传示意图.png)
![3](https://gitee.com/huihut/interview/raw/master/images/TCP的拥塞控制流程图.png)

#### TCP 传输连接管理

> 因为 TCP 三次握手建立连接、四次挥手释放连接很重要，所以附上《计算机网络（第 7 版）-谢希仁》书中对此章的详细描述：<https://gitee.com/huihut/interview/raw/master/images/TCP-transport-connection-management.png>

##### TCP 三次握手建立连接

![UDP 报文](https://gitee.com/huihut/interview/raw/master/images/TCP三次握手建立连接.png)

【TCP 建立连接全过程解释】

1. 客户端发送 SYN 给服务器，说明客户端请求建立连接；
2. 服务端收到客户端发的 SYN，并回复 SYN+ACK 给客户端（同意建立连接）；
3. 客户端收到服务端的 SYN+ACK 后，回复 ACK 给服务端（表示客户端收到了服务端发的同意报文）；
4. 服务端收到客户端的 ACK，连接已建立，可以数据传输。

##### TCP 为什么要进行三次握手？

【答案一】因为信道不可靠，而 TCP 想在不可靠信道上建立可靠地传输，那么三次通信是理论上的最小值。（而 UDP 则不需建立可靠传输，因此 UDP 不需要三次握手。）

> [Google Groups . TCP 建立连接为什么是三次握手？{技术}{网络通信}](https://groups.google.com/forum/#!msg/pongba/kF6O7-MFxM0/5S7zIJ4yqKUJ)

【答案二】因为双方都需要确认对方收到了自己发送的序列号，确认过程最少要进行三次通信。

> [知乎 . TCP 为什么是三次握手，而不是两次或四次？](https://www.zhihu.com/question/24853633/answer/115173386)

【答案三】为了防止已失效的连接请求报文段突然又传送到了服务端，因而产生错误。

> [《计算机网络（第 7 版）-谢希仁》](https://gitee.com/huihut/interview/raw/master/images/TCP-transport-connection-management.png)

##### TCP 四次挥手释放连接

![UDP 报文](https://gitee.com/huihut/interview/raw/master/images/TCP四次挥手释放连接.png)

【TCP 释放连接全过程解释】

1. 客户端发送 FIN 给服务器，说明客户端不必发送数据给服务器了（请求释放从客户端到服务器的连接）；
2. 服务器接收到客户端发的 FIN，并回复 ACK 给客户端（同意释放从客户端到服务器的连接）；
3. 客户端收到服务端回复的 ACK，此时从客户端到服务器的连接已释放（但服务端到客户端的连接还未释放，并且客户端还可以接收数据）；
4. 服务端继续发送之前没发完的数据给客户端；
5. 服务端发送 FIN+ACK 给客户端，说明服务端发送完了数据（请求释放从服务端到客户端的连接，就算没收到客户端的回复，过段时间也会自动释放）；
6. 客户端收到服务端的 FIN+ACK，并回复 ACK 给服务端（同意释放从服务端到客户端的连接）；
7. 服务端收到客户端的 ACK 后，释放从服务端到客户端的连接。

##### TCP 为什么要进行四次挥手？

【问题一】TCP 为什么要进行四次挥手？ / 为什么 TCP 建立连接需要三次，而释放连接则需要四次？

【答案一】因为 TCP 是全双工模式，客户端请求关闭连接后，客户端向服务端的连接关闭（一二次挥手），服务端继续传输之前没传完的数据给客户端（数据传输），服务端向客户端的连接关闭（三四次挥手）。所以 TCP 释放连接时服务器的 ACK 和 FIN 是分开发送的（中间隔着数据传输），而 TCP 建立连接时服务器的 ACK 和 SYN 是一起发送的（第二次握手），所以 TCP 建立连接需要三次，而释放连接则需要四次。

【问题二】为什么 TCP 连接时可以 ACK 和 SYN 一起发送，而释放时则 ACK 和 FIN 分开发送呢？（ACK 和 FIN 分开是指第二次和第三次挥手）

【答案二】因为客户端请求释放时，服务器可能还有数据需要传输给客户端，因此服务端要先响应客户端 FIN 请求（服务端发送 ACK），然后数据传输，传输完成后，服务端再提出 FIN 请求（服务端发送 FIN）；而连接时则没有中间的数据传输，因此连接时可以 ACK 和 SYN 一起发送。

【问题三】为什么客户端释放最后需要 TIME-WAIT 等待 2MSL 呢？

【答案三】

1. 为了保证客户端发送的最后一个 ACK 报文能够到达服务端。若未成功到达，则服务端超时重传 FIN+ACK 报文段，客户端再重传 ACK，并重新计时。
2. 防止已失效的连接请求报文段出现在本连接中。TIME-WAIT 持续 2MSL 可使本连接持续的时间内所产生的所有报文段都从网络中消失，这样可使下次连接中不会出现旧的连接报文段。

#### TCP 有限状态机

TCP 有限状态机图片

![TCP 的有限状态机](https://gitee.com/huihut/interview/raw/master/images/TCP的有限状态机.png)

### 应用层

#### DNS

* DNS（Domain Name System，域名系统）是互联网的一项服务。它作为将域名和 IP 地址相互映射的一个分布式数据库，能够使人更方便地访问互联网。DNS 使用 TCP 和 UDP 端口 53。当前，对于每一级域名长度的限制是 63 个字符，域名总长度则不能超过 253 个字符。

域名：

* `域名 ::= {<三级域名>.<二级域名>.<顶级域名>}`，如：`blog.huihut.com`

#### FTP

* FTP（File Transfer Protocol，文件传输协议）是用于在网络上进行文件传输的一套标准协议，使用客户/服务器模式，使用 TCP 数据报，提供交互式访问，双向传输。
* TFTP（Trivial File Transfer Protocol，简单文件传输协议）一个小且易实现的文件传输协议，也使用客户-服务器方式，使用UDP数据报，只支持文件传输而不支持交互，没有列目录，不能对用户进行身份鉴定

#### TELNET

* TELNET 协议是 TCP/IP 协议族中的一员，是 Internet 远程登陆服务的标准协议和主要方式。它为用户提供了在本地计算机上完成远程主机工作的能力。

* HTTP（HyperText Transfer Protocol，超文本传输协议）是用于从 WWW（World Wide Web，万维网）服务器传输超文本到本地浏览器的传送协议。

* SMTP（Simple Mail Transfer Protocol，简单邮件传输协议）是一组用于由源地址到目的地址传送邮件的规则，由它来控制信件的中转方式。SMTP 协议属于 TCP/IP 协议簇，它帮助每台计算机在发送或中转信件时找到下一个目的地。
* Socket 建立网络通信连接至少要一对端口号（Socket）。Socket 本质是编程接口（API），对 TCP/IP 的封装，TCP/IP 也要提供可供程序员做网络开发所用的接口，这就是 Socket 编程接口。

#### WWW

* WWW（World Wide Web，环球信息网，万维网）是一个由许多互相链接的超文本组成的系统，通过互联网访问

##### URL

* URL（Uniform Resource Locator，统一资源定位符）是因特网上标准的资源的地址（Address）

标准格式：

* `协议类型:[//服务器地址[:端口号]][/资源层级UNIX文件路径]文件名[?查询][#片段ID]`

完整格式：

* `协议类型:[//[访问资源需要的凭证信息@]服务器地址[:端口号]][/资源层级UNIX文件路径]文件名[?查询][#片段ID]`

> 其中【访问凭证信息@；:端口号；?查询；#片段ID】都属于选填项  
> 如：`https://github.com/huihut/interview#cc`

##### HTTP

HTTP（HyperText Transfer Protocol，超文本传输协议）是一种用于分布式、协作式和超媒体信息系统的应用层协议。HTTP 是万维网的数据通信的基础。

请求方法

方法 | 意义
--- | ---
OPTIONS | 请求一些选项信息，允许客户端查看服务器的性能
GET | 请求指定的页面信息，并返回实体主体
HEAD | 类似于 get 请求，只不过返回的响应中没有具体的内容，用于获取报头
POST | 向指定资源提交数据进行处理请求（例如提交表单或者上传文件）。数据被包含在请求体中。POST请求可能会导致新的资源的建立和/或已有资源的修改
PUT | 从客户端向服务器传送的数据取代指定的文档的内容
DELETE | 请求服务器删除指定的页面
TRACE | 回显服务器收到的请求，主要用于测试或诊断

状态码（Status-Code）

* 1xx：表示通知信息，如请求收到了或正在进行处理
  * 100 Continue：继续，客户端应继续其请求
  * 101 Switching Protocols 切换协议。服务器根据客户端的请求切换协议。只能切换到更高级的协议，例如，切换到 HTTP 的新版本协议
* 2xx：表示成功，如接收或知道了
  * 200 OK: 请求成功
* 3xx：表示重定向，如要完成请求还必须采取进一步的行动
  * 301 Moved Permanently: 永久移动。请求的资源已被永久的移动到新 URL，返回信息会包括新的 URL，浏览器会自动定向到新 URL。今后任何新的请求都应使用新的 URL 代替
* 4xx：表示客户的差错，如请求中有错误的语法或不能完成
  * 400 Bad Request: 客户端请求的语法错误，服务器无法理解
  * 401 Unauthorized: 请求要求用户的身份认证
  * 403 Forbidden: 服务器理解请求客户端的请求，但是拒绝执行此请求（权限不够）
  * 404 Not Found: 服务器无法根据客户端的请求找到资源（网页）。通过此代码，网站设计人员可设置 “您所请求的资源无法找到” 的个性页面
  * 408 Request Timeout: 服务器等待客户端发送的请求时间过长，超时
* 5xx：表示服务器的差错，如服务器失效无法完成请求
  * 500 Internal Server Error: 服务器内部错误，无法完成请求
  * 503 Service Unavailable: 由于超载或系统维护，服务器暂时的无法处理客户端的请求。延时的长度可包含在服务器的 Retry-After 头信息中
  * 504 Gateway Timeout: 充当网关或代理的服务器，未及时从远端服务器获取请求

> 更多状态码：[菜鸟教程 . HTTP状态码](http://www.runoob.com/http/http-status-codes.html)

##### HTTPs

* HTTP和HTTPs区别

    HTTP 和 HTTPS 都是传输协议。它们最主要的区别在于安全性和加密机制。

    HTTP (Hyper Text Transfer Protocol) 是一种基于应用层的协议，它规定了客户端和服务器之间数据的传输方式，它通过明文传输数据，不提供任何形式的数据加密，因此信息会被黑客截取和窃听。这样可能会导致数据被篡改，因为身份验证和加密机制缺失，所以它无法保证数据的完整性和安全性。HTTP 协议默认使用 80 端口。

    HTTPS (Hyper Text Transfer Protocol Secure) 是安全版的 HTTP，它在 HTTP 协议基础上添加了 SSL/TLS 传输协议，它通过加密方式保证了数据的安全性和完整性，使得黑客难以窃听你传输的数据。HTTPS 使用 SSL 或 TLS 加密数据，这意味着客户端和服务器之间传输的数据被加密并确认身份，防止数据在网络中被篡改。HTTPS 协议默认使用 443 端口。

    具体来说，HTTPS 通过使用 SSL/TLS 协议来保护 Web 数据传输。SSL 协议使用公钥和私钥来加密数据。公钥表示反映了服务器的身份的一个字符串，而私钥只有服务器自己拥有。当一个客户端向服务器发送请求时，客户端收到由服务器发送的公钥并使用公钥加密数据。此加密过程使用的密钥只有服务器才可以解密，保障了数据传输的机密性和完整性。因此使用 HTTPS 的网站可以保证用户提交的数据不会被窃取或被篡改，从而提高了安全性。

    综上所述，HTTPS 比 HTTP 更加安全，所以现在大部分网站都使用 HTTPS 协议来保护客户数据的安全。

* SSL/TLS协议

##### 其他协议

* SMTP（Simple Main Transfer Protocol，简单邮件传输协议）是在 Internet 传输 Email 的标准，是一个相对简单的基于文本的协议。在其之上指定了一条消息的一个或多个接收者（在大多数情况下被确认是存在的），然后消息文本会被传输。可以很简单地通过 Telnet 程序来测试一个 SMTP 服务器。SMTP 使用 TCP 端口 25。
* DHCP（Dynamic Host Configuration Protocol，动态主机设置协议）是一个局域网的网络协议，使用 UDP 协议工作，主要有两个用途：
  * 用于内部网络或网络服务供应商自动分配 IP 地址给用户
  * 用于内部网络管理员作为对所有电脑作中央管理的手段
* SNMP（Simple Network Management Protocol，简单网络管理协议）构成了互联网工程工作小组（IETF，Internet Engineering Task Force）定义的 Internet 协议族的一部分。该协议能够支持网络管理系统，用以监测连接到网络上的设备是否有任何引起管理上关注的情况。

<a id="database"></a>

## 💾 数据库

> 本节部分知识点来自《数据库系统概论（第 5 版）》

### 基本概念

* 数据（data）：描述事物的符号记录称为数据。
* 数据库（DataBase，DB）：是长期存储在计算机内、有组织的、可共享的大量数据的集合，具有永久存储、有组织、可共享三个基本特点。
* 数据库管理系统（DataBase Management System，DBMS）：是位于用户与操作系统之间的一层数据管理软件。
* 数据库系统（DataBase System，DBS）：是有数据库、数据库管理系统（及其应用开发工具）、应用程序和数据库管理员（DataBase Administrator DBA）组成的存储、管理、处理和维护数据的系统。
* 实体（entity）：客观存在并可相互区别的事物称为实体。
* 属性（attribute）：实体所具有的某一特性称为属性。
* 码（key）：唯一标识实体的属性集称为码。
* 实体型（entity type）：用实体名及其属性名集合来抽象和刻画同类实体，称为实体型。
* 实体集（entity set）：同一实体型的集合称为实体集。
* 联系（relationship）：实体之间的联系通常是指不同实体集之间的联系。
* 模式（schema）：模式也称逻辑模式，是数据库全体数据的逻辑结构和特征的描述，是所有用户的公共数据视图。
* 外模式（external schema）：外模式也称子模式（subschema）或用户模式，它是数据库用户（包括应用程序员和最终用户）能够看见和使用的局部数据的逻辑结构和特征的描述，是数据库用户的数据视图，是与某一应用有关的数据的逻辑表示。
* 内模式（internal schema）：内模式也称为存储模式（storage schema），一个数据库只有一个内模式。他是数据物理结构和存储方式的描述，是数据库在数据库内部的组织方式。

### 常用数据模型

* 层次模型（hierarchical model）
* 网状模型（network model）
* 关系模型（relational model）
    * 关系（relation）：一个关系对应通常说的一张表
    * 元组（tuple）：表中的一行即为一个元组
    * 属性（attribute）：表中的一列即为一个属性
    * 码（key）：表中可以唯一确定一个元组的某个属性组
    * 域（domain）：一组具有相同数据类型的值的集合
    * 分量：元组中的一个属性值
    * 关系模式：对关系的描述，一般表示为 `关系名(属性1, 属性2, ..., 属性n)`
* 面向对象数据模型（object oriented data model）
* 对象关系数据模型（object relational data model）
* 半结构化数据模型（semistructure data model）

### 常用 SQL 操作

<table>
  <tr>
    <th>对象类型</th>
    <th>对象</th>
    <th>操作类型</th>
  </tr>
  <tr>
    <td rowspan="4">数据库模式</td>
    <td>模式</td>
    <td><code>CREATE SCHEMA</code></td>
  </tr>
  <tr>
    <td>基本表</td>
    <td><code>CREATE SCHEMA</code>，<code>ALTER TABLE</code></td>
  </tr>
    <tr>
    <td>视图</td>
    <td><code>CREATE VIEW</code></td>
  </tr>
    <tr>
    <td>索引</td>
    <td><code>CREATE INDEX</code></td>
  </tr>
    <tr>
    <td rowspan="2">数据</td>
    <td>基本表和视图</td>
    <td><code>SELECT</code>，<code>INSERT</code>，<code>UPDATE</code>，<code>DELETE</code>，<code>REFERENCES</code>，<code>ALL PRIVILEGES</code></td>
  </tr>
    <tr>
    <td>属性列</td>
    <td><code>SELECT</code>，<code>INSERT</code>，<code>UPDATE</code>，<code>REFERENCES</code>，<code>ALL PRIVILEGES</code></td>
  </tr>
</table>

> SQL 语法教程：[runoob . SQL 教程](http://www.runoob.com/sql/sql-tutorial.html)

### 关系型数据库

* 基本关系操作：查询（选择、投影、连接（等值连接、自然连接、外连接（左外连接、右外连接））、除、并、差、交、笛卡尔积等）、插入、删除、修改
* 关系模型中的三类完整性约束：实体完整性、参照完整性、用户定义的完整性

#### 索引

* 数据库索引：顺序索引、B+ 树索引、hash 索引
* [MySQL 索引背后的数据结构及算法原理](http://blog.codinglabs.org/articles/theory-of-mysql-index.html)

### 数据库完整性

* 数据库的完整性是指数据的正确性和相容性。
    * 完整性：为了防止数据库中存在不符合语义（不正确）的数据。
    * 安全性：为了保护数据库防止恶意破坏和非法存取。
* 触发器：是用户定义在关系表中的一类由事件驱动的特殊过程。

### 关系数据理论

* 数据依赖是一个关系内部属性与属性之间的一种约束关系，是通过属性间值的相等与否体现出来的数据间相关联系。
* 最重要的数据依赖：函数依赖、多值依赖。

#### 范式

* 第一范式（1NF）：属性（字段）是最小单位不可再分。
* 第二范式（2NF）：满足 1NF，每个非主属性完全依赖于主键（消除 1NF 非主属性对码的部分函数依赖）。
* 第三范式（3NF）：满足 2NF，任何非主属性不依赖于其他非主属性（消除 2NF 非主属性对码的传递函数依赖）。
* 鲍依斯-科得范式（BCNF）：满足 3NF，任何非主属性不能对主键子集依赖（消除 3NF 主属性对码的部分和传递函数依赖）。
* 第四范式（4NF）：满足 3NF，属性之间不能有非平凡且非函数依赖的多值依赖（消除 3NF 非平凡且非函数依赖的多值依赖）。

### 数据库恢复

* 事务：是用户定义的一个数据库操作序列，这些操作要么全做，要么全不做，是一个不可分割的工作单位。
* 事物的 ACID 特性：原子性、一致性、隔离性、持续性。
* 恢复的实现技术：建立冗余数据 -> 利用冗余数据实施数据库恢复。
* 建立冗余数据常用技术：数据转储（动态海量转储、动态增量转储、静态海量转储、静态增量转储）、登记日志文件。

### 并发控制

* 事务是并发控制的基本单位。
* 并发操作带来的数据不一致性包括：丢失修改、不可重复读、读 “脏” 数据。
* 并发控制主要技术：封锁、时间戳、乐观控制法、多版本并发控制等。
* 基本封锁类型：排他锁（X 锁 / 写锁）、共享锁（S 锁 / 读锁）。
* 活锁死锁：
    * 活锁：事务永远处于等待状态，可通过先来先服务的策略避免。
    * 死锁：事务永远不能结束
        * 预防：一次封锁法、顺序封锁法；
        * 诊断：超时法、等待图法；
        * 解除：撤销处理死锁代价最小的事务，并释放此事务的所有的锁，使其他事务得以继续运行下去。
* 可串行化调度：多个事务的并发执行是正确的，当且仅当其结果与按某一次序串行地执行这些事务时的结果相同。可串行性时并发事务正确调度的准则。

<a id="usefultools"></a>

## 🔧 实用工具

### 命令行

### git

### vim

### docker

<a id="data-structure"></a>

## 〽️ 数据结构

### `hash`

### `heap`

<a id="algorithm"></a>

## ❓算法

### 动态规划

### 滑动窗口

* [无重复最长子串](https://leetcode.cn/problems/longest-substring-without-repeating-characters/)
* [最小覆盖子串](https://leetcode.cn/problems/minimum-window-substring/)

<a id="design-pattern"></a>

## 📏 设计模式

> 各大设计模式例子参考：[CSDN专栏 . C++ 设计模式](https://blog.csdn.net/liang19890820/article/details/66974516) 系列博文

[设计模式工程目录](DesignPattern)

### 单例模式

[单例模式例子](DesignPattern/SingletonPattern)

### 抽象工厂模式

[抽象工厂模式例子](DesignPattern/AbstractFactoryPattern)

### 适配器模式

[适配器模式例子](DesignPattern/AdapterPattern)

### 桥接模式

[桥接模式例子](DesignPattern/BridgePattern)

### 观察者模式

[观察者模式例子](DesignPattern/ObserverPattern)

### 设计模式的六大原则

* 单一职责原则（SRP，Single Responsibility Principle）
* 里氏替换原则（LSP，Liskov Substitution Principle）
* 依赖倒置原则（DIP，Dependence Inversion Principle）
* 接口隔离原则（ISP，Interface Segregation Principle）
* 迪米特法则（LoD，Law of Demeter）
* 开放封闭原则（OCP，Open Close Principle）

<a id="link-loading-library"></a>

## ⚙️ 链接装载库

> 本节部分知识点来自《程序员的自我修养——链接装载库》

### 内存、栈、堆

一般应用程序内存空间有如下区域：

* 栈：由操作系统自动分配释放，存放函数的参数值、局部变量等的值，用于维护函数调用的上下文
* 堆：一般由程序员分配释放，若程序员不释放，程序结束时可能由操作系统回收，用来容纳应用程序动态分配的内存区域
* 可执行文件映像：存储着可执行文件在内存中的映像，由装载器装载是将可执行文件的内存读取或映射到这里
* 保留区：保留区并不是一个单一的内存区域，而是对内存中受到保护而禁止访问的内存区域的总称，如通常 C 语言讲无效指针赋值为 0（NULL），因此 0 地址正常情况下不可能有效的访问数据

#### 栈

栈保存了一个函数调用所需要的维护信息，常被称为堆栈帧（Stack Frame）或活动记录（Activate Record），一般包含以下几方面：

* 函数的返回地址和参数
* 临时变量：包括函数的非静态局部变量以及编译器自动生成的其他临时变量
* 保存上下文：包括函数调用前后需要保持不变的寄存器

#### 堆

堆分配算法：

* 空闲链表（Free List）
* 位图（Bitmap）
* 对象池

#### “段错误（segment fault）” 或 “非法操作，该内存地址不能 read/write”

典型的非法指针解引用造成的错误。当指针指向一个不允许读写的内存地址，而程序却试图利用指针来读或写该地址时，会出现这个错误。

普遍原因：

* 将指针初始化为 NULL，之后没有给它一个合理的值就开始使用指针
* 没用初始化栈中的指针，指针的值一般会是随机数，之后就直接开始使用指针

### 编译链接

#### 各平台文件格式

平台 | 可执行文件 | 目标文件 | 动态库/共享对象 | 静态库
---|---|---|---|---
Windows|exe|obj|dll|lib
Unix/Linux|ELF、out|o|so|a
Mac|Mach-O|o|dylib、tbd、framework|a、framework

#### 编译链接过程

1. 预编译（预编译器处理如 `#include`、`#define` 等预编译指令，生成 `.i` 或 `.ii` 文件）
2. 编译（编译器进行词法分析、语法分析、语义分析、中间代码生成、目标代码生成、优化，生成 `.s` 文件）
3. 汇编（汇编器把汇编码翻译成机器码，生成 `.o` 文件）
4. 链接（连接器进行地址和空间分配、符号决议、重定位，生成 `.out` 文件）

> 现在版本 GCC 把预编译和编译合成一步，预编译编译程序 cc1、汇编器 as、连接器 ld
> MSVC 编译环境，编译器 cl、连接器 link、可执行文件查看器 dumpbin

#### 目标文件

编译器编译源代码后生成的文件叫做目标文件。目标文件从结构上讲，它是已经编译后的可执行文件格式，只是还没有经过链接的过程，其中可能有些符号或有些地址还没有被调整。

> 可执行文件（Windows 的 `.exe` 和 Linux 的 `ELF`）、动态链接库（Windows 的 `.dll` 和 Linux 的 `.so`）、静态链接库（Windows 的 `.lib` 和 Linux 的 `.a`）都是按照可执行文件格式存储（Windows 按照 PE-COFF，Linux 按照 ELF）

##### 目标文件格式

* Windows 的 PE（Portable Executable），或称为 PE-COFF，`.obj` 格式
* Linux 的 ELF（Executable Linkable Format），`.o` 格式
* Intel/Microsoft 的 OMF（Object Module Format）
* Unix 的 `a.out` 格式
* MS-DOS 的 `.COM` 格式

> PE 和 ELF 都是 COFF（Common File Format）的变种

##### 目标文件存储结构

段 | 功能
--- | ---
File Header | 文件头，描述整个文件的文件属性（包括文件是否可执行、是静态链接或动态连接及入口地址、目标硬件、目标操作系统等）
.text section | 代码段，执行语句编译成的机器代码
.data section | 数据段，已初始化的全局变量和局部静态变量
.bss section | BSS 段（Block Started by Symbol），未初始化的全局变量和局部静态变量（因为默认值为 0，所以只是在此预留位置，不占空间）
.rodata section | 只读数据段，存放只读数据，一般是程序里面的只读变量（如 const 修饰的变量）和字符串常量
.comment section | 注释信息段，存放编译器版本信息
.note.GNU-stack section | 堆栈提示段

> 其他段略

#### 链接的接口————符号

在链接中，目标文件之间相互拼合实际上是目标文件之间对地址的引用，即对函数和变量的地址的引用。我们将函数和变量统称为符号（Symbol），函数名或变量名就是符号名（Symbol Name）。

如下符号表（Symbol Table）：

Symbol（符号名） | Symbol Value （地址）
--- | ---
main| 0x100
Add | 0x123
... | ...

### Linux 的共享库（Shared Library）

Linux 下的共享库就是普通的 ELF 共享对象。

共享库版本更新应该保证二进制接口 ABI（Application Binary Interface）的兼容

#### 命名

`libname.so.x.y.z`

* x：主版本号，不同主版本号的库之间不兼容，需要重新编译
* y：次版本号，高版本号向后兼容低版本号
* z：发布版本号，不对接口进行更改，完全兼容

#### 路径

大部分包括 Linux 在内的开源系统遵循 FHS（File Hierarchy Standard）的标准，这标准规定了系统文件如何存放，包括各个目录结构、组织和作用。

* `/lib`：存放系统最关键和最基础的共享库，如动态链接器、C 语言运行库、数学库等
* `/usr/lib`：存放非系统运行时所需要的关键性的库，主要是开发库
* `/usr/local/lib`：存放跟操作系统本身并不十分相关的库，主要是一些第三方应用程序的库

> 动态链接器会在 `/lib`、`/usr/lib` 和由 `/etc/ld.so.conf` 配置文件指定的，目录中查找共享库

#### 环境变量

* `LD_LIBRARY_PATH`：临时改变某个应用程序的共享库查找路径，而不会影响其他应用程序
* `LD_PRELOAD`：指定预先装载的一些共享库甚至是目标文件
* `LD_DEBUG`：打开动态链接器的调试功能

#### so 共享库的编写

使用 CLion 编写共享库

创建一个名为 MySharedLib 的共享库

CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(MySharedLib)

set(CMAKE_CXX_STANDARD 11)

add_library(MySharedLib SHARED library.cpp library.h)
```

library.h

```cpp
#ifndef MYSHAREDLIB_LIBRARY_H
#define MYSHAREDLIB_LIBRARY_H

// 打印 Hello World!
void hello();

// 使用可变模版参数求和
template <typename T>
T sum(T t)
{
    return t;
}
template <typename T, typename ...Types>
T sum(T first, Types ... rest)
{
    return first + sum<T>(rest...);
}

#endif
```

library.cpp

```cpp
#include <iostream>
#include "library.h"

void hello() {
    std::cout << "Hello, World!" << std::endl;
}
```

#### so 共享库的使用（被可执行项目调用）

使用 CLion 调用共享库

创建一个名为 TestSharedLib 的可执行项目

CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(TestSharedLib)

# C++11 编译
set(CMAKE_CXX_STANDARD 11)

# 头文件路径
set(INC_DIR /home/xx/code/clion/MySharedLib)
# 库文件路径
set(LIB_DIR /home/xx/code/clion/MySharedLib/cmake-build-debug)

include_directories(${INC_DIR})
link_directories(${LIB_DIR})
link_libraries(MySharedLib)

add_executable(TestSharedLib main.cpp)

# 链接 MySharedLib 库
target_link_libraries(TestSharedLib MySharedLib)
```

main.cpp

```cpp
#include <iostream>
#include "library.h"
using std::cout;
using std::endl;

int main() {

    hello();
    cout << "1 + 2 = " << sum(1,2) << endl;
    cout << "1 + 2 + 3 = " << sum(1,2,3) << endl;

    return 0;
}
```

执行结果

```text
Hello, World!
1 + 2 = 3
1 + 2 + 3 = 6
```

### Windows 应用程序入口函数

* GUI（Graphical User Interface）应用，链接器选项：`/SUBSYSTEM:WINDOWS`
* CUI（Console User Interface）应用，链接器选项：`/SUBSYSTEM:CONSOLE`

_tWinMain 与 _tmain 函数声明

```cpp
Int WINAPI _tWinMain(
    HINSTANCE hInstanceExe,
    HINSTANCE,
    PTSTR pszCmdLine,
    int nCmdShow);

int _tmain(
    int argc,
    TCHAR *argv[],
    TCHAR *envp[]);
```

应用程序类型|入口点函数|嵌入可执行文件的启动函数
---|---|---
处理ANSI字符（串）的GUI应用程序|_tWinMain(WinMain)|WinMainCRTSartup
处理Unicode字符（串）的GUI应用程序|_tWinMain(wWinMain)|wWinMainCRTSartup
处理ANSI字符（串）的CUI应用程序|_tmain(Main)|mainCRTSartup
处理Unicode字符（串）的CUI应用程序|_tmain(wMain)|wmainCRTSartup
动态链接库（Dynamic-Link Library）|DllMain|_DllMainCRTStartup

### Windows 的动态链接库（Dynamic-Link Library）

> 部分知识点来自《Windows 核心编程（第五版）》

#### 用处

* 扩展了应用程序的特性
* 简化了项目管理
* 有助于节省内存
* 促进了资源的共享
* 促进了本地化
* 有助于解决平台间的差异
* 可以用于特殊目的

#### 注意

* 创建 DLL，事实上是在创建可供一个可执行模块调用的函数
* 当一个模块提供一个内存分配函数（malloc、new）的时候，它必须同时提供另一个内存释放函数（free、delete）
* 在使用 C 和 C++ 混编的时候，要使用 extern "C" 修饰符
* 一个 DLL 可以导出函数、变量（避免导出）、C++ 类（导出导入需要同编译器，否则避免导出）
* DLL 模块：cpp 文件中的 __declspec(dllexport) 写在 include 头文件之前
* 调用 DLL 的可执行模块：cpp 文件的 __declspec(dllimport) 之前不应该定义 MYLIBAPI

#### 加载 Windows 程序的搜索顺序

1. 包含可执行文件的目录
2. Windows 的系统目录，可以通过 GetSystemDirectory 得到
3. 16 位的系统目录，即 Windows 目录中的 System 子目录
4. Windows 目录，可以通过 GetWindowsDirectory 得到
5. 进程的当前目录
6. PATH 环境变量中所列出的目录

#### DLL 入口函数

DllMain 函数

```cpp
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // 第一次将一个DLL映射到进程地址空间时调用
        // The DLL is being mapped into the process' address space.
        break;
    case DLL_THREAD_ATTACH:
        // 当进程创建一个线程的时候，用于告诉DLL执行与线程相关的初始化（非主线程执行）
        // A thread is bing created.
        break;
    case DLL_THREAD_DETACH:
        // 系统调用 ExitThread 线程退出前，即将终止的线程通过告诉DLL执行与线程相关的清理
        // A thread is exiting cleanly.
        break;
    case DLL_PROCESS_DETACH:
        // 将一个DLL从进程的地址空间时调用
        // The DLL is being unmapped from the process' address space.
        break;
    }
    return (TRUE); // Used only for DLL_PROCESS_ATTACH
}
```

#### 载入卸载库

LoadLibrary、LoadLibraryExA、LoadPackagedLibrary、FreeLibrary、FreeLibraryAndExitThread 函数声明

```cpp
// 载入库
HMODULE WINAPI LoadLibrary(
  _In_ LPCTSTR lpFileName
);
HMODULE LoadLibraryExA(
  LPCSTR lpLibFileName,
  HANDLE hFile,
  DWORD  dwFlags
);
// 若要在通用 Windows 平台（UWP）应用中加载 Win32 DLL，需要调用 LoadPackagedLibrary，而不是 LoadLibrary 或 LoadLibraryEx
HMODULE LoadPackagedLibrary(
  LPCWSTR lpwLibFileName,
  DWORD   Reserved
);

// 卸载库
BOOL WINAPI FreeLibrary(
  _In_ HMODULE hModule
);
// 卸载库和退出线程
VOID WINAPI FreeLibraryAndExitThread(
  _In_ HMODULE hModule,
  _In_ DWORD   dwExitCode
);
```

#### 显示地链接到导出符号

GetProcAddress 函数声明

```cpp
FARPROC GetProcAddress(
  HMODULE hInstDll,
  PCSTR pszSymbolName  // 只能接受 ANSI 字符串，不能是 Unicode
);
```

#### DumpBin.exe 查看 DLL 信息

在 `VS 的开发人员命令提示符` 使用 `DumpBin.exe` 可查看 DLL 库的导出段（导出的变量、函数、类名的符号）、相对虚拟地址（RVA，relative virtual address）。如：

```text
DUMPBIN -exports D:\mydll.dll
```

#### LoadLibrary 与 FreeLibrary 流程图

LoadLibrary 与 FreeLibrary 流程图

##### LoadLibrary

![WindowsLoadLibrary](https://gitee.com/huihut/interview/raw/master/images/WindowsLoadLibrary.png)

##### FreeLibrary

![WindowsFreeLibrary](https://gitee.com/huihut/interview/raw/master/images/WindowsFreeLibrary.png)

#### DLL 库的编写（导出一个 DLL 模块）

DLL 库的编写（导出一个 DLL 模块）
DLL 头文件

```cpp
// MyLib.h

#ifdef MYLIBAPI

// MYLIBAPI 应该在全部 DLL 源文件的 include "Mylib.h" 之前被定义
// 全部函数/变量正在被导出

#else

// 这个头文件被一个exe源代码模块包含，意味着全部函数/变量被导入
#define MYLIBAPI extern "C" __declspec(dllimport)

#endif

// 这里定义任何的数据结构和符号

// 定义导出的变量（避免导出变量）
MYLIBAPI int g_nResult;

// 定义导出函数原型
MYLIBAPI int Add(int nLeft, int nRight);
```

DLL 源文件

```cpp
// MyLibFile1.cpp

// 包含标准Windows和C运行时头文件
#include <windows.h>

// DLL源码文件导出的函数和变量
#define MYLIBAPI extern "C" __declspec(dllexport)

// 包含导出的数据结构、符号、函数、变量
#include "MyLib.h"

// 将此DLL源代码文件的代码放在此处
int g_nResult;

int Add(int nLeft, int nRight)
{
    g_nResult = nLeft + nRight;
    return g_nResult;
}
```

#### DLL 库的使用（运行时动态链接 DLL）

DLL 库的使用（运行时动态链接 DLL）

```cpp
// A simple program that uses LoadLibrary and 
// GetProcAddress to access myPuts from Myputs.dll. 
 
#include <windows.h> 
#include <stdio.h> 
 
typedef int (__cdecl *MYPROC)(LPWSTR); 
 
int main( void ) 
{ 
    HINSTANCE hinstLib; 
    MYPROC ProcAdd; 
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 
 
    // Get a handle to the DLL module.
 
    hinstLib = LoadLibrary(TEXT("MyPuts.dll")); 
 
    // If the handle is valid, try to get the function address.
 
    if (hinstLib != NULL) 
    { 
        ProcAdd = (MYPROC) GetProcAddress(hinstLib, "myPuts"); 
 
        // If the function address is valid, call the function.
 
        if (NULL != ProcAdd) 
        {
            fRunTimeLinkSuccess = TRUE;
            (ProcAdd) (L"Message sent to the DLL function\n"); 
        }
        // Free the DLL module.
 
        fFreeResult = FreeLibrary(hinstLib); 
    } 

    // If unable to call the DLL function, use an alternative.
    if (! fRunTimeLinkSuccess) 
        printf("Message printed from executable\n"); 

    return 0;
}
```

### 运行库（Runtime Library）

#### 典型程序运行步骤

1. 操作系统创建进程，把控制权交给程序的入口（往往是运行库中的某个入口函数）
2. 入口函数对运行库和程序运行环境进行初始化（包括堆、I/O、线程、全局变量构造等等）。
3. 入口函数初始化后，调用 main 函数，正式开始执行程序主体部分。
4. main 函数执行完毕后，返回到入口函数进行清理工作（包括全局变量析构、堆销毁、关闭I/O等），然后进行系统调用结束进程。

> 一个程序的 I/O 指代程序与外界的交互，包括文件、管程、网络、命令行、信号等。更广义地讲，I/O 指代操作系统理解为 “文件” 的事物。

#### glibc 入口

`_start -> __libc_start_main -> exit -> _exit`

其中 `main(argc, argv, __environ)` 函数在 `__libc_start_main` 里执行。

#### MSVC CRT 入口

`int mainCRTStartup(void)`

执行如下操作：

1. 初始化和 OS 版本有关的全局变量。
2. 初始化堆。
3. 初始化 I/O。
4. 获取命令行参数和环境变量。
5. 初始化 C 库的一些数据。
6. 调用 main 并记录返回值。
7. 检查错误并将 main 的返回值返回。

#### C 语言运行库（CRT）

大致包含如下功能：

* 启动与退出：包括入口函数及入口函数所依赖的其他函数等。
* 标准函数：有 C 语言标准规定的C语言标准库所拥有的函数实现。
* I/O：I/O 功能的封装和实现。
* 堆：堆的封装和实现。
* 语言实现：语言中一些特殊功能的实现。
* 调试：实现调试功能的代码。

#### C语言标准库（ANSI C）

包含：

* 标准输入输出（stdio.h）
* 文件操作（stdio.h）
* 字符操作（ctype.h）
* 字符串操作（string.h）
* 数学函数（math.h）
* 资源管理（stdlib.h）
* 格式转换（stdlib.h）
* 时间/日期（time.h）
* 断言（assert.h）
* 各种类型上的常数（limits.h & float.h）
* 变长参数（stdarg.h）
* 非局部跳转（setjmp.h）

<a id="books"></a>

## 📚 书籍

### 语言

* [C++ Standard Library](/books/C%2B%2B%20Standard%20Library.md)
* [C++并发编程实战](/books/C%2B%2B%E5%B9%B6%E5%8F%91%E7%BC%96%E7%A8%8B%E5%AE%9E%E6%88%98.md)
* [深入理解C++11](/books/%E6%B7%B1%E5%85%A5%E7%90%86%E8%A7%A3C%2B%2B11.md)
* [Effective C++](/books/Effective%20C%2B%2B.md)
* [STL源码剖析](/STL/STL-master/0%20SIG%20STL%E6%BA%90%E7%A0%81%E5%88%86%E6%9E%90.md)

### Linux

* Linux系统编程
* APUE
* [Linux多线程服务端编程](/book_notes/Linux%E5%A4%9A%E7%BA%BF%E7%A8%8B%E6%9C%8D%E5%8A%A1%E7%AB%AF%E7%BC%96%E7%A8%8B.md)

### 系统

* 《深入理解计算机系统》
* 《Windows 核心编程》
* 《Unix 环境高级编程》

### 网络

* 《Unix 网络编程》
* 《TCP/IP 详解》

### 其他

* 《程序员的自我修养》

<a id="cc-project"></a>

## 🔱 C/C++ 项目

### muduo网络库

#### [线程安全的日志系统](/project/log.cpp)

<a id="review-of-brush-questions-website"></a>

## 💯 复习刷题网站

* [cplusplus](http://www.cplusplus.com/)
* [cppreference](https://zh.cppreference.com/w/%E9%A6%96%E9%A1%B5)
* [runoob](http://www.runoob.com/cplusplus/cpp-tutorial.html)
* [leetcode](https://leetcode.com/) | [leetcode-cn](https://leetcode-cn.com/)
* [lintcode](https://www.lintcode.com/)
* [nowcoder](https://www.nowcoder.net/)

<a id="interview-questions-experience"></a>

## 📝 面试题目经验

* [牛客网 . 2020秋招面经大汇总！（岗位划分）](https://www.nowcoder.com/discuss/205497)
* [牛客网 . 【备战秋招】2020届秋招备战攻略](https://www.nowcoder.com/discuss/197116)
* [牛客网 . 2019校招面经大汇总！【每日更新中】](https://www.nowcoder.com/discuss/90907)
* [牛客网 . 2019校招技术类岗位面经汇总【技术类】](https://www.nowcoder.com/discuss/146655)
* [牛客网 . 2018校招笔试真题汇总](https://www.nowcoder.com/discuss/68802)
* [牛客网 . 2017秋季校园招聘笔经面经专题汇总](https://www.nowcoder.com/discuss/12805)
* [牛客网 . 史上最全2017春招面经大合集！！](https://www.nowcoder.com/discuss/25268)
* [牛客网 . 面试题干货在此](https://www.nowcoder.com/discuss/57978)
* [知乎 . 互联网求职路上，你见过哪些写得很好、很用心的面经？最好能分享自己的面经、心路历程。](https://www.zhihu.com/question/29693016)
* [知乎 . 互联网公司最常见的面试算法题有哪些？](https://www.zhihu.com/question/24964987)
* [CSDN . 全面整理的C++面试题](http://blog.csdn.net/ljzcome/article/details/574158)
* [CSDN . 百度研发类面试题（C++方向）](http://blog.csdn.net/Xiongchao99/article/details/74524807?locationNum=6&fps=1)
* [CSDN . c++常见面试题30道](http://blog.csdn.net/fakine/article/details/51321544)
* [CSDN . 腾讯2016实习生面试经验（已经拿到offer)](http://blog.csdn.net/onever_say_love/article/details/51223886)
* [cnblogs . C++面试集锦( 面试被问到的问题 )](https://www.cnblogs.com/Y1Focus/p/6707121.html)
* [cnblogs . C/C++ 笔试、面试题目大汇总](https://www.cnblogs.com/fangyukuan/archive/2010/09/18/1829871.html)
* [cnblogs . 常见C++面试题及基本知识点总结（一）](https://www.cnblogs.com/LUO77/p/5771237.html)
* [segmentfault . C++常见面试问题总结](https://segmentfault.com/a/1190000003745529)

<a id="recruitment-time-post"></a>

## 📆 招聘时间岗位

* [牛客网 . 名企校招日程](https://www.nowcoder.com/school/schedule)

<a id="recommend"></a>

## 👍 内推

* [Github . CyC2018/Job-Recommend](https://github.com/CyC2018/Job-Recommend)：🔎 互联网内推信息（社招、校招、实习）
* [Github . amusi/AI-Job-Recommend](https://github.com/amusi/AI-Job-Recommend)：国内公司人工智能方向（含机器学习、深度学习、计算机视觉和自然语言处理）岗位的招聘信息（含全职、实习和校招）
