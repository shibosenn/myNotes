template <typename T>
class unique_ptr {
public:
    // 构造函数
    unique_ptr() : ptr_(nullptr) {}
    explicit unique_ptr(T* ptr) : ptr_(ptr) {}

    // 移动构造函数
    unique_ptr(unique_ptr&& other) noexcept { 
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    }

    // 移动赋值运算符
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) {
            reset(other.ptr_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // 析构函数
    ~unique_ptr() noexcept { delete ptr_; }

    // 定义常用的操作符
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }

    // 获取 underlying pointer
    T* get() const noexcept { return ptr_; }

    // 释放所有权并返回 underlying pointer
    T* release() noexcept {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    // 重置指针
    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            delete ptr_;
            ptr_ = ptr;
        }
    }

private:
    T* ptr_;
};