# hash_map

**类型定义**

```c++
#ifndef __STL_LIMITED_DEFAULT_TEMPLATES
template <class Key, class T, class HashFcn = hash<Key>,
          class EqualKey = equal_to<Key>,
          class Alloc = alloc>
#else
template <class Key, class T, class HashFcn, class EqualKey, 
          class Alloc = alloc>
#endif
class hash_map
{
private:
    // 定义hashtable
  typedef hashtable<pair<const Key, T>, Key, HashFcn,
                    select1st<pair<const Key, T> >, EqualKey, Alloc> ht;
  ht rep;

public:
    // 类型定义
  typedef typename ht::key_type key_type;
  typedef T data_type;
  typedef T mapped_type;
  typedef typename ht::value_type value_type;
  typedef typename ht::hasher hasher;
  typedef typename ht::key_equal key_equal;

  typedef typename ht::size_type size_type;
  typedef typename ht::difference_type difference_type;
  typedef typename ht::pointer pointer;
  typedef typename ht::const_pointer const_pointer;
  typedef typename ht::reference reference;
  typedef typename ht::const_reference const_reference;

  typedef typename ht::iterator iterator;
  typedef typename ht::const_iterator const_iterator;
	// 返回值是仿函数
  hasher hash_funct() const { return rep.hash_funct(); }
  key_equal key_eq() const { return rep.key_eq(); }
    ...
};
```

**构造函数**

```c++
class hash_map
{
	...
public:
    // 构造函数
  hash_map() : rep(100, hasher(), key_equal()) {}	// 默认构造函数
  explicit hash_map(size_type n) : rep(n, hasher(), key_equal()) {}
  hash_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
  hash_map(size_type n, const hasher& hf, const key_equal& eql)
    : rep(n, hf, eql) {}
	// 接受迭代器
  template <class InputIterator>
  hash_map(InputIterator f, InputIterator l)
    : rep(100, hasher(), key_equal()) { rep.insert_unique(f, l); }
  template <class InputIterator>
  hash_map(InputIterator f, InputIterator l, size_type n)
    : rep(n, hasher(), key_equal()) { rep.insert_unique(f, l); }
  template <class InputIterator>
  hash_map(InputIterator f, InputIterator l, size_type n,
           const hasher& hf)
    : rep(n, hf, key_equal()) { rep.insert_unique(f, l); }
  template <class InputIterator>
  hash_map(InputIterator f, InputIterator l, size_type n,
           const hasher& hf, const key_equal& eql)
    : rep(n, hf, eql) { rep.insert_unique(f, l); }
    ...
};
```

```c++
class hash_map
{
	...
public:
  size_type size() const { return rep.size(); }	// map中的元素个数
  size_type max_size() const { return rep.max_size(); }
  bool empty() const { return rep.empty(); }
  void swap(hash_map& hs) { rep.swap(hs.rep); }

    // 获取迭代器
  iterator begin() { return rep.begin(); }
  iterator end() { return rep.end(); }
  const_iterator begin() const { return rep.begin(); }
  const_iterator end() const { return rep.end(); }
  iterator find(const key_type& key) { return rep.find(key); }
  const_iterator find(const key_type& key) const { return rep.find(key); }
  size_type count(const key_type& key) const { return rep.count(key); }	// 获取一个元素出现的个数, map中count=0可以表示不存在
    ...
};
```

**重载**

```c++
class hash_map
{
	...
public:
  friend bool operator== __STL_NULL_TMPL_ARGS (const hash_map&, const hash_map&);
    
  T& operator[](const key_type& key) {
    return rep.find_or_insert(value_type(key, T())).second;
  }
    ...
};
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
inline bool operator==(const hash_map<Key, T, HashFcn, EqualKey, Alloc>& hm1,
                       const hash_map<Key, T, HashFcn, EqualKey, Alloc>& hm2)
{
  return hm1.rep == hm2.rep;
}
```

**insert等函数**

`insert_unique`不重复的插入元素

```c++
class hash_map
{
	...
public:
  // 不重复的插入元素
  pair<iterator, bool> insert(const value_type& obj)
    { return rep.insert_unique(obj); }
#ifdef __STL_MEMBER_TEMPLATES
    // 接受迭代器
  template <class InputIterator>
  void insert(InputIterator f, InputIterator l) { rep.insert_unique(f,l); }
#else
  void insert(const value_type* f, const value_type* l) {
    rep.insert_unique(f,l);
  }
  void insert(const_iterator f, const_iterator l) { rep.insert_unique(f, l); }
#endif /*__STL_MEMBER_TEMPLATES */
  pair<iterator, bool> insert_noresize(const value_type& obj)
    { return rep.insert_unique_noresize(obj); }    
  
  pair<iterator, iterator> equal_range(const key_type& key)
    { return rep.equal_range(key); }
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    { return rep.equal_range(key); }

    // 删除元素
  size_type erase(const key_type& key) {return rep.erase(key); }
  void erase(iterator it) { rep.erase(it); }
  void erase(iterator f, iterator l) { rep.erase(f, l); }
  void clear() { rep.clear(); }

public:
  void resize(size_type hint) { rep.resize(hint); }
  size_type bucket_count() const { return rep.bucket_count(); }
  size_type max_bucket_count() const { return rep.max_bucket_count(); }
  size_type elems_in_bucket(size_type n) const
    { return rep.elems_in_bucket(n); }
};
```
