# hashtable

## hashtable分析

### 桶与节点

桶 : 定义的哈希表大小, 以vector为桶

节点 : 链表

```c++
// 这里链表是自定义的, 并没有采用list和slist
template <class Value>
struct __hashtable_node
{
  __hashtable_node* next;
  Value val;
}; 
```

```c++
// 前置声明
template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc = alloc>
class hashtable;

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator;

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator;
```

### hashtable迭代器

hashtable迭代器是`forward_iterator_tag`类型, 正向迭代器, 所以他也就没有重载`--` , 没有回退.

`__hashtable_const_iterator`与`__hashtable_iterator`一样, 这里就只分析后者

```c++
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator {
  typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>  hashtable;
  typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
          iterator;
  typedef __hashtable_const_iterator<Value, Key, HashFcn,  ExtractKey, EqualKey, Alloc>
          const_iterator;
  typedef __hashtable_node<Value> node;

  typedef forward_iterator_tag iterator_category;	// 正向迭代器
  typedef Value value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef Value& reference;
  typedef Value* pointer;

  node* cur;		// 定义节点
  hashtable* ht;	// 定义哈希表指针

  __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
  __hashtable_iterator() {}
  // 重载指针
  reference operator*() const { return cur->val; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */
	// 重在++, 因为是正向迭代器, 所以没有--
  iterator& operator++();
  iterator operator++(int);
  bool operator==(const iterator& it) const { return cur == it.cur; }
  bool operator!=(const iterator& it) const { return cur != it.cur; }
};
```

#### 定义哈希表大小

定义了哈希表的大小, 默认long为32位, 定义了28个数组大小. 哈希表的的大小都是素数, 减少冲突

```c++
// Note: assumes long is at least 32 bits.
static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
  53,         97,         193,       389,       769,
  1543,       3079,       6151,      12289,     24593,
  49157,      98317,      196613,    393241,    786433,
  1572869,    3145739,    6291469,   12582917,  25165843,
  50331653,   100663319,  201326611, 402653189, 805306457, 
  1610612741, 3221225473, 4294967291
};
// 找到大于n最近的素数
inline unsigned long __stl_next_prime(unsigned long n)
{
  const unsigned long* first = __stl_prime_list;
  const unsigned long* last = __stl_prime_list + __stl_num_primes;
  const unsigned long* pos = lower_bound(first, last, n);
  return pos == last ? *(last - 1) : *pos;
}
```

## 哈希表

### hashtable类型定义

模板参数含义 :

1.  Value : 节点的实值类型
2.  Key : 节点的键值类型
3.  HashFcn : hash function的类型
4.  ExtractKey : 从节点中取出键值的方法(函数或仿函数)
5.  EqualKey : 判断键值是否相同的方法(函数或仿函数)

```c++
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
public:
  typedef Key key_type;
  typedef Value value_type;
  typedef HashFcn hasher;
  typedef EqualKey key_equal;

  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;

	// 这里返回的都是仿函数
  hasher hash_funct() const { return hash; }
  key_equal key_eq() const { return equals; }

private:
	// 这里定义的都是函数或者仿函数
  hasher hash;
  key_equal equals;
  ExtractKey get_key;

  typedef __hashtable_node<Value> node;
  typedef simple_alloc<node, Alloc> node_allocator;

  vector<node*,Alloc> buckets;	// 以vector作为桶, node*
  size_type num_elements;		// 哈希表中元素个数的计数

public:
  typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;

  typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;

// 迭代器定义为友元
  friend struct
  __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
  friend struct
  __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
  ...
};
```

### 构造与析构函数

```c++
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
	...
public:
	// 构造函数, 没有定义默认构造函数
  hashtable(size_type n, const HashFcn&  hf,const EqualKey&   eql,const ExtractKey& ext)
    : hash(hf), equals(eql), get_key(ext), num_elements(0)
  {
    initialize_buckets(n);
  }

  hashtable(size_type n, const HashFcn&  hf, const EqualKey&   eql)
    : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
  {
    initialize_buckets(n);
  }
	// 拷贝构造函数
  hashtable(const hashtable& ht)
    : hash(ht.hash), equals(ht.equals), get_key(ht.get_key), num_elements(0)
  {
    copy_from(ht);
  }
  // 析构函数
  ~hashtable() { clear(); }
  ...
};
```

