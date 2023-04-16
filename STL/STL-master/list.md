# list

## list基本结构框架

`list`将基本的框架分成`__list_node`(实现节点), `__list_iterator`(实现迭代器)两部分方便随时调用. 下面我们就先慢慢分析.

### __list_node链表结构

`__list_node`用来实现节点, 数据结构中就储存前后指针和属性.

```c++
template <class T> struct __list_node 
{
    // 前后指针
    typedef void* void_pointer;
    void_pointer next;
    void_pointer prev;
    // 属性
    T data;
};
```

### __list_iterator结构

#### 基本类型

```c++
template<class T, class Ref, class Ptr> struct __list_iterator 
{
    typedef __list_iterator<T, T&, T*>             iterator;    // 迭代器
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    typedef __list_iterator<T, Ref, Ptr>           self;

    // 迭代器是bidirectional_iterator_tag类型
    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    ... 
};
```

#### 构造函数

```c++
template<class T, class Ref, class Ptr> struct __list_iterator 
{
    ...
    // 定义节点指针
    typedef __list_node<T>* link_type;
    link_type node;
    // 构造函数
    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator& x) : node(x.node) {}
   ... 
};
```

#### 重载

```c++
template<class T, class Ref, class Ptr> struct __list_iterator 
{
    ...
    // 重载
    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }
    // 对*和->操作符进行重载
    reference operator*() const { return (*node).data; }
    #ifndef __SGI_STL_NO_ARROW_OPERATOR
    pointer operator->() const { return &(operator*()); }
    #endif /* __SGI_STL_NO_ARROW_OPERATOR */

    // ++和--是直接操作的指针指向next还是prev, 因为list是一个双向链表
    self& operator++() 
    { 
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int) 
    { 
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() 
    { 
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int) 
    { 
        self tmp = *this;
        --*this;
        return tmp;
    }
};
```

## list结构

### list基本类型定义

```c++
template <class T, class Alloc = alloc>
class list 
{
protected:
    typedef void* void_pointer;
    typedef __list_node<T> list_node;   // 节点
    typedef simple_alloc<list_node, Alloc> list_node_allocator; // 空间配置器
public:      
    // 定义嵌套类型
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef list_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    
protected:
    // 定义一个节点, 这里节点并不是一个指针.
    link_type node;
    
public:
    // 定义迭代器
    typedef __list_iterator<T, T&, T*>             iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    ...
};
```

### list构造和析构函数实现

构造函数前期准备

1. 分配空间`get_node`
2. 释放空间`put_node`
3. 分配并构造`create_node`
4. 析构并释放空间`destroy_node`
5. 对节点进行初始化`empty_initialize`

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
protected:
    // 分配一个元素大小的空间, 返回分配的地址
    link_type get_node() { return list_node_allocator::allocate(); }
    // 释放一个元素大小的内存
    void put_node(link_type p) { list_node_allocator::deallocate(p); }
    // 分配一个元素大小的空间并调用构造初始化内存
    link_type create_node(const T& x) 
    {
      link_type p = get_node();
      __STL_TRY {
        construct(&p->data, x);
      }
      __STL_UNWIND(put_node(p));
      return p;
    }
    // 调用析构并释放一个元素大小的空间
    void destroy_node(link_type p) {
      destroy(&p->data);
      put_node(p);
    }
    // 对节点初始化
    void empty_initialize() 
    { 
      node = get_node();
      node->next = node;
      node->prev = node;
    }  
    ...
};
```

### list 构造函数

1. 多个重载, 以实现直接构造n个节点并初始化一个值, 支持传入迭代器进行范围初始化, 也支持接受一个`list`参数, 同样进行范围初始化.
2. **每个构造函数都会创造一个空的node节点, 为了保证我们在执行任何操作都不会修改迭代器.**

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
protected: 
    // 构造函数
    list() { empty_initialize(); }  // 默认构造函数, 分配一个空的node节点
    // 都调用同一个函数进行初始化
    list(size_type n, const T& value) { fill_initialize(n, value); }
    list(int n, const T& value) { fill_initialize(n, value); }
    list(long n, const T& value) { fill_initialize(n, value); }
    // 分配n个节点
    explicit list(size_type n) { fill_initialize(n, T()); }

#ifdef __STL_MEMBER_TEMPLATES
    // 接受两个迭代器进行范围的初始化
    template <class InputIterator>
      list(InputIterator first, InputIterator last) {
        range_initialize(first, last);
      }
#else /* __STL_MEMBER_TEMPLATES */
    // 接受两个迭代器进行范围的初始化
    list(const T* first, const T* last) { range_initialize(first, last); }
    list(const_iterator first, const_iterator last) {
      range_initialize(first, last);
    }
#endif /* __STL_MEMBER_TEMPLATES */
    // 接受一个list参数, 进行拷贝
    list(const list<T, Alloc>& x) {
      range_initialize(x.begin(), x.end());
    }
    list<T, Alloc>& operator=(const list<T, Alloc>& x);
    ...
};
```

