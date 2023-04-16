# multimap

## 定义类型

```c++
#ifndef __STL_LIMITED_DEFAULT_TEMPLATES
template <class Key, class T, class Compare = less<Key>, class Alloc = alloc>
#else
template <class Key, class T, class Compare, class Alloc = alloc>
#endif
class multimap {
public:
// typedefs:
  typedef Key key_type;
  typedef T data_type;
  typedef T mapped_type;
  typedef pair<const Key, T> value_type;	// 同样, 键值不允许进行修改
  typedef Compare key_compare;
private:
  typedef rb_tree<key_type, value_type, 
                  select1st<value_type>, key_compare, Alloc> rep_type;
  rep_type t;  // red-black tree representing multimap
public:
  typedef typename rep_type::pointer pointer;
  typedef typename rep_type::const_pointer const_pointer;
  typedef typename rep_type::reference reference;
  typedef typename rep_type::const_reference const_reference;
  typedef typename rep_type::iterator iterator;
  typedef typename rep_type::const_iterator const_iterator; 
  typedef typename rep_type::reverse_iterator reverse_iterator;
  typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename rep_type::size_type size_type;
  typedef typename rep_type::difference_type difference_type;
...
};
```

## 嵌套类

```c++
class multimap {
public:
  class value_compare : public binary_function<value_type, value_type, bool> {
  friend class multimap<Key, T, Compare, Alloc>;
  protected:
    Compare comp;
    value_compare(Compare c) : comp(c) {}
  public:
      // 仿函数
    bool operator()(const value_type& x, const value_type& y) const {
      return comp(x.first, y.first);
    }
  };
  ...
};
```

## 构造函数

与`map`不同, map是以RB-tree的`insert_uniqual`为接口, 而multimap是以`insert_qual`为接口, 所以允许键值重复.

```c++
class multimap {
	...
public:
// allocation/deallocation
  multimap() : t(Compare()) { }
  explicit multimap(const Compare& comp) : t(comp) { }

#ifdef __STL_MEMBER_TEMPLATES  
  template <class InputIterator>
  multimap(InputIterator first, InputIterator last)
    : t(Compare()) { t.insert_equal(first, last); }

  template <class InputIterator>
  multimap(InputIterator first, InputIterator last, const Compare& comp)
    : t(comp) { t.insert_equal(first, last); }
#else
  multimap(const value_type* first, const value_type* last)
    : t(Compare()) { t.insert_equal(first, last); }
  multimap(const value_type* first, const value_type* last,
           const Compare& comp)
    : t(comp) { t.insert_equal(first, last); }

  multimap(const_iterator first, const_iterator last)
    : t(Compare()) { t.insert_equal(first, last); }
  multimap(const_iterator first, const_iterator last, const Compare& comp)
    : t(comp) { t.insert_equal(first, last); }
#endif /* __STL_MEMBER_TEMPLATES */
    ...
};
```

## 基本属性获取

```c++
class multimap {
	...
public:
  key_compare key_comp() const { return t.key_comp(); }
    // 返回的是一个仿函数
  value_compare value_comp() const { return value_compare(t.key_comp()); }
  iterator begin() { return t.begin(); }
  const_iterator begin() const { return t.begin(); }
  iterator end() { return t.end(); }
  const_iterator end() const { return t.end(); }
  reverse_iterator rbegin() { return t.rbegin(); }
  const_reverse_iterator rbegin() const { return t.rbegin(); }
  reverse_iterator rend() { return t.rend(); }
  const_reverse_iterator rend() const { return t.rend(); }
  bool empty() const { return t.empty(); }
  size_type size() const { return t.size(); }
  size_type max_size() const { return t.max_size(); }
    // 交换
  void swap(multimap<Key, T, Compare, Alloc>& x) { t.swap(x.t); }
    ...
};
```

## 重载

```c++
class multimap {
	...
public:
  multimap(const multimap<Key, T, Compare, Alloc>& x) : t(x.t) { }
  multimap<Key, T, Compare, Alloc>&
  operator=(const multimap<Key, T, Compare, Alloc>& x) {
    t = x.t;
    return *this; 
  }
  friend bool operator== __STL_NULL_TMPL_ARGS (const multimap&, const multimap&);
  friend bool operator< __STL_NULL_TMPL_ARGS (const multimap&, const multimap&);
    ...
};
template <class Key, class T, class Compare, class Alloc>
inline bool operator==(const multimap<Key, T, Compare, Alloc>& x, 
                       const multimap<Key, T, Compare, Alloc>& y) {
  return x.t == y.t;
}

template <class Key, class T, class Compare, class Alloc>
inline bool operator<(const multimap<Key, T, Compare, Alloc>& x, 
                      const multimap<Key, T, Compare, Alloc>& y) {
  return x.t < y.t;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class Key, class T, class Compare, class Alloc>
inline void swap(multimap<Key, T, Compare, Alloc>& x, 
                 multimap<Key, T, Compare, Alloc>& y) {
  x.swap(y);
}
```

## find, insert, erase

```c++
class multimap {
	...
public:
  // insert/erase
    // 其插入方式与map一样, 都是接受pair结构和迭代器
  iterator insert(const value_type& x) { return t.insert_equal(x); }
  iterator insert(iterator position, const value_type& x) {
    return t.insert_equal(position, x);
  }
#ifdef __STL_MEMBER_TEMPLATES  
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    t.insert_equal(first, last);
  }
#else
  void insert(const value_type* first, const value_type* last) {
    t.insert_equal(first, last);
  }
  void insert(const_iterator first, const_iterator last) {
    t.insert_equal(first, last);
  }
#endif /* __STL_MEMBER_TEMPLATES */
  void erase(iterator position) { t.erase(position); }
  size_type erase(const key_type& x) { return t.erase(x); }
  void erase(iterator first, iterator last) { t.erase(first, last); }
  void clear() { t.clear(); }
  // multimap operations:
  iterator find(const key_type& x) { return t.find(x); }
  const_iterator find(const key_type& x) const { return t.find(x); }
  size_type count(const key_type& x) const { return t.count(x); }
  iterator lower_bound(const key_type& x) {return t.lower_bound(x); }
  const_iterator lower_bound(const key_type& x) const {
    return t.lower_bound(x); 
  }
  iterator upper_bound(const key_type& x) {return t.upper_bound(x); }
  const_iterator upper_bound(const key_type& x) const {
    return t.upper_bound(x); 
  }
   pair<iterator,iterator> equal_range(const key_type& x) {
    return t.equal_range(x);
  }
  pair<const_iterator,const_iterator> equal_range(const key_type& x) const {
    return t.equal_range(x);
  }
};
```
