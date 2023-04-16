template <typename T>
class shared_ptr {
public:
    // 构造函数
    shared_ptr() : ptr_(nullptr), count_(new size_t(0)) {}
    explicit shared_ptr(T* ptr) : ptr_(ptr), count_(new size_t(1)) {}

    // 复制构造函数
    shared_ptr(const shared_ptr& other) : ptr_(other.ptr_), count_(other.count_) { ++(*count_); }

    // 复制赋值运算符
    shared_ptr& operator=(const shared_ptr& other) {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            ++(*count_);
        }
        return *this;
    }

    // 移动构造函数
    shared_ptr(shared_ptr&& other) noexcept {
        ptr_ = other.ptr_;
        count_ = other.count_;
        other.ptr_ = nullptr;
        other.count_ = nullptr;
    }

    // 移动赋值运算符
    shared_ptr& operator=(shared_ptr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            other.ptr_ = nullptr;
            other.count_ = nullptr;
        }
        return *this;
    }

    // 析构函数
    ~shared_ptr() { release(); }

    // 定义常用的操作符
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }

    // 获取 underlying pointer
    T* get() const noexcept { return ptr_; }

    // 重置指针
    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            release();
            ptr_ = ptr;
            count_ = new size_t(1);
        }
    }

private:
    // 释放资源
    void release() noexcept {
        if (*count_ == 1) {
            delete ptr_;
            ptr_ = nullptr;
            delete count_;
            count_ = nullptr;
        } else {
            --(*count_);
        }
    }

private:
    T* ptr_;
    size_t* count_; // 引用计数器
};