构造函数内大都调用这个函数, 可以看出来`list`在初始化的时候都会**构造一个空的`node`节点**, 然后对元素进行`insert`插入操作.

```c++
    void fill_initialize(size_type n, const T& value) {
      empty_initialize();
      __STL_TRY {
        insert(begin(), n, value);
      }
      __STL_UNWIND(clear(); put_node(node));
    }
```

### list析构函数

```c++
    ~list() {
        // 删除初空节点以外的所有节点
      clear();
        // 删除空节点
      put_node(node);
    }
```

## 基本属性获取

要注意一点`list`中的迭代器一般不会被修改, 因为`node`节点始终指向的一个空节点同时`list`是一个循环的链表, 空节点正好在头和尾的中间, 所以`node.next`就是指向头的指针, `node.prev`就是指向结束的指针, `end`返回的是最后一个数据的后一个地址也就是`node`. 清楚这些后就容易看懂下面怎么获取属性了.

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
public: 
    iterator begin() { return (link_type)((*node).next); }  // 返回指向头的指针
    const_iterator begin() const { return (link_type)((*node).next); }
    iterator end() { return node; } // 返回最后一个元素的后一个的地址
    const_iterator end() const { return node; }
    
    // 这里是为旋转做准备, rbegin返回最后一个地址, rend返回第一个地址. 我们放在配接器里面分析
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { 
      return const_reverse_iterator(end()); 
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { 
      return const_reverse_iterator(begin());
    } 
    
    // 判断是否为空链表, 这是判断只有一个空node来表示链表为空.
    bool empty() const { return node->next == node; }
    // 因为这个链表, 地址并不连续, 所以要自己迭代计算链表的长度.
    size_type size() const {
      size_type result = 0;
      distance(begin(), end(), result);
      return result;
    }
    size_type max_size() const { return size_type(-1); }
    // 返回第一个元素的值
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    // 返回最后一个元素的值
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }
    
    // 交换
    void swap(list<T, Alloc>& x) { __STD::swap(node, x.node); }
    ...
};
template <class T, class Alloc>
inline void swap(list<T, Alloc>& x, list<T, Alloc>& y) 
{
    x.swap(y);
}
```

## list操作

### push和pop操作

因为`list`是一个循环的双链表, 所以push和pop就必须实现是在头插入, 删除还是在尾插入和删除. push操作都调用`insert`函数, pop操作都调用`erase`函数.

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
    // 直接在头部或尾部插入
    void push_front(const T& x) { insert(begin(), x); }
    void push_back(const T& x) { insert(end(), x); }
    // 直接在头部或尾部删除
    void pop_front() { erase(begin()); }
    void pop_back() { 
      iterator tmp = end();
      erase(--tmp);
    }
    ...
};
```

### 删除操作

