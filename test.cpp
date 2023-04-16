
// #include <iostream>
// #include <functional>

// void print_message(const std::string& message) {
//     std::cout << "Message: " << message << std::endl;
// }

// class MessageSender {
// public:
//     void send_message(const std::string& message) const {
//         std::cout << "Sent: " << message << std::endl;
//     }
// };

// int main() {
//     // 1. 普通函数
//     std::function<void(const std::string&)> func1 = print_message;
//     func1("Hello, world!"); // 输出 "Message: Hello, world!"

//     // 2. 成员函数
//     MessageSender sender;
//     std::function<void(MessageSender&, const std::string&)> func2 = &MessageSender::send_message;
//     func2(sender, "Hello, world!"); // 输出 "Sent: Hello, world!"

//     // 3. Lambda 表达式
//     auto lambda = [](const std::string& message) -> void { 
//         std::cout << "Lambda message: " << message << std::endl; 
//     };
//     std::function<void(const std::string&)> func3 = lambda;
//     func3("Hello, world!"); // 输出 "Lambda message: Hello, world!"

//     return 0;
// }


// 在上面的示例中，我们分别使用了普通函数、成员函数和 Lambda 表达式定义了三个可调用对象（函数），然后将这些对象分别存储在 `std::function` 类型的变量中，并调用它们。

// `std::function` 实际上是一个类模板，可以根据函数类型的签名来创建不同的 `std::function` 对象。下面是 `std::function` 的简化实现，展示了如何实现一个具有相似功能的函数对象包装器：

// ```c++
template <typename T>
class my_function;

template <typename R, typename... Args>
class my_function<R(Args...)> {
public:
    my_function() noexcept : ptr(nullptr) {}

    template <typename F>
    my_function(F&& f) : ptr(new func_t<F>(std::forward<F>(f))) {}

    R operator()(Args... args) const {
        return (*ptr)(std::forward<Args>(args)...);
    }

private:
    class base_t {
    public:
        virtual ~base_t() {}
        virtual R operator()(Args...) = 0;
    };

    template <typename F>
    class func_t : public base_t {
    public:
        func_t(F&& f) : func(std::forward<F>(f)) {}

        virtual R operator()(Args... args) override {
            return func(std::forward<Args>(args)...);
        }

    private:
        F func;
    };

private:
    std::unique_ptr<base_t> ptr;
};
// ```

// 在上面的代码中，我们定义了一个基类 `base_t` 和一个派生类 `func_t`，其中 `base_t` 是一个纯虚基类，它提供了调用函数必需的接口，而 `func_t` 类是一个实现了这个接口的具体类，它包装了一个给定的可调用对象 `f`。在包装器的构造函数中，我们使用了一个完美转发来保持输入类型不变，并使用 `std::unique_ptr` 来管理内存所有权。最后，通过重载 `()` 运算符来实现了这个包装器的调用功能。

// 当然，这个简化版的代码并没有考虑像空函数指针等异常情况，而且在实际 C++ 标准库中 `std::function` 包含了更多细节和优化。笔者也仅仅为了展示其原理而提供上述代码参考。