### 基本属性获取

```c++
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
	...
public:
  size_type size() const { return num_elements; }
  size_type max_size() const { return size_type(-1); }
  bool empty() const { return size() == 0; }

	// 交换, 并不是交换所有数据, 只是交换了其指针指向和个数
  void swap(hashtable& ht)
  {
    __STD::swap(hash, ht.hash);
    __STD::swap(equals, ht.equals);
    __STD::swap(get_key, ht.get_key);
    buckets.swap(ht.buckets);
    __STD::swap(num_elements, ht.num_elements);
  }

  iterator begin()
  { 
    for (size_type n = 0; n < buckets.size(); ++n)
    	// 从头遍历桶, 如果有不空的链表存在, 就返回该链表的第一个元素
      if (buckets[n])
        return iterator(buckets[n], this);
    // 没有元素就返回end.
    return end();
  }
	// end返回0
  iterator end() { return iterator(0, this); }

  const_iterator begin() const
  {
    for (size_type n = 0; n < buckets.size(); ++n)
      if (buckets[n])
        return const_iterator(buckets[n], this);
    return end();
  }
  const_iterator end() const { return const_iterator(0, this); }
  
  // 返回桶的大小
  size_type bucket_count() const { return buckets.size(); }

  size_type max_bucket_count() const
    { return __stl_prime_list[__stl_num_primes - 1]; } 

 // 返回指定位置的节点的个数
  size_type elems_in_bucket(size_type bucket) const
  {
    size_type result = 0;
    for (node* cur = buckets[bucket]; cur; cur = cur->next)
      result += 1;
    return result;
  }
  ...
};
```

### 重载

```c++
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
	...
public:
  hashtable& operator= (const hashtable& ht)
  {
    if (&ht != this) {
      clear();	// 清除原表中的数据
      // 重新进行赋值
      hash = ht.hash;
      equals = ht.equals;
      get_key = ht.get_key;
      copy_from(ht);
    }
    return *this;
  }
  friend bool
  operator== __STL_NULL_TMPL_ARGS (const hashtable&, const hashtable&);
  ...
};

// 重载++
template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>&
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++()
{
  const node* old = cur;
  cur = cur->next;
    // cur指向了NULL
  if (!cur) {
    size_type bucket = ht->bkt_num(old->val);
      // 寻找桶中下一个链表不为空的链表的第一个元素
    while (!cur && ++bucket < ht->buckets.size())
      cur = ht->buckets[bucket];
  }
  return *this;
}

template <class V, class K, class HF, class ExK, class EqK, class A>
inline __hashtable_iterator<V, K, HF, ExK, EqK, A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int)
{
  iterator tmp = *this;
  ++*this;
  return tmp;
}

template <class V, class K, class HF, class Ex, class Eq, class A>
bool operator==(const hashtable<V, K, HF, Ex, Eq, A>& ht1,
                const hashtable<V, K, HF, Ex, Eq, A>& ht2)
{
  typedef typename hashtable<V, K, HF, Ex, Eq, A>::node node;
    // 先判断桶的大小
  if (ht1.buckets.size() != ht2.buckets.size())
    return false;
    // 其次比较桶中每个指向的链表
  for (int n = 0; n < ht1.buckets.size(); ++n) {
    node* cur1 = ht1.buckets[n];
    node* cur2 = ht2.buckets[n];
      // 比较链表中的元素也是否相等
    for ( ; cur1 && cur2 && cur1->val == cur2->val;
          cur1 = cur1->next, cur2 = cur2->next)
      {}
      // 有一个链表还有剩余的元素就表示不相等
    if (cur1 || cur2)
      return false;
  }
  return true;
}  
```

### 插入

hashtable也有`insert_equal`和`insert_unqiue`两种插入方式, 而不可重复插入返回的是pair结构, 可重复插入返回的是迭代器.

同时兼顾了`ForwardIterator`和`InputIterator`的类型迭代器