删除元素的操作大都是由`erase`函数来实现的, 其他的所有函数都是直接或间接调用`erase`. `list`是链表, 所以链表怎么实现删除, list就在怎么操作.

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
    iterator erase(iterator first, iterator last);
    void clear();   
    // 参数是一个迭代器
    // 修改该元素的前后指针指向再单独释放节点就行了
    iterator erase(iterator position) {
      link_type next_node = link_type(position.node->next);
      link_type prev_node = link_type(position.node->prev);
      prev_node->next = next_node;
      next_node->prev = prev_node;
      destroy_node(position.node);
      return iterator(next_node);
    }
    ...
};
// erase的重载, 删除两个迭代器之间的元素
template <class T, class Alloc>
list<T,Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last) 
{
    // 就是一次次调用erase进行删除
    while (first != last) 
        erase(first++);
    return last;
}
// remove调用erase链表清除
template <class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {
  iterator first = begin();
  iterator last = end();
  while (first != last) {
    iterator next = first;
    ++next;
    if (*first == value) erase(first);
    first = next;
  }
}
```

 `clear`函数是删除除空节点以外的所有节点, 即只留下了最初创建的空节点.

```c++
// 删除除空节点以外的所有节点
template <class T, class Alloc> 
void list<T, Alloc>::clear()
{
  link_type cur = (link_type) node->next;
    // 除空节点都删除
  while (cur != node) {
    link_type tmp = cur;
    cur = (link_type) cur->next;
    destroy_node(tmp);
  }
  node->next = node;
  node->prev = node;
}
```

### 重载操作

`list`也提供了基本操作的重载, 所以我们使用`list`也很方便.

#### 相等比较

```c++
// 判断两个list相等
template <class T, class Alloc>
inline bool operator==(const list<T,Alloc>& x, const list<T,Alloc>& y) 
{
  typedef typename list<T,Alloc>::link_type link_type;
  link_type e1 = x.node;
  link_type e2 = y.node;
  link_type n1 = (link_type) e1->next;
  link_type n2 = (link_type) e2->next;
    // 将两个链表执行一一的对比来分析是否相等. 
    // 这里不把元素个数进行一次比较, 主要获取个数时也要遍历整个数组, 所以就不将个数纳入比较
  for ( ; n1 != e1 && n2 != e2 ; n1 = (link_type) n1->next, n2 = (link_type) n2->next)
    if (n1->data != n2->data)
      return false;
  return n1 == e1 && n2 == e2;
}
```

#### 小于比较

```c++
template <class T, class Alloc>
inline bool operator<(const list<T, Alloc>& x, const list<T, Alloc>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}
```

#### 赋值操作

需要考虑两个链表的实际大小不一样时的操作.

1. 原链表大 : 复制完后要删除掉原链表多余的元素
2. 原链表小 : 复制完后要还要将x链表的剩余元素以插入的方式插入到原链表中

```c++
template <class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& x) {
  if (this != &x) {
    iterator first1 = begin();
    iterator last1 = end();
    const_iterator first2 = x.begin();
    const_iterator last2 = x.end();
    // 直到两个链表有一个空间用尽
    while (first1 != last1 && first2 != last2) 
      *first1++ = *first2++;
    //  原链表大, 复制完后要删除掉原链表多余的元素
    if (first2 == last2)
      erase(first1, last1);
    //  原链表小, 复制完后要还要将x链表的剩余元素以插入的方式插入到原链表中
    else
      insert(last1, first2, last2);
  }
  return *this;
}
```

### resize操作

`resize`重新修改`list`的大小

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
    resize(size_type new_size, const T& x);
  void resize(size_type new_size) { resize(new_size, T()); }
    ...
}; 
template <class T, class Alloc>
void list<T, Alloc>::resize(size_type new_size, const T& x)
{
  iterator i = begin();
  size_type len = 0;
  for ( ; i != end() && len < new_size; ++i, ++len)
    ;
  // 如果链表长度大于new_size的大小, 那就删除后面多余的节点
  if (len == new_size)
    erase(i, end());
    // i == end(), 扩大链表的节点
  else                          
    insert(end(), new_size - len, x);
}
```

### unique操作

**`unique`函数是将数值相同且连续的元素删除, 只保留一个副本.** 记住, `unique`*并不是删除所有的相同元素, 而是连续的相同元素, 如果要删除所有相同元素就要对list做一个排序在进行unique操作.*

一般unique同sort一起用的. `sort`函数准备放在下一节来分析.

```c++
template <class T, class Alloc> template <class BinaryPredicate>
void list<T, Alloc>::unique(BinaryPredicate binary_pred) {
  iterator first = begin();
  iterator last = end();
  if (first == last) return;
  iterator next = first;
  // 删除连续相同的元素, 留一个副本
  while (++next != last) {
    if (binary_pred(*first, *next))
      erase(next);
    else
      first = next;
    next = first;
  }
}
```

### insert操作

`insert`函数有很多的重载函数, 满足足够用户的各种插入方法了. 但是最核心的还是`iterator insert(iterator position, const T& x)`, 每一个重载函数都是直接或间接的调用该函数.

