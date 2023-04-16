# deque

## __deque_iterator迭代器

### 全局函数

```c++
inline size_t __deque_buf_size(size_t n, size_t sz)
{
  return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}
```

### 类型定义

`deque`是`random_access_iterator_tag`类型. 满足`traits`编程.

```c++
#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
    // 迭代器定义
  typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
  typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(BufSiz, sizeof(T)); }
#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */
template <class T, class Ref, class Ptr>
struct __deque_iterator {
  typedef __deque_iterator<T, T&, T*>             iterator;
  typedef __deque_iterator<T, const T&, const T*> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(0, sizeof(T)); }
#endif
    // deque是random_access_iterator_tag类型
  typedef random_access_iterator_tag iterator_category;
    // 基本类型的定义, 满足traits编程
  typedef T value_type;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  // node
  typedef T** map_pointer;
  map_pointer node;

  typedef __deque_iterator self;
  ...
};
```

```c++
// 满足traits编程
template <class T, class Ref, class Ptr, size_t BufSiz>
inline random_access_iterator_tag
iterator_category(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return random_access_iterator_tag();
}
template <class T, class Ref, class Ptr, size_t BufSiz>
inline T* value_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return 0;
}
template <class T, class Ref, class Ptr, size_t BufSiz>
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return 0;
}
```

### 迭代器关键指针

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
  ...
  typedef T value_type;
  T* cur;
  T* first;
  T* last;
  ...
  // node
  typedef T** map_pointer;
  map_pointer node;
  ...
};
```

每个连续空间有三个关键的指针，`cur`， `first`， `last`， `node`

### 迭代器构造函数

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
  ...
    // 初始化cur指向当前数组位置, last指针数组的尾, node指向y
    __deque_iterator(T* x, map_pointer y)  : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
    // 初始化为一个空的deque
    __deque_iterator() : cur(0), first(0), last(0), node(0) {}
    // 接受一个迭代器
    __deque_iterator(const iterator& x) : cur(x.cur), first(x.first), last(x.last), node(x.node) {}
    ...
};
```

### 迭代器重载

`__deque_iterator`实现了基本运算符, `deque`重载的运算符操作都是调用`__deque_iterator`的运算符.

不过先分析一个待会会用到的函数`set_node`.

因为node是一个指向`*map`的指针, **当数组填充满了后, 要重新指向下一个数组的头, set_node就是更新指向数组的头的功能.**

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
  ...
  void set_node(map_pointer new_node) 
  {
    // 让node指针另一个数组的头, 同时修改头和尾的地址
      node = new_node;
      first = *new_node;
      last = first + difference_type(buffer_size());
    }
    ...
};
```

#### 重载++和--

需要注意++和--都可能出现数组越界, 如果判断要越界就得更新`node`的指向.

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
  ...  
  // 这里需要判断是否达到当前数组的尾部
  self& operator++() {
    ++cur;
    // 达到了尾部就需要更新node的指向
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this; 
  }
  // 同理, 需要判断是否到达数组的头. 到达就要更新node指向
  self& operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }
  
  self operator++(int)  {
    self tmp = *this;
    ++*this;
    return tmp;
  }
  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }
  ...
};
```

