# vector

## vector基本数据结构

```c++
template <class T, class Alloc = alloc>
class vector 
{
public:
    // 定义vector自身的嵌套型别
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    // 定义迭代器, 这里就只是一个普通的指针
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    ...
  protected:
    typedef simple_alloc<value_type, Alloc> data_allocator; // 设置其空间配置器
    iterator start;             // 使用空间的头
    iterator finish;            // 使用空间的尾
    iterator end_of_storage;    // 可用空间的尾
    ...
};
```

因为`vector` 需要表示用户的数据的起始地址, 结束地址, 还需要其真正的最大地址, 所以总共需要3个迭代器分别指向数据的头(start), 数据的尾(finish), 数组的尾(end_of_storage).

```c++
    iterator start;             // 使用空间的头
    iterator finish;            // 使用空间的尾
    iterator end_of_storage;    // 可用空间的尾
```

## 构造函数

`vector`有多个构造函数, 为了满足多种初始化.

***初始化满足要么都初始化成功, 要么一个都不初始化并释放掉抛出异常***

```c++
vector() : start(0), finish(0), end_of_storage(0) {}        // 默认构造函数
explicit vector(size_type n) { fill_initialize(n, T()); }   // 必须显示的调用这个构造函数, 接受一个值
vector(size_type n, const T& value) { fill_initialize(n, value); } // 接受一个大小和初始化值. int和long都执行相同的函数初始化
vector(int n, const T& value) { fill_initialize(n, value); }
vector(long n, const T& value) { fill_initialize(n, value); }
vector(const vector<T, Alloc>& x);  // 接受一个vector参数的构造函数
```

```c++
// 初始化vector的使用空间头和空间的尾
void fill_initialize(size_type n, const T& value) 
{
    start = allocate_and_fill(n, value);    // 初始化并初始化值
    finish = start + n;
    end_of_storage = finish;
}
```

```c++
// 调用默认的第二配置器分配内存, 分配失败就释放所分配的内存
iterator allocate_and_fill(size_type n, const T& x)
{
    iterator result = data_allocator::allocate(n);  // 申请n个元素的线性空间.
    __STL_TRY   // 对整个线性空间进行初始化, 如果有一个失败则删除全部空间并抛出异常.
    {
         uninitialized_fill_n(result, n, x);
         return result;
    }
    __STL_UNWIND(data_allocator::deallocate(result, n));
}
```

**构造函数，接受一个`vector`**

```c++
vector(const vector<T, Alloc>& x) 
{
    start = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
    finish = start + (x.end() - x.begin()); // 初始化头和尾迭代器位置
    end_of_storage = finish;
}
// 同样进行初始化
template <class ForwardIterator> 
iterator allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last) 
{
    iterator result = data_allocator::allocate(n);
    __STL_TRY 
    {
          uninitialized_copy(first, last, result);  // 这里采用的是uninitialized_copy, 进行复制. 
          return result;
    }
    __STL_UNWIND(data_allocator::deallocate(result, n));
}
```

**构造函数, 接受两个迭代器, 构造一个范围的数据.**

```c++
#ifdef __STL_MEMBER_TEMPLATES
template <class InputIterator>
vector(InputIterator first, InputIterator last) :
        start(0), finish(0), end_of_storage(0)
  {
    range_initialize(first, last, iterator_category(first));
  }
#else /* __STL_MEMBER_TEMPLATES */
vector(const_iterator first, const_iterator last) {
      size_type n = 0;
      distance(first, last, n);
      start = allocate_and_copy(n, first, last);
      finish = start + n;
      end_of_storage = finish;
    }
#endif /* __STL_MEMBER_TEMPLATES */
```

## 析构函数

析构函数就是直接调用`deallocate` 空间配置器, 从头释放到数据尾部, 最后将内存还给空间配置器.

`vector`因为是类, 所以我们并不需要手动的释放内存, 生命周期结束后就自动调用析构从而释放调用空间, 当然我们也可以直接调用析构函数释放内存.

