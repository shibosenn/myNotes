# set

严格意义说`set`应该是配置器. `set`就是将`RB-tree`作为底层容器, `insert_unique`为核心的配接器.

`set`的主要实现大都是调用`RB-tree`的接口.

## set类型定义

这里的类型的定义要注意一点, 都是`const`类型, 因为set的主键定义后就不能被修改了, 所以这里都是以`const`类型.

```c++
#ifndef __STL_LIMITED_DEFAULT_TEMPLATES
template <class Key, class Compare = less<Key>, class Alloc = alloc>
#else
template <class Key, class Compare, class Alloc = alloc>
#endif
class set {
public:
  // typedefs:
  typedef Key key_type;
  typedef Key value_type;
  typedef Compare key_compare;
  typedef Compare value_compare;
private:
	// 一RB-tree为接口封装
  typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;	
  rep_type t;  // red-black tree representing set
public:
	// 定义的类型都是const类型, 不能修改
  typedef typename rep_type::const_pointer pointer;
  typedef typename rep_type::const_pointer const_pointer;
  typedef typename rep_type::const_reference reference;
  typedef typename rep_type::const_reference const_reference;
  typedef typename rep_type::const_iterator iterator;
  typedef typename rep_type::const_iterator const_iterator;
  typedef typename rep_type::const_reverse_iterator reverse_iterator;
  typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename rep_type::size_type size_type;
  typedef typename rep_type::difference_type difference_type;
  ...
};
```

## 构造函数

构造函数构造成员的时候调用的是RB-tree的`insert_unique`

```c++
class set {
public:
    ...
  set() : t(Compare()) {}
  explicit set(const Compare& comp) : t(comp) {}	// 不能隐式转换

    // 接受两个迭代器
    // 构造函数构造成员的时候调用的是RB-tree的insert_unique
  template <class InputIterator>
  set(InputIterator first, InputIterator last)
    : t(Compare()) { t.insert_unique(first, last); }
  template <class InputIterator>
  set(InputIterator first, InputIterator last, const Compare& comp)
    : t(comp) { t.insert_unique(first, last); }

  set(const value_type* first, const value_type* last) 
    : t(Compare()) { t.insert_unique(first, last); }
  set(const value_type* first, const value_type* last, const Compare& comp)
    : t(comp) { t.insert_unique(first, last); }

  set(const_iterator first, const_iterator last)
    : t(Compare()) { t.insert_unique(first, last); }
  set(const_iterator first, const_iterator last, const Compare& comp)
    : t(comp) { t.insert_unique(first, last); }
    ...
};
```

## 成员属性获取

```c++
class set {
public:
    ...
    // 所有的操作都是通过调用RB-tree获取的
  key_compare key_comp() const { return t.key_comp(); }
  value_compare value_comp() const { return t.key_comp(); }
  iterator begin() const { return t.begin(); }
  iterator end() const { return t.end(); }
  reverse_iterator rbegin() const { return t.rbegin(); } 
  reverse_iterator rend() const { return t.rend(); }
  bool empty() const { return t.empty(); }
  size_type size() const { return t.size(); }
  size_type max_size() const { return t.max_size(); }
    // 交换
  void swap(set<Key, Compare, Alloc>& x) { t.swap(x.t); }

    // 其他的find, count等都是直接调用的RB-tree的接口
  iterator find(const key_type& x) const { return t.find(x); }
  size_type count(const key_type& x) const { return t.count(x); }
  iterator lower_bound(const key_type& x) const {
    return t.lower_bound(x);
  }
  iterator upper_bound(const key_type& x) const {
    return t.upper_bound(x); 
  }
  pair<iterator,iterator> equal_range(const key_type& x) const {
    return t.equal_range(x);
  }
    ...
};
template <class Key, class Compare, class Alloc>
inline void swap(set<Key, Compare, Alloc>& x, set<Key, Compare, Alloc>& y) 
{
  x.swap(y);
}
```

## insert

```c++
class set {
public:
    ...
    // pair类型我们准备下一节分析, 这里是直接调用insert_unique, 返回插入成功就是pair( , true), 插入失败则是( , false)
  typedef  pair<iterator, bool> pair_iterator_bool; 
  pair<iterator,bool> insert(const value_type& x) { 
    pair<typename rep_type::iterator, bool> p = t.insert_unique(x); 
    return pair<iterator, bool>(p.first, p.second);
  }
    // 指定位置的插入
  iterator insert(iterator position, const value_type& x) {
    typedef typename rep_type::iterator rep_iterator;
    return t.insert_unique((rep_iterator&)position, x);
  }
    // 可接受范围插入
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    t.insert_unique(first, last);
  }
    ...
};
```

## erase

```c++
class set {
public:
    ...
     // erase的实现是通过调用RB-tree实现的erase
  void erase(iterator position) { 
    typedef typename rep_type::iterator rep_iterator;
    t.erase((rep_iterator&)position); 
  }
  size_type erase(const key_type& x) { 
    return t.erase(x); 
  }
  void erase(iterator first, iterator last) { 
    typedef typename rep_type::iterator rep_iterator;
    t.erase((rep_iterator&)first, (rep_iterator&)last); 
  }
  void clear() { t.clear(); }
	...
};
```

## 重载

重载运算符也是以`RB-tree`为接口调用.

```c++
class set {
public:
    ...

  set(const set<Key, Compare, Alloc>& x) : t(x.t) {}
  set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) { 
    t = x.t; 
    return *this;
  }
    ...
};

template <class Key, class Compare, class Alloc>
inline bool operator==(const set<Key, Compare, Alloc>& x, 
                       const set<Key, Compare, Alloc>& y) {
  return x.t == y.t;
}

template <class Key, class Compare, class Alloc>
inline bool operator<(const set<Key, Compare, Alloc>& x, 
                      const set<Key, Compare, Alloc>& y) {
  return x.t < y.t;
}
```