因为`deque`是`random_access_iterator_tag`类型, 所以支持直接加减操作.

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
  ...
  // 重载指针
  reference operator*() const { return *cur; }
  reference operator[](difference_type n) const { return *(*this + n); } // 这个会调用重载+运算符
  #ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
  #endif /* __SGI_STL_NO_ARROW_OPERATOR */

  self& operator+=(difference_type n) 
  {
    difference_type offset = n + (cur - first); // 要移动的距离
    // 如果是在的当前数组内并且向前移动就直接指向那个位置就行了.
    if (offset >= 0 && offset < difference_type(buffer_size()))
      cur += n;
      // 向后移动或已经不再当前数组中
    else 
    {
      // 计算需要跨多少个数组
      difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) : -difference_type((-offset - 1) / buffer_size()) - 1;
      set_node(node + node_offset);
      cur = first + (offset - node_offset * difference_type(buffer_size()));
    }
    return *this;
    }
    // 以下都是调用+运算符
  difference_type operator-(const self& x) const 
  {
      return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
    }
    self operator+(difference_type n) const {
      self tmp = *this;
      return tmp += n;
    }

    self& operator-=(difference_type n) { return *this += -n; }
 
    self operator-(difference_type n) const {
        self tmp = *this;
      return tmp -= n;
    }
  // 
    bool operator==(const self& x) const { return cur == x.cur; }
    bool operator!=(const self& x) const { return !(*this == x); }
    bool operator<(const self& x) const 
    {
      return (node == x.node) ? (cur < x.cur) : (node < x.node);
    }
};
```

## deque结构

### 基本类型定义

`deque`满足`traits`编程的嵌套定义类型.

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
public:                         // Basic types
    // 满足traits编程
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

public:                         // Iterators
    // 定义迭代器
#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG
  typedef __deque_iterator<T, T&, T*, BufSiz>              iterator;
  typedef __deque_iterator<T, const T&, const T&, BufSiz>  const_iterator;
#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */
  typedef __deque_iterator<T, T&, T*>                      iterator;
  typedef __deque_iterator<T, const T&, const T*>          const_iterator;
#endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
  typedef reverse_iterator<const_iterator, value_type, const_reference, 
                           difference_type>  
          const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type>
          reverse_iterator; 
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:                      // Internal typedefs
    // map, 指向指针的指针
  typedef pointer* map_pointer;
  typedef simple_alloc<value_type, Alloc> data_allocator;   // value_type类型的空间配置器
  typedef simple_alloc<pointer, Alloc> map_allocator;       // 指针类型的空间配置器
    ...

  iterator start;
  iterator finish;
  map_pointer map;
  size_type map_size;
};
```

注意`map_pointer`是一个指向指针的指针, `deque`就保存一个`map_pointer`, 用它来指向我们分配的内存空间, 用来管理数据.

### 构造函数

有多个重载函数, 接受大部分不同的参数类型. 基本上每一个构造函数都会调用`create_map_and_nodes`, 这就是构造函数的核心

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public:                         // Basic types
  deque() : start(), finish(), map(0), map_size(0)  // 默认构造函数
  {
    create_map_and_nodes(0);
  }
  deque(const deque& x) : start(), finish(), map(0), map_size(0)    // 接受一个deque
  {
    create_map_and_nodes(x.size());
    __STL_TRY {
      uninitialized_copy(x.begin(), x.end(), start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }
    // 接受 n:初始化大小, value:初始化的值
  deque(size_type n, const value_type& value) : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }
  deque(int n, const value_type& value) : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  } 
  deque(long n, const value_type& value) : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }
    // 接受 n:初始化大小
  explicit deque(size_type n) : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value_type());
  }
#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  deque(InputIterator first, InputIterator last) : start(), finish(), map(0), map_size(0)
  {
    range_initialize(first, last, iterator_category(first));
  }