```c++
void deallocate() 
{
    if (start) 
        data_allocator::deallocate(start, end_of_storage - start);
}
// 调用析构函数并释放内存
~vector() 
{ 
    destroy(start, finish);
    deallocate();
}
```

## 属性获取

位置参数的获取. 因为`end()`返回的是`finish`, 而`finish`是指向最后一个元素的后一个位置的指针, 所以使用end的时候尽量注意一点.

```c++
public:
    // 获取数据的开始以及结束位置的指针. 记住这里返回的是迭代器, 也就是vector迭代器就是该类型的指针.
    iterator begin() { return start; }
    iterator end() { return finish; }
    // 获取值
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); } 
    // 获取右值
    const_iterator begin() const { return start; }
    const_iterator end() const { return finish; }
    const_reference front() const { return *begin(); }
    const_reference back() const { return *(end() - 1); }
    // 获取基本数组信息
    size_type size() const { return size_type(end() - begin()); }   // 数组元素的个数
    size_type max_size() const { return size_type(-1) / sizeof(T); }    // 最大能存储的元素个数
    size_type capacity() const { return size_type(end_of_storage - begin()); }  // 数组的实际大小
    bool empty() const { return begin() == end(); }     //比较头尾指针
```

## vector实现

### push和pop操作

```c++
// 如果可用空间还有就调用对象的构造函数并使用空间的尾增加
// 没有空间就重新申请一个更大的空间, 然后进行插入
void push_back(const T& x) 
{
      // 如果还没有到填满整个数组, 就在数据尾部插入
      if (finish != end_of_storage) 
      {
            construct(finish, x);
            ++finish;
      }
    // 数组被填充满, 调用insert_aux必须重新寻找新的更大的连续空间, 再进行插入
      else
          insert_aux(end(), x);
}
```

`pop_back`从尾部进行删除

```c++
// 使用空间的尾自减并调用其析构函数. 但是并没有释放内存
void pop_back() 
{
     --finish;
     destroy(finish);
}
```

push和pop也保证 了`finish`始终都指向最后一个元素的后一个位置的地址.

### 删除元素

`erase`清除指定位置的元素, 其重载函数用于清除一个范围内的所有元素. 实际实现就是将删除元素后面所有元素往前移动, 对于vector来说这样的操作花费的时间还是很大的, 毕竟他是一个数组.

```c++
// 清除指定位置的元素. 实际就是将指定位置后面的所有元素向前移动, 最后析构掉最后一个元素
iterator erase(iterator position) 
{
    if (position + 1 != end())
        copy(position + 1, finish, position);
    --finish;
    destroy(finish);
    return position;
}
 
// 清除一个指定范围的元素, 同样将指定范围后面的所有元素向前移动, 最后析构掉整个范围的元素
// 清除的是左闭右开的区间 [ )
iterator erase(iterator first, iterator last) 
{
    iterator i = copy(last, finish, first);
    destroy(i, finish);
    finish = finish - (last - first);
    return first;
}
```

### 重载运算符

`vector`为实现给用户最直接最方便的用途, 重载了`[]`, `=`等运算符, 用户更加容易的能操作迭代器, 使用起来更像是直接操作数组一样.

`[]`返回的是元素的**引用, 即一个左值**, 毕竟可能会对元素值进行修改.

```c++
reference operator[](size_type n) { return *(begin() + n); }
const_reference operator[](size_type n) const { return *(begin() + n); }
```

针对右值和const类型选择不同的重载方法.

vector的`[]`重载很有意思, 是`begin() + n`实现, 也就是说**n可以为负数**.

```c++
// 验证为n可以为负数
int main()
{
  vector<int> a(10);
  iota(a.begin(), a.end(), 1);
  for_each(a.begin(), a.end(), [](int a){ cout << a <<  " ";}); // 1 2 3 4 5 6 7 8 9 10 
  auto i = a.begin() + 3;
  cout << i[-1] << " " << a.end()[-1];  // 3 10

  exit(0);
}
```

*end()[-1]可以这样的操作来获取最后一个元素, 当然这样的操作一般也不会这样干*.

### 容器的调整

`reserve`修改容器实际的大小