```c++
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
	...
public: 
	// 不可重复插入
  pair<iterator, bool> insert_unique(const value_type& obj)
  {
    resize(num_elements + 1);
    return insert_unique_noresize(obj);
  }
	// 可重复插入
  iterator insert_equal(const value_type& obj)
  {
    resize(num_elements + 1);
    return insert_equal_noresize(obj);
  }
	// 不可重复插入返回的是pair结构
  pair<iterator, bool> insert_unique_noresize(const value_type& obj);
  	// 可重复插入返回的是迭代器
  iterator insert_equal_noresize(const value_type& obj);
 
 // 以下是insert的各个重载函数
#ifdef __STL_MEMBER_TEMPLATES
	// 针对InputIterator的迭代器
  template <class InputIterator>
  void insert_unique(InputIterator f, InputIterator l)
  {
    insert_unique(f, l, iterator_category(f));
  }
  template <class InputIterator>
  void insert_equal(InputIterator f, InputIterator l)
  {
    insert_equal(f, l, iterator_category(f));
  }
  template <class InputIterator>
  void insert_unique(InputIterator f, InputIterator l,input_iterator_tag)
  {
    for ( ; f != l; ++f)
      insert_unique(*f);
  }
  template <class InputIterator>
  void insert_equal(InputIterator f, InputIterator l,input_iterator_tag)
  {
    for ( ; f != l; ++f)
      insert_equal(*f);
  }
  // 针对ForwardIterator类型的迭代器, 一个个进行插入
  template <class ForwardIterator>
  void insert_unique(ForwardIterator f, ForwardIterator l,forward_iterator_tag)
  {
    size_type n = 0;
    distance(f, l, n);
    resize(num_elements + n);
    for ( ; n > 0; --n, ++f)
      insert_unique_noresize(*f);
  }
  template <class ForwardIterator>
  void insert_equal(ForwardIterator f, ForwardIterator l,forward_iterator_tag)
  {
    size_type n = 0;
    distance(f, l, n);
    resize(num_elements + n);
    for ( ; n > 0; --n, ++f)
      insert_equal_noresize(*f);
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert_unique(const value_type* f, const value_type* l)
  {
    size_type n = l - f;
    resize(num_elements + n);
    for ( ; n > 0; --n, ++f)
      insert_unique_noresize(*f);
  }
  void insert_equal(const value_type* f, const value_type* l)
  {
    size_type n = l - f;
    resize(num_elements + n);
    for ( ; n > 0; --n, ++f)
      insert_equal_noresize(*f);
  }
  void insert_unique(const_iterator f, const_iterator l)
  {
    size_type n = 0;
    distance(f, l, n);
    resize(num_elements + n);
    for ( ; n > 0; --n, ++f)
      insert_unique_noresize(*f);
  }
  void insert_equal(const_iterator f, const_iterator l)
  {
    size_type n = 0;
    distance(f, l, n);
    resize(num_elements + n);
    for ( ; n > 0; --n, ++f)
      insert_equal_noresize(*f);
  }
#endif /*__STL_MEMBER_TEMPLATES */
	...
};
```

`insert_unique_noresize`不可重复插入

```c++
template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool> 
hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj)
{
    // 确定插入的桶的具体位置
  const size_type n = bkt_num(obj);
  node* first = buckets[n];

    // 将元素插入到链表中
  for (node* cur = first; cur; cur = cur->next) 
      // 判断该元素在链表中是否已经存在了
    if (equals(get_key(cur->val), get_key(obj)))
      return pair<iterator, bool>(iterator(cur, this), false);	// 存在pair第二个参数返回false

    // 元素不存在链表中, 将它插入到链表的头部
  node* tmp = new_node(obj);
  tmp->next = first;
  buckets[n] = tmp;
  ++num_elements;	// 计数++
  return pair<iterator, bool>(iterator(tmp, this), true);	// 返回pair结构
}
```

`insert_equal_noresize`可重复插入

```c++
template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::iterator 
hashtable<V, K, HF, Ex, Eq, A>::insert_equal_noresize(const value_type& obj)
{
    // 确定插入的桶的具体位置
  const size_type n = bkt_num(obj);
  node* first = buckets[n];
	
    // 将元素插入到链表中
  for (node* cur = first; cur; cur = cur->next) 
      // 判断该元素在链表中是否已经存在了, 则将元素插入到重复数据的位置
    if (equals(get_key(cur->val), get_key(obj))) {
      node* tmp = new_node(obj);
      tmp->next = cur->next;
      cur->next = tmp;
      ++num_elements;
      return iterator(tmp, this);
    }

    // 元素不存在链表中, 将它插入到链表的头部
  node* tmp = new_node(obj);
  tmp->next = first;
  buckets[n] = tmp;	
  ++num_elements;	// 计数++
  return iterator(tmp, this);	// 返回pair结构
}
```