#else /* __STL_MEMBER_TEMPLATES */
  deque(const value_type* first, const value_type* last) : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(last - first);
    __STL_TRY {
      uninitialized_copy(first, last, start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }
    // 接受两个迭代器, 构造一个范围
  deque(const_iterator first, const_iterator last) : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(last - first);
    __STL_TRY {
      uninitialized_copy(first, last, start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

#endif /* __STL_MEMBER_TEMPLATES */
    ...
};
```

`create_map_and_nodes`函数实现.

1. 计算初始化类型参数的个数
2. 两者取最大的. num_nodes是保证前后都留有位置
3. 计算出数组的头前面留出来的位置保存并在nstart.
4. 为每一个a[cur]分配一个buffer_size的数组, 即这样就实现了二维数组即map(*这里描述为二维数组并不准确, 毕竟指针跟数组是不一样的, 二维数组的地址并不是连续的, 这里这样说只是为了描述起来容易理解.*)
5. 修改start, finish以及分别指向的数组的cur指针的位置, 说通俗就是start和finish分别指向第一个和最后一个元素的位置.

```c++
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) 
{
    // 计算初始化类型参数的个数
  size_type num_nodes = num_elements / buffer_size() + 1;
  // 因为deque是头尾插入都是O(1), 就是deque在头和尾都留有空间方便头尾插入
    // 两者取最大的. num_nodes是保证前后都留有位置
  map_size = max(initial_map_size(), num_nodes + 2);
  map = map_allocator::allocate(map_size);  // 分配空间

    // 计算出数组的头前面留出来的位置保存并在nstart.
  map_pointer nstart = map + (map_size - num_nodes) / 2;
  map_pointer nfinish = nstart + num_nodes - 1;
    
  map_pointer cur;
  __STL_TRY 
  {
      // 为每一个a[cur]分配一个buffer_size的数组, 即这样就实现了二维数组即map
    for (cur = nstart; cur <= nfinish; ++cur)
      *cur = allocate_node();
  }
#     ifdef  __STL_USE_EXCEPTIONS 
  catch(...) {
    for (map_pointer n = nstart; n < cur; ++n)
      deallocate_node(*n);
    map_allocator::deallocate(map, map_size);
    throw;
  }
#     endif /* __STL_USE_EXCEPTIONS */
  // 修改start, finish, cur指针的位置
  start.set_node(nstart);
  finish.set_node(nfinish);
  start.cur = start.first;
  finish.cur = finish.first + num_elements % buffer_size();
}
```

#### range_initialize函数

`range_initialize`是保证不同的迭代器类型能正常的工作, 虽然`deque`迭代器是`random_access_iterator_tag`类型, 但是可能我们在执行构造函数的时候, 传入的类型是`list`等其他容器的迭代器, 就会调用该函数.

主要实现不同迭代器之前的差异而考虑的, 迭代器要考虑"向下兼容".

```c++
template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::range_initialize(InputIterator first,
                                                InputIterator last,
                                                input_iterator_tag) {
  create_map_and_nodes(0);
    // 一个个进行插入操作
  for ( ; first != last; ++first)
    push_back(*first);
}

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::range_initialize(ForwardIterator first,
                                                ForwardIterator last,
                                                forward_iterator_tag) {
  size_type n = 0;
    // 计算距离, 申请空间. 失败则释放所有空间
  distance(first, last, n);
  create_map_and_nodes(n);
  __STL_TRY {
    uninitialized_copy(first, last, start);
  }
  __STL_UNWIND(destroy_map_and_nodes());
}
```

**fill_initialize**函数.

```c++
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value) 
{
    // 申请空间
  create_map_and_nodes(n);
  map_pointer cur;
  __STL_TRY {
      //对每个空间进行初始化
    for (cur = start.node; cur < finish.node; ++cur)
      uninitialized_fill(*cur, *cur + buffer_size(), value);
      // 最后一个数组单独处理. 毕竟最后一个数组一般不是会全部填充满
    uninitialized_fill(finish.first, finish.cur, value);
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (map_pointer n = start.node; n < cur; ++n)
      destroy(*n, *n + buffer_size());
    destroy_map_and_nodes();
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}
```

### 析构函数

分步释放内存

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public: 
   ~deque() {
    destroy(start, finish);
    destroy_map_and_nodes();
  }
};
```

`deque`是一个"二维数组"并且每个数组之间并不连续, 所以需要一个数组一个数组的执行释放.

```c++
// This is only used as a cleanup function in catch clauses.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_and_nodes() {
// 便利所有的数组, 一个个析构
  for (map_pointer cur = start.node; cur <= finish.node; ++cur)
    deallocate_node(*cur);
    // 内存释放
  map_allocator::deallocate(map, map_size);
}
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public: 
  pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
  void deallocate_node(pointer n) {
    data_allocator::deallocate(n, buffer_size());
  }
};
```

### deque基本属性获取方法

要注意`deque`的`first`是指向第一个元素的地址, `finish`是指向最后一个元素的后一个地址, 这里两个都是指向一个结构体的指针即迭代器.

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
protected:                      // Internal typedefs
...
    // 获取缓冲区大小
  static size_type buffer_size() {
    return __deque_buf_size(BufSiz, sizeof(value_type));
  }
  static size_type initial_map_size() { return 8; }

protected:                      // Data members
  iterator start;   // 指向第一个元素的地址
  iterator finish;  // 指向最后一个元素的后一个地址, 即尾

  map_pointer map;      // 定义map, 指向指针的指针
  size_type map_size;   // map的实际大小
public:                         // Basic accessors
  iterator begin() { return start; }    // 获取头地址
  const_iterator begin() const { return start; }
  iterator end() { return finish; }     // 获取尾地址
  const_iterator end() const { return finish; }
    // 倒转后获取首尾地址.
  reverse_iterator rbegin() { return reverse_iterator(finish); }
  reverse_iterator rend() { return reverse_iterator(start); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(finish);
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(start);
  }

  // 获取第一个和最后一个元素的值
  reference front() { return *start; }
  reference back() {
    iterator tmp = finish;
    --tmp;
    return *tmp;
  }
  const_reference front() const { return *start; }
  const_reference back() const {
    const_iterator tmp = finish;
    --tmp;
    return *tmp;
  }
  size_type size() const { return finish - start;; }    // 获取数组的大小
  size_type max_size() const { return size_type(-1); }
  bool empty() const { return finish == start; }        // 判断deque是否为空
    
  reference operator[](size_type n) { return start[difference_type(n)]; }
  const_reference operator[](size_type n) const {
    return start[difference_type(n)];
  }
    ...
};
```

### 重载

`deuqe`是`random_access_iterator_tag`类型, 但是这里并没有对+, -进行重载, 其实是迭代器部分我们都已经实现了, 也就不必要再重载该运算符了.

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public:                         // Constructor, destructor.
    // 重载 = 
    // 原deque比赋值的大, 就必须清除多余的元素, 否则就需要将多余的元素进行插入
  deque& operator= (const deque& x) {
    const size_type len = size();
    if (&x != this) {
        // 清楚原deque的多余元素
      if (len >= x.size())
        erase(copy(x.begin(), x.end(), start), finish);
      else {
        const_iterator mid = x.begin() + difference_type(len);
        copy(x.begin(), mid, start);
        insert(finish, mid, x.end());
      }
    }
    return *this;
  }        

#ifdef __STL_NON_TYPE_TMPL_PARAM_BUG
public:
    // 重载==, !=, <
  bool operator==(const deque<T, Alloc, 0>& x) const {
    return size() == x.size() && equal(begin(), end(), x.begin());
  }
  bool operator!=(const deque<T, Alloc, 0>& x) const {
    return size() != x.size() || !equal(begin(), end(), x.begin());
  }
  bool operator<(const deque<T, Alloc, 0>& x) const {
    return lexicographical_compare(begin(), end(), x.begin(), x.end());
  }
#endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */
  ...
};

template <class T, class Alloc, size_t BufSiz>
bool operator==(const deque<T, Alloc, BufSiz>& x,
                const deque<T, Alloc, BufSiz>& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc, size_t BufSiz>
bool operator<(const deque<T, Alloc, BufSiz>& x,
               const deque<T, Alloc, BufSiz>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}
```