```c++
void reserve(size_type n) 
{
      // 修改的容器大小要大于原始数组大小才行
      if (capacity() < n) 
      {
          const size_type old_size = size();
          // 重新拷贝数据, 并将原来的空间释放掉
          iterator tmp = allocate_and_copy(n, start, finish);
          destroy(start, finish);
          deallocate();
          // 重新修改3个迭代器位置
          start = tmp;
          finish = tmp + old_size;
          end_of_storage = start + n;
      }
}
```

`resize`重新修改数组元素的容量. 这里是修改容纳元素的大小, 不是数组的大小.

```c++
void resize(size_type new_size) { resize(new_size, T()); }
void resize(size_type new_size, const T& x) 
{
    // 元素大小大于了要修改的大小, 则释放掉超过的元素
      if (new_size < size()) 
        erase(begin() + new_size, end());
    // 元素不够, 就从end开始到要求的大小为止都初始化x
      else
        insert(end(), new_size - size(), x);
}
```

### swap

vector实现`swap`就只是将3个迭代器进行交换即可, 并不用将整个数组进行交换.

```c++
void swap(vector<T, Alloc>& x) 
{
      __STD::swap(start, x.start);
      __STD::swap(finish, x.finish);
      __STD::swap(end_of_storage, x.end_of_storage);
}
```

### 插入实现

`insert`为了接受不同的参数和参数个数, 所以定义了多个重载函数

`insert(iterator position, const T& x)`

```c++
iterator insert(iterator position) { return insert(position, T()); }
iterator insert(iterator position, const T& x) ;

iterator insert(iterator position, const T& x) 
{
      size_type n = position - begin();
      // 如果数组还有备用空间, 并且插入的是finish位置, 直接插入即可, 最后调整finish就行了.
      if (finish != end_of_storage && position == end()) 
      {
        construct(finish, x);
        ++finish;
      }
      // 以上条件不成立, 调用另一个函数执行插入操作
      else
        insert_aux(position, x);
      return begin() + n;
}
```

```c++
void insert_aux(iterator position, const T& x);

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) 
{
  // 如果数组还有备用空间, 就直接移动元素, 再将元素插入过去, 最后调整finish就行了.
  if (finish != end_of_storage) 
  {
    // 调用构造, 并将最后一个元素复制过去, 调整finish
    construct(finish, *(finish - 1));
    ++finish;
    T x_copy = x;
    // 将插入元素位置的后面所有元素往后移动, 最后元素插入到位置上.
    copy_backward(position, finish - 2, finish - 1);
    *position = x_copy;
  }
  // 没有备用空间, 重新申请空间再将元素拷贝过去同时执行插入操作
  else {
    const size_type old_size = size();
    const size_type len = old_size != 0 ? 2 * old_size : 1; // 重新申请空间原始空间的两倍+1的空间

    iterator new_start = data_allocator::allocate(len);
    iterator new_finish = new_start;
    __STL_TRY {
      // 进行分段将原始元素拷贝新的空间中, 这样也就实现了插入操作
      new_finish = uninitialized_copy(start, position, new_start);
      construct(new_finish, x);
      ++new_finish;
      new_finish = uninitialized_copy(position, finish, new_finish);
    }

  #ifdef  __STL_USE_EXCEPTIONS 
    catch(...) {
      destroy(new_start, new_finish); 
      data_allocator::deallocate(new_start, len);
      throw;
    }
  #endif /* __STL_USE_EXCEPTIONS */
    // 释放掉原来的空间, 调整新的3个迭代器的位置
    destroy(begin(), end());
    deallocate();
    start = new_start;
    finish = new_finish;
    end_of_storage = new_start + len;
  }
}
```

`void insert (iterator pos, size_type n, const T& x)`

```c++
void insert (iterator pos, size_type n, const T& x);
void insert (iterator pos, int n, const T& x) 
{
      insert(pos, (size_type) n, x);
}
void insert (iterator pos, long n, const T& x) 
{
      insert(pos, (size_type) n, x);
}        
```