**查找**

hashtable的查找分为`find`和`find_or_insert`两种, 前者只是查找, 后者则有和插入的功能

```c++
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
	...
public: 
	// 找到指定的桶的位置再在链表中进行遍历
  iterator find(const key_type& key) 
  {
    size_type n = bkt_num_key(key);
    node* first;
    // 找到指定的位置并返回
    for ( first = buckets[n];first && !equals(get_key(first->val), key);  first = first->next)
      {}
    return iterator(first, this);
  } 
  ...
};
```

`find_or_insert`如果找到了就返回该元素的数据, 没有找到指定元素就将其插入到链表的头部.

```c++
template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::reference 
hashtable<V, K, HF, Ex, Eq, A>::find_or_insert(const value_type& obj)
{
  resize(num_elements + 1);

  size_type n = bkt_num(obj);
  node* first = buckets[n];
	// 如果找到了就返回该元素的数据
  for (node* cur = first; cur; cur = cur->next)
    if (equals(get_key(cur->val), get_key(obj)))
      return cur->val;

    // 没有找到指定元素就将其插入到链表的头部
  node* tmp = new_node(obj);
  tmp->next = first;
  buckets[n] = tmp;
  ++num_elements;
  return tmp->val;
}
```

**删除**

erase有很多个重载函数, 这里就具体分析一个就行了.

```c++
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
	...
public: 
  size_type erase(const key_type& key);
  void erase(const iterator& it);
  void erase(iterator first, iterator last);

  void erase(const const_iterator& it);
  void erase(const_iterator first, const_iterator last);

  void resize(size_type num_elements_hint);
  void clear();
  ...
};
```

```c++
template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable<V, K, HF, Ex, Eq, A>::size_type 
hashtable<V, K, HF, Ex, Eq, A>::erase(const key_type& key)
{
  const size_type n = bkt_num_key(key);
  node* first = buckets[n];
  size_type erased = 0;
	
    // 找到key具体在哪一个链表
  if (first) {
    node* cur = first;
    node* next = cur->next;
      // 遍历链表
    while (next) {
        // 元素在中间
      if (equals(get_key(next->val), key)) {
        cur->next = next->next;
        delete_node(next);
        next = cur->next;
        ++erased;
        --num_elements;
      }
        // 在头部
      else {
        cur = next;
        next = cur->next;
      }
    }
      // 析构, 释放
    if (equals(get_key(first->val), key)) {
      buckets[n] = first->next;
      delete_node(first);
      ++erased;
      --num_elements;
    }
  }
  return erased;
}
```

范围的删除

```c++
template <class V, class K, class HF, class Ex, class Eq, class A>
void 
hashtable<V, K, HF, Ex, Eq, A>::erase_bucket(const size_type n, node* last)
{
  node* cur = buckets[n];
  while (cur != last) {
    node* next = cur->next;
    delete_node(cur);
    cur = next;
    buckets[n] = cur;
    --num_elements;
  }
}

template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::clear()
{
  for (size_type i = 0; i < buckets.size(); ++i) {
    node* cur = buckets[i];
    while (cur != 0) {
      node* next = cur->next;
      delete_node(cur);
      cur = next;
    }
    buckets[i] = 0;
  }
  num_elements = 0;
}

```

**复制**

```c++
template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable& ht)
{
  buckets.clear(); //把元素全部删除
  buckets.reserve(ht.buckets.size());	// 重新调整桶的大小
    // 重新插入
  buckets.insert(buckets.end(), ht.buckets.size(), (node*) 0);
  __STL_TRY {
      // 插入
    for (size_type i = 0; i < ht.buckets.size(); ++i) {
      if (const node* cur = ht.buckets[i]) {
        node* copy = new_node(cur->val);
        buckets[i] = copy;

        for (node* next = cur->next; next; cur = next, next = cur->next) {
          copy->next = new_node(next->val);
          copy = copy->next;
        }
      }
    }
    num_elements = ht.num_elements;
  }
  __STL_UNWIND(clear());
}
```