## deque函数接口

### push, pop

因为`deque`的是能够双向操作, 所以其push和pop操作都类似于`list`都可以直接有对应的操作. 需要注意的是`list`是链表, 并不会涉及到界线的判断, 而`deque`是由数组来存储的, 就需要随时对界线进行判断.

**push实现**.  

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public:                         // push_* and pop_*
    // 对尾进行插入
    // 判断函数是否达到了数组尾部. 没有达到就直接进行插入
  void push_back(const value_type& t) {
    if (finish.cur != finish.last - 1) {
      construct(finish.cur, t);
      ++finish.cur;
    }
    else
      push_back_aux(t);
  }
    // 对头进行插入
    // 判断函数是否达到了数组头部. 没有达到就直接进行插入
  void push_front(const value_type& t) {
    if (start.cur != start.first) {
      construct(start.cur - 1, t);
      --start.cur;
    }
    else
      push_front_aux(t);
  }
    ...
};
```

如果判断数组越界, 就移动到另一个数组进行push操作.

注意 : `push_back`是先执行构造在移动node, 而`push_front`是先移动node在进行构造. 实现的差异主要是`finish`是指向最后一个元素的后一个地址而`first`指向的就只第一个元素的地址. 下面pop也是一样的.

```c++
// Called only if finish.cur == finish.last - 1.
// 到达了数组的尾部
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
  value_type t_copy = t;
  reserve_map_at_back();
  // 申请空间
  *(finish.node + 1) = allocate_node();
  __STL_TRY {
    // 执行构造
    construct(finish.cur, t_copy);
    // 移动node, 指向下一个数组的头
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;  // cur只指向当前数组的头
  }
  // 如果分配失败, 释放掉该内存
  __STL_UNWIND(deallocate_node(*(finish.node + 1)));
}