```c++
template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) 
{
  if (n != 0) 
    {
        // ******** 1 ***********
        // 备用空间足够大
      if (size_type(end_of_storage - finish) >= n) 
      {
          T x_copy = x;
            // 保存插入位置到end的距离
          const size_type elems_after = finish - position;
          iterator old_finish = finish;
            // ******* a **********
            // 从插入的位置到数据结束的距离大于了要插入的个数n
          if (elems_after > n) 
          {
                // 先构造出finish-n个大小的空间, 再移动finish - n个元素的数据
            uninitialized_copy(finish - n, finish, finish);
            finish += n;
                // 在将从插入位置后的n个元素移动
            copy_backward(position, old_finish - n, old_finish);
                // 元素从插入位置开始进行填充即可
            fill(position, position + n, x_copy);
          }
            // ********* b *********
            // 从插入的位置到end的距离小于了要插入的个数
          else 
            {
                // 先构造出n - elems_after个大小的空间, 再从finish位置初始化n - elems_after为x
            uninitialized_fill_n(finish, n - elems_after, x_copy);
            finish += n - elems_after;
                // 从插入位置开始到原来的finish位置结束全部复制到新的结束位置后面
            uninitialized_copy(position, old_finish, finish);
            finish += elems_after;
                // 从插入位置进行填充x
            fill(position, old_finish, x_copy);
          }
      }
        // ******* 2 ***********
        // 空间不足处理
      else 
      {
            // 重新申请一个当前两倍的空间或者当前大小+插入的空间, 选择两者最大的方案.
          const size_type old_size = size();        
          const size_type len = old_size + max(old_size, n);
          iterator new_start = data_allocator::allocate(len);
          iterator new_finish = new_start;
          __STL_TRY 
          {
                // 同样进行分段复制到新的数组中, 从而实现插入
            new_finish = uninitialized_copy(start, position, new_start);
            new_finish = uninitialized_fill_n(new_finish, n, x);
            new_finish = uninitialized_copy(position, finish, new_finish);
          }
        #ifdef  __STL_USE_EXCEPTIONS 
          catch(...) 
          {
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
          }
        #endif /* __STL_USE_EXCEPTIONS */
            // 将当前数组的元素进行析构, 最后释放空间
          destroy(start, finish);
          deallocate();
            // 修改3个迭代器
          start = new_start;
          finish = new_finish;
          end_of_storage = new_start + len;
      }
  }
}
```

`inser(iterator position, const_iterator first, const_iterator last)`

```c++
template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, const_iterator first, const_iterator last) {
// 插入不为空
  if (first != last) {
    size_type n = 0;
    // 计算插入的长度, 并保存在n中
    distance(first, last, n);
    // 如果是剩余的空间大于插入的长度.
    if (size_type(end_of_storage - finish) >= n) {
      // 保存插入点到end的距离
      const size_type elems_after = finish - position;
      iterator old_finish = finish;
        // 同样比较n与插入点到end的距离, 下面的过程与上面描述的基本一致.
        // 从插入的位置到end的距离大于要插入的个数n
      if (elems_after > n) {
        uninitialized_copy(finish - n, finish, finish);
        finish += n;
        copy_backward(position, old_finish - n, old_finish);
        copy(first, last, position);
      }
      else {
        uninitialized_copy(first + elems_after, last, finish);
        finish += n - elems_after;
        uninitialized_copy(position, old_finish, finish);
        finish += elems_after;
        copy(first, first + elems_after, position);
      }
    }
    else {
      const size_type old_size = size();
      const size_type len = old_size + max(old_size, n);
      iterator new_start = data_allocator::allocate(len);
      iterator new_finish = new_start;
      __STL_TRY {
        new_finish = uninitialized_copy(start, position, new_start);
        new_finish = uninitialized_copy(first, last, new_finish);
        new_finish = uninitialized_copy(position, finish, new_finish);
      }
    #ifdef __STL_USE_EXCEPTIONS
      catch(...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
      }
    #endif /* __STL_USE_EXCEPTIONS */
      destroy(start, finish);
      deallocate();
      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
  }
}
```
