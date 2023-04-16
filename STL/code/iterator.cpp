using namespace std;

template<typename T>
struct iterator_traits {
    using iterator_category = typename T::iterator_category;
    using value_type        = typename T::value_type;
    using difference_type   = typename T::difference_type;
    using pointer           = typename T::pointer;
    using reference         = typename T::reference;
};

template<typename T>
class my_iter {
public:
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;
    using iterator_category = std::random_access_iterator_tag;   // 随机访问迭代器

    my_iter() noexcept : ptr(nullptr) {}
    explicit my_iter(pointer p) noexcept : ptr(p) {}

    reference operator*() const noexcept {
        return *ptr;
    }
    pointer operator->() const noexcept {
        return ptr;
    }
    reference operator[](difference_type n) const noexcept {
        return *(ptr + n);
    }

    my_iter& operator++() noexcept {
        ++ptr;
        return *this;
    }
    my_iter operator++(int) noexcept {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    my_iter& operator--() noexcept {
        --ptr;
        return *this;
    }
    my_iter operator--(int) noexcept {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    my_iter& operator+=(difference_type n) noexcept {
        ptr += n;
        return *this;
    }
    my_iter operator+(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp += n;
    }
    friend my_iter operator+(difference_type n, const my_iter& iter) noexcept {
        return iter + n;
    }

    my_iter& operator-=(difference_type n) noexcept {
        return *this += -n;
    }
    my_iter operator-(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp -= n;
    }
    difference_type operator-(const my_iter& other) const noexcept {
        return ptr - other.ptr;
    }

    bool operator==(const my_iter& other) const noexcept {
        return ptr == other.ptr;
    }
    bool operator!=(const my_iter& other) const noexcept {
        return !(*this == other);
    }
    bool operator<(const my_iter& other) const noexcept {
        return ptr < other.ptr;
    }
    bool operator>(const my_iter& other) const noexcept {
        return other < *this;
    }
    bool operator<=(const my_iter& other) const noexcept {
        return !(other < *this);
    }
    bool operator>=(const my_iter& other) const noexcept {
        return !(*this < other);
    }

private:
    pointer ptr;
};