// Called only if start.cur == start.first.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
  value_type t_copy = t;
  reserve_map_at_front();
  // 申请空间
  *(start.node - 1) = allocate_node();
  __STL_TRY {
    // 先要移动node, 让其指向上一个数组的尾部
    start.set_node(start.node - 1);
    // cur指向当前数组的尾部
    start.cur = start.last - 1;
    // 执行构造
    construct(start.cur, t_copy);
  }
#     ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    start.set_node(start.node + 1);
    start.cur = start.first;
    deallocate_node(*(start.node - 1));
    throw;
  }
#     endif /* __STL_USE_EXCEPTIONS */
} 
```

**pop实现**.

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public: 
    // 对尾部进行操作
    // 判断是否达到数组的头部. 没有到达就直接释放
    void pop_back() {
    if (finish.cur != finish.first) {
      --finish.cur;
      destroy(finish.cur);
    }
    else
      pop_back_aux();
  }
    // 对头部进行操作
    // 判断是否达到数组的尾部. 没有到达就直接释放
  void pop_front() {
    if (start.cur != start.last - 1) {
      destroy(start.cur);
      ++start.cur;
    }
    else 
      pop_front_aux();
  }
    ...
};
```

pop判断越界后执行以下函数.

```c++
// Called only if finish.cur == finish.first.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>:: pop_back_aux() {
  deallocate_node(finish.first);    // 先调用析构函数
  finish.set_node(finish.node - 1); // 再移动node
  finish.cur = finish.last - 1;     // 然后cur指向当前数组的最后位置
  destroy(finish.cur);              // 最后释放内存空间.
}

// Called only if start.cur == start.last - 1.  Note that if the deque
//  has at least one element (a necessary precondition for this member
//  function), and if start.cur == start.last, then the deque must have
//  at least two nodes.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
  destroy(start.cur);               // 先释放内存空间.
  deallocate_node(start.first);     // 再调用析构函数
  start.set_node(start.node + 1);   // 然后移动node
  start.cur = start.first;          // 最后cur指向当前数组的第一个位置
}
```

**reserve_map_at一类函数**.  pop和push都先调用了reserve_map_at_XX函数, 这些函数主要是为了**判断前后空间是否足够**.

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
  public:
  void new_elements_at_front(size_type new_elements);
  void new_elements_at_back(size_type new_elements);

  void destroy_nodes_at_front(iterator before_start);
  void destroy_nodes_at_back(iterator after_finish);

