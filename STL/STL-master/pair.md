# pair

`pair`是一个有两个变量的结构体, 即谁都可以直接调用它的变量, 毕竟struct默认权限都是public, 将两个变量用`pair`绑定在一起, 这就为map<T1, T2>提供的存储的基础.

## pair结构定义

```c++
template <class T1, class T2>	// 两个参数类型
struct pair {
  typedef T1 first_type;
  typedef T2 second_type;

    // 定义的两个变量
  T1 first;	
  T2 second;
    
    // 构造函数
  pair() : first(T1()), second(T2()) {}
  pair(const T1& a, const T2& b) : first(a), second(b) {}
#ifdef __STL_MEMBER_TEMPLATES
  template <class U1, class U2>
  pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}
#endif
};
```

## 重载

因为只有两个变量, 运算符重载也很简单

```c++
template <class T1, class T2>
inline bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) { 
  return x.first == y.first && x.second == y.second; 
}
template <class T1, class T2>
inline bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y) { 
  return x.first < y.first || (!(y.first < x.first) && x.second < y.second); 
}
```

## make_pair

根据两个数值构造一个pair.

```c++
template <class T1, class T2>
inline pair<T1, T2> make_pair(const T1& x, const T2& y) {
  return pair<T1, T2>(x, y);
}
```