`insert`是将元素插入到指定地址的前一个位置.

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
public:
    // 最基本的insert操作, 之插入一个元素
    iterator insert(iterator position, const T& x) 
    {
        // 将元素插入指定位置的前一个地址
      link_type tmp = create_node(x);
      tmp->next = position.node;
      tmp->prev = position.node->prev;
      (link_type(position.node->prev))->next = tmp;
      position.node->prev = tmp;
      return tmp;
    }
    
   // 以下重载函数都是调用iterator insert(iterator position, const T& x)函数
   iterator insert(iterator position) { return insert(position, T()); }
#ifdef __STL_MEMBER_TEMPLATES
    template <class InputIterator>
      void insert(iterator position, InputIterator first, InputIterator last);
#else /* __STL_MEMBER_TEMPLATES */
    void insert(iterator position, const T* first, const T* last);
    void insert(iterator position,
        const_iterator first, const_iterator last);
#endif /* __STL_MEMBER_TEMPLATES */
    void insert(iterator pos, size_type n, const T& x);
    void insert(iterator pos, int n, const T& x) {
      insert(pos, (size_type)n, x);
    }
    void insert(iterator pos, long n, const T& x) {
      insert(pos, (size_type)n, x);
    }
    void resize(size_type new_size, const T& x);
    ...
};

#ifdef __STL_MEMBER_TEMPLATES
template <class T, class Alloc> template <class InputIterator>
void list<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last) 
{
    for ( ; first != last; ++first)
      insert(position, *first);
}
#else /* __STL_MEMBER_TEMPLATES */
template <class T, class Alloc>
void list<T, Alloc>::insert(iterator position, const T* first, const T* last) {
  for ( ; first != last; ++first)
    insert(position, *first);
}
template <class T, class Alloc>
void list<T, Alloc>::insert(iterator position,
    const_iterator first, const_iterator last) {
  for ( ; first != last; ++first)
    insert(position, *first);
}
#endif /* __STL_MEMBER_TEMPLATES */
template <class T, class Alloc>
void list<T, Alloc>::insert(iterator position, size_type n, const T& x) {
  for ( ; n > 0; --n)
    insert(position, x);
}
```

## list操作实现

在分析`sort`之前先来分析`transfer`, `reverse`, `merge`这几个会被调用的函数.

### transfer函数

**`transfer`函数功能是将一段链表插入到我们指定的位置之前**该函数一定要理解, 后面分析的所有函数都是该基础上进行修改的.

`transfer`函数接受3个迭代器. 第一个迭代器表示链表要插入的位置, `first`到`last`最闭右开区间插入到`position`之前.

从`if`下面开始分析(*这里我将源码的执行的先后顺序进行的部分调整, 下面我分析的都是调整顺序过后的代码. 当然我也会把源码顺序写下来, 以便参考*)

- **为了避免待会解释起来太绕口, 这里先统一一下部分名字**

1. `last`的前一个节点叫`last_but_one`
2. `first`的前一个节点叫`zero`

- 好, 现在我们开始分析`transfer`的每一步(*最好在分析的时候在纸上画出两个链表一步步来画*)

1. 第一行.  `last_but_one`的`next`指向插入的`position`节点
2. 第二行. `position`的`next`指向`last_but_one`
3. 第三行. 临时变量`tmp`保存`position`的前一个节点
4. 第四行. `first`的`prev`指向`tmp`
5. 第五行. `position`的前一个节点的`next`指向`first`节点
6. 第六行. `zero`的`next`指向`last`节点
7. 第七行. `last`的`prev`指向`zero`

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
protected:
    void transfer(iterator position, iterator first, iterator last) 
    {
      if (position != last) 
      {
          (*(link_type((*last.node).prev))).next = position.node;
          (*position.node).prev = (*last.node).prev;
          link_type tmp = link_type((*position.node).prev);
          (*first.node).prev = tmp;
          (*(link_type((*position.node).prev))).next = first.node; 
          (*(link_type((*first.node).prev))).next = last.node;
          (*last.node).prev = (*first.node).prev; 
      }
    }
    /*
    void transfer(iterator position, iterator first, iterator last) 
    {
      if (position != last) 
      {
        (*(link_type((*last.node).prev))).next = position.node;
        (*(link_type((*first.node).prev))).next = last.node;
        (*(link_type((*position.node).prev))).next = first.node;  
        link_type tmp = link_type((*position.node).prev);
        (*position.node).prev = (*last.node).prev;
        (*last.node).prev = (*first.node).prev; 
        (*first.node).prev = tmp;
      }
    }
    */
    ...
};
```

**splice** 将两个链表进行合并.