protected:                      // Allocation of map and nodes
  // Makes sure the map has space for new nodes.  Does not actually
  //  add the nodes.  Can invalidate map pointers.  (And consequently, 
  //  deque iterators.)
  // 始终保证后面要有一个及以上的空数组大小
  void reserve_map_at_back (size_type nodes_to_add = 1) {
    if (nodes_to_add + 1 > map_size - (finish.node - map))
      reallocate_map(nodes_to_add, false);
  }
  // 始终保证前面要有一个及以上的空数组大小
  void reserve_map_at_front (size_type nodes_to_add = 1) {
    if (nodes_to_add > start.node - map)
      reallocate_map(nodes_to_add, true);
  }

  void reallocate_map(size_type nodes_to_add, bool add_at_front);
    ...
};
```

**reallocate_map**函数, 空间不足

1. deque空间实际足够
    1. deque内部进行调整start, 和finish
2. deque空间真的不足
    1. 申请更大的空间
    2. 拷贝元素过去
    3. 修改map和start, finish指向

```c++
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) 
{
    // 保存现在的空间大小和新的空间大小
  size_type old_num_nodes = finish.node - start.node + 1;
  size_type new_num_nodes = old_num_nodes + nodes_to_add;

  map_pointer new_nstart;
    // map_size > 2 * new_num_nodes 发现deque空间还很充足就只是调整deque内部的元素就行了, 没必要重新开空间
    // 这种情况主要出现在一直往首或尾单方向插入元素, 导致首(尾)前面还有很多余留的空间, 这种情况就这样调整
  if (map_size > 2 * new_num_nodes) {
    new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
    if (new_nstart < start.node)
      copy(start.node, finish.node + 1, new_nstart);
    else
      copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
  }
    // 空间是真的不够了
  else {
    size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
    // 分配空间. 重新定位start的位置
    map_pointer new_map = map_allocator::allocate(new_map_size);
    new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
      // 拷贝原deque元素, 最后释放掉原内存空间
    copy(start.node, finish.node + 1, new_nstart);
    map_allocator::deallocate(map, map_size);
      // 调整map
    map = new_map;
    map_size = new_map_size;
  }
    // 重新调整start, finish
  start.set_node(new_nstart);
  finish.set_node(new_nstart + old_num_nodes - 1);
}
```

```c++
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_front(iterator before_start) {
  for (map_pointer n = before_start.node; n < start.node; ++n)
    deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_back(iterator after_finish) {
  for (map_pointer n = after_finish.node; n > finish.node; --n)
    deallocate_node(*n);
}
```

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public:  
  iterator reserve_elements_at_front(size_type n) {
    size_type vacancies = start.cur - start.first;
    if (n > vacancies) 
      new_elements_at_front(n - vacancies);
    return start - difference_type(n);
  }

  iterator reserve_elements_at_back(size_type n) {
    size_type vacancies = (finish.last - finish.cur) - 1;
    if (n > vacancies)
      new_elements_at_back(n - vacancies);
    return finish + difference_type(n);
  }
    ...
};
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_front(size_type new_elements) {
  size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
  reserve_map_at_front(new_nodes);
  size_type i;
  __STL_TRY {
    for (i = 1; i <= new_nodes; ++i)
      *(start.node - i) = allocate_node();
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (size_type j = 1; j < i; ++j)
      deallocate_node(*(start.node - j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_back(size_type new_elements) {
  size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
  reserve_map_at_back(new_nodes);
  size_type i;
  __STL_TRY {
    for (i = 1; i <= new_nodes; ++i)
      *(finish.node + i) = allocate_node();
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (size_type j = 1; j < i; ++j)
      deallocate_node(*(finish.node + j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}
```

### 删除操作

不知道还记得我们最开始构造函数调用`create_map_and_nodes`考虑到`deque`实现前后插入时间复杂度为O(1), 保证了在前后留出了空间, 所以push和pop都可以在前面的数组进行操作.

好了, 现在就来看erase. 因为deque的是由数组构成, 所以地址空间是连续的. 删除也就像`vector`一样, 要移动所有的元素, `deque`为了保证效率尽量高, 就判断删除的位置是中间偏后还是中间偏前来进行移动.

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public:                         // Erase
  iterator erase(iterator pos) 
  {
    iterator next = pos;
    ++next;
    difference_type index = pos - start;
      // 删除的地方是中间偏前, 移动前面的元素
    if (index < (size() >> 1)) 
    {
      copy_backward(start, pos, next);
      pop_front();
    }
      // 删除的地方是中间偏后, 移动后面的元素
    else {
      copy(next, finish, pos);
      pop_back();
    }
    return start + index;
  }
    // 范围删除, 实际也是调用上面的erase函数.
  iterator erase(iterator first, iterator last);
  void clear(); 
    ...
};
```

**erase(iterator first, iterator last)**

```c++

template <class T, class Alloc, size_t BufSize>
deque<T, Alloc, BufSize>::iterator 
deque<T, Alloc, BufSize>::erase(iterator first, iterator last) 
{
  if (first == start && last == finish) {
    clear();
    return finish;
  }
  else {
      // 计算出两个迭代器的距离, 毕竟是连续的, 可以直接计算
    difference_type n = last - first;
      // 同样, 选择前后哪种方法移动.
    difference_type elems_before = first - start;
      // 删除的地方是中间偏前, 移动前面的元素
    if (elems_before < (size() - n) / 2) {
      copy_backward(start, first, last);
      iterator new_start = start + n;
      destroy(start, new_start);
        // 可能会涉及到跨数组的问题(用户使用并不知道)
      for (map_pointer cur = start.node; cur < new_start.node; ++cur)
        data_allocator::deallocate(*cur, buffer_size());
      start = new_start;
    }
      // 删除的地方是中间偏后, 移动后面的元素
    else {
      copy(last, finish, first);
      iterator new_finish = finish - n;
      destroy(new_finish, finish);
        // 可能会涉及到跨数组的问题(用户使用并不知道)
      for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
        data_allocator::deallocate(*cur, buffer_size());
      finish = new_finish;
    }
    return start + elems_before;
  }
}
```

**clear函数**. 删除所有元素. 分两步执行:

1. 从第二个数组开始到倒数第二个数组一次性全部删除, 毕竟中间的数组肯定都是满的, 前后两个数组就不一定是填充满的.
2. 删除前后两个数组的元素

```c++
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
  // 从第二个数组开始到倒数第二个数组一次性全部删除
  // 毕竟中间的数组肯定都是满的, 前后两个数组就不一定是填充满的.
  for (map_pointer node = start.node + 1; node < finish.node; ++node) {
    destroy(*node, *node + buffer_size());
    data_allocator::deallocate(*node, buffer_size());
  }
  // 删除前后两个数组的元素.
  if (start.node != finish.node) {
    destroy(start.cur, start.last);
    destroy(finish.first, finish.cur);
    data_allocator::deallocate(finish.first, buffer_size());
  }
  else
    destroy(start.cur, finish.cur);

  finish = start;
}
```

### swap

`deque`的swap操作也只是交换了start, finish, map, 并没有交换所有的元素.

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
        ...
      void swap(deque& x)
        {
          __STD::swap(start, x.start);
          __STD::swap(finish, x.finish);
          __STD::swap(map, x.map);
          __STD::swap(map_size, x.map_size);
        }
        ...
};
template <class T, class Alloc, size_t BufSiz>
inline void swap(deque<T, Alloc, BufSiz>& x, deque<T, Alloc, BufSiz>& y) {
  x.swap(y);
}
```

