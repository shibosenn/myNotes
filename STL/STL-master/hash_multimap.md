# hash_multimap

## insert

最大的区别就在这里 `hash_multimap` 是以`insert_equal`为接口, 所以支持插入重复的键.

```c++
#ifndef __STL_LIMITED_DEFAULT_TEMPLATES
template <class Key, class T, class HashFcn = hash<Key>,
          class EqualKey = equal_to<Key>,
          class Alloc = alloc>
#else
template <class Key, class T, class HashFcn, class EqualKey,
          class Alloc = alloc>
#endif
class hash_multimap
{
public:
	...
  iterator insert(const value_type& obj) { return rep.insert_equal(obj); }
#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  void insert(InputIterator f, InputIterator l) { rep.insert_equal(f,l); }
#else
  void insert(const value_type* f, const value_type* l) {
    rep.insert_equal(f,l);
  }
  void insert(const_iterator f, const_iterator l) { rep.insert_equal(f, l); }
#endif /*__STL_MEMBER_TEMPLATES */
  iterator insert_noresize(const value_type& obj)
    { return rep.insert_equal_noresize(obj); }    
    ...
};
```