```c++
template <class T, class Alloc = alloc>
class list 
{
    ...
public:
    void splice(iterator position, list& x) {
      if (!x.empty()) 
        transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, list&, iterator i) {
      iterator j = i;
      ++j;
      if (position == i || position == j) return;
      transfer(position, i, j);
    }
    void splice(iterator position, list&, iterator first, iterator last) {
      if (first != last) 
        transfer(position, first, last);
    }
    ...
};
```

### merge函数

`merge`函数接受一个`list`参数.

**`merge`函数是将传入的`list`链表x与原链表按从小到大合并到原链表中(前提是两个链表都是已经从小到大排序了)**. 这里`merge`的核心就是`transfer`函数.

```c++
template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x) {
  iterator first1 = begin();
  iterator last1 = end();
  iterator first2 = x.begin();
  iterator last2 = x.end();
  while (first1 != last1 && first2 != last2)
    if (*first2 < *first1) {
      iterator next = first2;
      // 将first2到first+1的左闭右开区间插入到first1的前面
      // 这就是将first2合并到first1链表中
      transfer(first1, first2, ++next);
      first2 = next;
    }
    else
      ++first1;
      // 如果链表x还有元素则全部插入到first1链表的尾部
  if (first2 != last2) transfer(last1, first2, last2);
}
```

### reverse函数

**`reverse`函数是实现将链表翻转的功能.** 主要是`list`的迭代器基本不会改变的特点, 将每一个元素一个个插入到`begin`之前. 这里注意迭代器不会变, 但是`begin`会改变, 它始终指向第一个元素的地址.

```c++
template <class T, class Alloc>
void list<T, Alloc>::reverse() 
{
  if (node->next == node || link_type(node->next)->next == node) 
    return;
  iterator first = begin();
  ++first;
  while (first != end()) {
    iterator old = first;
    ++first;
      // 将元素插入到begin()之前
    transfer(begin(), old, first);
  }
} 
```

### sort

`list`实现`sort` 功能本身就不容易, 当我分析了之后就对其表示佩服. 严格的说`list`排序的时间复杂度应为`nlog(n)`, 其实现用了归并排序的思想, 将所有元素分成n分, 总共2^n个元素.

这个sort的分析 :

- 这里将每个重要的参数列出来解释其含义

    1. `fill` : 当前可以处理的元素个数为2^fill个
    2. `counter[fill]` : 可以容纳2^(fill+1)个元素
    3. `carry` : 一个临时中转站, 每次将一元素插入到counter[i]链表中.

在处理的元素个数不足2^fill个时，在`counter[i](0<i<fill)`之前转移元素

具体是显示步骤是：

1. 每次读一个数据到`carry`中，并将carry的数据转移到`counter[0]`中
     1. 当`counter[0]`中的数据个数少于2时，持续转移数据到counter[0]中
     2. 当counter[0]的数据个数等于2时，将counter[0]中的数据转移到counter[1]...从counter[i]转移到counter[i+1],直到counter[fill]中数据个数达到2^(fill+1)个。
2. ++fill, 重复步骤1

```c++
//list 不能使用sort函数，因为list的迭代器是bidirectional_iterator, 而sort
//sort函数要求random_access_iterator
template<class T,class Alloc>
void list<T,Alloc>::sort()
{
    //如果元素个数小于等于1，直接返回
    if(node->next==node||node->next->next==node)
    return ;
    list<T,Alloc> carry; //中转站
    list<T,Alloc> counter[64];
    int fill=0;
    while(!empty())
    {
        carry.splice(carry.begin(),*this,begin());  //每次取出一个元素
        int i=0;    
        while(i<fill&&!counter[i].empty())
        {
            counter[i].merge(carry);  //将carry中的元素合并到counter[i]中
            carry.swap(counter[i++]);  //交换之后counter[i-1]为空
        }
        carry.swap(counter[i]);
        if(i==fill) 
            ++fill;
    }
    // 将counter数组链表的所有节点按从小到大的顺序排列存储在counter[fill-1]的链表中
    for(int i=1;i<fill;++i)
    {
        counter[i].merge(counter[i-1]);
    }
    // 最后将couter与carry交换, 实现排序
    swap(counter[fill-1]);
}
```

`sort`用了一个数组链表用来存储2^i个元素, 当上一个元素存储满了之后继续往下一个链表存储, 最后将所有的链表进行`merge`归并(合并), 从而实现了链表的排序.