### resize函数

**resize函数**. 重新将`deque`进行调整, 实现与`list`一样的.

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public: 
    void resize(size_type new_size) { resize(new_size, value_type()); }
    void resize(size_type new_size, const value_type& x) {
    const size_type len = size();
    // 元素大小大于了要修改的大小, 则释放掉超过的元素
    if (new_size < len) 
      erase(start + new_size, finish);
    // 元素不够, 就从end开始到要求的大小为止都初始化x
    else
      insert(finish, new_size - len, x);
  }
};
```

### insert实现

这里先将`insert`的所有重载函数进行罗列.

```c++
iterator insert(iterator position, const value_type& x);
iterator insert(iterator position) ;

// 调用相同的重载函数
void insert(iterator pos, size_type n, const value_type& x);
void insert(iterator pos, int n, const value_type& x);
void insert(iterator pos, long n, const value_type& x);

void insert(iterator pos, InputIterator first, InputIterator last);
void insert(iterator pos, const value_type* first, const value_type* last);
void insert(iterator pos, const_iterator first, const_iterator last);

void insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag);
void insert(iterator pos, ForwardIterator first, ForwardIterator last,forward_iterator_tag);
```

**iterator insert(iterator position, const value_type& x)**

```c++
template <class T, class Alloc = alloc, size_t BufSiz = 0> 
class deque {
    ...
public:                         // Insert
  iterator insert(iterator position, const value_type& x) {
      // 如果只是在头尾插入, 直接调用push就行了.
    if (position.cur == start.cur) {
      push_front(x);
      return start;
    }
    else if (position.cur == finish.cur) {
      push_back(x);
      iterator tmp = finish;
      --tmp;
      return tmp;
    }
      // 随机插入
    else {
      return insert_aux(position, x);
    }
  }
};
```

**insert(iterator pos, size_type n, const value_type& x)** 在指定的位置插入n个元素并初始化.

```c++
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos, size_type n, const value_type& x) 
{
    // 同样判断是不是直接在头尾进行插入.
  if (pos.cur == start.cur) {
      // 判断还有没有足够的空间
    iterator new_start = reserve_elements_at_front(n);
    uninitialized_fill(new_start, start, x); // 范围初始化
    start = new_start;	// 修改start位置
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);	// 判断还有没有足够的空间
    uninitialized_fill(finish, new_finish, x);	// 范围初始化
    finish = new_finish;	// 修改finish位置
  }	
    // 随机插入
  else 
    insert_aux(pos, n, x);
}
  void insert(iterator pos, int n, const value_type& x) {
    insert(pos, (size_type) n, x);
  }
  void insert(iterator pos, long n, const value_type& x) {
    insert(pos, (size_type) n, x);
  }
```

**void insert(iterator pos, InputIterator first, InputIterator last)**. 通过参数的类型选择最优, 高效率的插入方式.

```c++
template <class InputIterator>
void insert(iterator pos, InputIterator first, InputIterator last) 
{
    insert(pos, first, last, iterator_category(first));
}

// input_iterator_tag类型的迭代器
template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,InputIterator first, InputIterator last,
input_iterator_tag) 
{
  copy(first, last, inserter(*this, pos));	// 直接调用copy函数
}

// forward_iterator_tag类型的迭代器
template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,ForwardIterator first,ForwardIterator last,forward_iterator_tag) 
{
  size_type n = 0;
  distance(first, last, n); // 计算迭代器之间的距离
    // 同样, 首尾插入判断
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(first, last, new_start);
      start = new_start;
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(first, last, finish);
      finish = new_finish;
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
    // 随机插入
  else
    insert_aux(pos, first, last, n);
}
```

```c++
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,const value_type* first,const value_type* last) 
{
  size_type n = last - first;
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(first, last, new_start);
      start = new_start;
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(first, last, finish);
      finish = new_finish;
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
  else
    insert_aux(pos, first, last, n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,const_iterator first,const_iterator last)
{
  size_type n = last - first;
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __STL_TRY {
      uninitialized_copy(first, last, new_start);
      start = new_start;
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __STL_TRY {
      uninitialized_copy(first, last, finish);
      finish = new_finish;
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
  else
    insert_aux(pos, first, last, n);
}
```

### insert_auto

上面对insert函数做了简单的分析, 可以发现基本每一个`insert`重载函数都会调用了`insert_auto`, 现在我们就来分析该函数的实现.

**insert_aux(iterator pos, const value_type& x) **

```c++
template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) 
{
  difference_type index = pos - start;
  value_type x_copy = x;
    // 判断插入的位置离头还是尾比较近
    // 离头进
  if (index < size() / 2) {
    push_front(front());	// 将头往前移动
      // 调整将要移动的距离
    iterator front1 = start;
    ++front1;
    iterator front2 = front1;
    ++front2;
    pos = start + index;
    iterator pos1 = pos;
    ++pos1;
      // 用copy进行调整
    copy(front2, pos1, front1);
  }
    // 离尾近
  else {
    push_back(back());	// 将尾往前移动
      // 调整将要移动的距离
    iterator back1 = finish;
    --back1;
    iterator back2 = back1;
    --back2;
    pos = start + index;
      // 用copy进行调整
    copy_backward(pos, back2, back1);
  }
  *pos = x_copy;
  return pos;
}
```

**insert_aux(iterator pos, size_type n, const value_type& x)**

```c++
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos, size_type n, const value_type& x) 
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  value_type x_copy = x;
    // 判断插入的位置离头还是尾比较近
    // 离头近
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);	// 新的内存空间
    iterator old_start = start;
      // 计算pos的新位置
    pos = start + elems_before;
    __STL_TRY {
        // 到头的距离大于插入的个数n
      if (elems_before >= difference_type(n)) {
          // 一部分一部分的进行调整
        iterator start_n = start + difference_type(n);
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        copy(start_n, pos, old_start);
        fill(pos - difference_type(n), pos, x_copy);
      }
        // 到头的距离不大于插入的个数n
      else {
        __uninitialized_copy_fill(start, pos, new_start, start, x_copy);
        start = new_start;
        fill(old_start, pos, x_copy);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
    // 离尾近. 执行都是一样的
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        fill(pos, pos + difference_type(n), x_copy);
      }
      else {
        __uninitialized_fill_copy(finish, pos + difference_type(n),
                                  x_copy,
                                  pos, finish);
        finish = new_finish;
        fill(pos, old_finish, x_copy);
      }
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
}
```

剩下的就不一一进行分析了, 基本要涉及到的操作前面都已经讲的很明白了.

```c++
#ifdef __STL_MEMBER_TEMPLATES  
template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          ForwardIterator first,
                                          ForwardIterator last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    iterator old_start = start;
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n); 
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        ForwardIterator mid = first;
        advance(mid, difference_type(n) - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        ForwardIterator mid = first;
        advance(mid, elems_after);
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
}

#else /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          const value_type* first,
                                          const value_type* last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    iterator old_start = start;
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n);
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        const value_type* mid = first + (difference_type(n) - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        const value_type* mid = first + elems_after;
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          const_iterator first,
                                          const_iterator last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    iterator old_start = start;
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= n) {
        iterator start_n = start + n;
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        const_iterator mid = first + (n - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = length - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > n) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        const_iterator mid = first + elems_after;
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
    }
    __STL_UNWIND(destroy_nodes_at_back(new_finish));
  }
}
```
