# RB-tree

`RB-tree`规则:

1. **每个节点的颜色是黑色或者红色**
2. **根节点必须是黑色的**
3. **每个叶节点(NULL)必须是黑色的**
4. **如果节点是红色的, 则子节点必须是黑色的**
5. **从节点到每个子孙节点的路径包括的黑色节点数目相同**

```c++
// 红黑定义
typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;	
const __rb_tree_color_type __rb_tree_black = true;
```

## RB-tree基本结构分析

基本结构与 `list` 相似, 将节点与数据分开定义, `__rb_tree_node_base`定义指针; `__rb_tree_node`继承前者, 增加了数据, 就是一个完整的节点.

### RB-tree基本结构

#### __rb_tree_node_base

```c++
struct __rb_tree_node_base
{
  typedef __rb_tree_color_type color_type;
  typedef __rb_tree_node_base* base_ptr;

  color_type color;   // 定义节点颜色
  base_ptr parent;    // 定义父节点
  base_ptr left;    // 定义左孩子
  base_ptr right;   // 定义右孩子

    // 查找最小节点
  static base_ptr minimum(base_ptr x)
  {
    while (x->left != 0) x = x->left;
    return x;
  }

    // 查找最大节点
  static base_ptr maximum(base_ptr x)
  {
    while (x->right != 0) x = x->right;
    return x;
  }
};
```

**__rb_tree_node** : 完整的节点

```c++
template <class Value>
struct __rb_tree_node : public __rb_tree_node_base  // 继承__rb_tree_node_base
{
  typedef __rb_tree_node<Value>* link_type;
  Value value_field;  // 定义节点数据
};
```

#### RB-tree迭代器

**__rb_tree_base_iterator** 迭代器基本结构

迭代器中`increment`和`decrement`函数是实现++与--的功能的核心.

```c++
struct __rb_tree_base_iterator
{
  typedef __rb_tree_node_base::base_ptr base_ptr;
  typedef bidirectional_iterator_tag iterator_category; // bidirectional_iterator_tag类型的迭代器
  typedef ptrdiff_t difference_type;
  base_ptr node;  // 指针节点

    // ++核心函数
    // 节点是从node节点出发, 一直寻找右节点的左孩子, 每次找到比上次大的元素
  void increment()
  {
      // 有右节点, 就往右节点走
    if (node->right != 0) {
      node = node->right;
        // 一直往左节点走, 直到走到头
      while (node->left != 0)
        node = node->left;
    }
      // 没有右节点, 就寻找父节点
    else {
      base_ptr y = node->parent;
        // 如果该节点是父节点的右孩子就继续往上找, 直到y节点不是父节点的右孩子
      while (node == y->right) {
        node = y;
        y = y->parent;
      }
      if (node->right != y)
        node = y;
    }
  }

    // --核心代码
    // 节点是从node节点出发, 一直寻找左节点的右孩子, 每次找到比上次小的元素
  void decrement()
  {
      // 只有根节点, 每次--都是根节点
    if (node->color == __rb_tree_red && node->parent->parent == node)
      node = node->right;
      // 有左节点
    else if (node->left != 0) {
        // 往左节点走
      base_ptr y = node->left;
        // 只要有右节点就一直往右节点走
      while (y->right != 0)
        y = y->right;
      node = y;
    }
      // 没有左节点
    else {
        // 寻找父节点
      base_ptr y = node->parent;
        // 如果当前节点是父节点的左孩子就继续寻找父节点直到不再是左孩子
      while (node == y->left) {
        node = y;
        y = y->parent;
      }
      node = y;
    }
  }
};
```

**__rb_tree_iterator** 迭代器

```c++
template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator  // 继承__rb_tree_base_iterator
{
  typedef Value value_type;
  typedef Ref reference;
  typedef Ptr pointer;
  typedef __rb_tree_iterator<Value, Value&, Value*>             iterator;
  typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
  typedef __rb_tree_iterator<Value, Ref, Ptr>                   self;
  typedef __rb_tree_node<Value>* link_type;

  // 构造函数
  __rb_tree_iterator() {}
  __rb_tree_iterator(link_type x) { node = x; } // 初始化node节点
  __rb_tree_iterator(const iterator& it) { node = it.node; }  // 初始化node节点

  // 重载指针
  reference operator*() const { return link_type(node)->value_field; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  // 重载++与--操作, 调用increment和decrement函数
  self& operator++() { increment(); return *this; }
  self operator++(int) {
    self tmp = *this;
    increment();
    return tmp;
  }
    
  self& operator--() { decrement(); return *this; }
  self operator--(int) {
    self tmp = *this;
    decrement();
    return tmp;
  }
};
```

traits编程

```c++
#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION
inline bidirectional_iterator_tag
iterator_category(const __rb_tree_base_iterator&) {
  return bidirectional_iterator_tag();
}

inline __rb_tree_base_iterator::difference_type*
distance_type(const __rb_tree_base_iterator&) {
  return (__rb_tree_base_iterator::difference_type*) 0;
}

template <class Value, class Ref, class Ptr>
inline Value* value_type(const __rb_tree_iterator<Value, Ref, Ptr>&) {
  return (Value*) 0;
}
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
```

重载

```c++
// ==与!= 比较两个tree的node是相同
inline bool operator==(const __rb_tree_base_iterator& x,
                       const __rb_tree_base_iterator& y) {
  return x.node == y.node;
}
inline bool operator!=(const __rb_tree_base_iterator& x,
                       const __rb_tree_base_iterator& y) {
  return x.node != y.node;
}
```

**红黑树的调整最重要的就是用左旋和右旋.**

##### <font color=#b20>左旋</font>

```c++
inline void 
__rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
  __rb_tree_node_base* y = x->right;	// y为x的右孩子
  x->right = y->left;	// x的右孩子为y的左孩子
    // 如果y的左节点不为空
    // 将y的左节点的父节点指向x
  if (y->left != 0)
    y->left->parent = x;	
  // y的父节点指向x的父节点
  y->parent = x->parent;

    // 如果x就是为根节点
  if (x == root)
    root = y;
    // 如果x为父节点的左孩子
    // x的父节点的左节点指向y
  else if (x == x->parent->left)
    x->parent->left = y;
    // 如果x为父节点的右孩子
    // x的父节点的右节点指向y
  else
    x->parent->right = y;
  // y的左孩子指向x
  // x的父节点指向y
  y->left = x;
  x->parent = y;
}
```

##### <font color=#b20>右旋</font>

```c++
inline void 
__rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
  __rb_tree_node_base* y = x->left;	// y为x的左节点
  x->left = y->right;	// x的左节点指向y的右节点
    // y有右孩子
    // y的的右孩子的父节点指向节点x
  if (y->right != 0)
    y->right->parent = x;
  // y的父节点指向x的父节点
  y->parent = x->parent;

    // 如果x为根节点
    // 将y改为根节点
  if (x == root)
    root = y;
    // x为父节点的右孩子
    // x的父节点的左节点指向y
  else if (x == x->parent->right)
    x->parent->right = y;
    // x的父节点的左节点指向y
  else
    x->parent->left = y;
  // y的右节点指向x
  // x的父节点指向y
  y->right = x;
  x->parent = y;
}
```

##### <font color=#b20>调整红黑树 : </font>

1.  (如果) x不是根节点同时x的父节点颜色为红色
    1.  (如果) x的父节点是x的祖节点的左孩子
        1.  有y为x父节点的兄弟
        2.  (如果) y节点存在并且颜色为红色
            1.  将父节点和兄弟节点颜色都改为黑色
            2.  祖节点改为红色
            3.  当前节点(x)为祖节点
        3.  (否则) y节点不存在或颜色为黑色
            1.  x是父节点的右孩子
            2.  当前节点为x的父节点
            3.  左旋
            4.  x父节点颜色改为黑色
            5.  兄弟节点颜色改为红色
            6.  以祖节点为节点右旋
    2.  (否则) x的父节点是x的祖节点的右孩子
        1.  有y为x父节点的兄弟
        2.  (如果) y存在并且颜色为红色
            1.  将父节点和兄弟节点颜色都改为黑色
            2.  祖节点改为红色
            3.  当前节点(x)为祖节点
        3.  (否则) y节点不存在或颜色为黑色
            1.  x是父节点的左孩子
            2.  右旋
            3.  x父节点颜色改为黑色
            4.  兄弟节点颜色改为红色
            5.  以祖节点为节点左旋
2.  (否则) 将根节点调整为黑色, 因为根节点可能会被修改

```c++
// 这里x指的当前节点, 因为后面会修改到x, 描述x就会出问题
inline void 
__rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
  x->color = __rb_tree_red;	// 当前节点颜色改为红色
    // x不是根节点同时x的父节点颜色为红色
  while (x != root && x->parent->color == __rb_tree_red) {
      /************ 1 **********/
      // x的父节点是x的祖节点的左孩子
    if (x->parent == x->parent->parent->left) {
        // 有y为x父节点的兄弟
      __rb_tree_node_base* y = x->parent->parent->right;
        /********* a ***********/
        // y节点存在并且颜色为红色
      if (y && y->color == __rb_tree_red) {
        // 将父节点和兄弟节点颜色都改为黑色
        // 祖节点改为红色
        // 当前节点(x)为祖节点
        x->parent->color = __rb_tree_black;
        y->color = __rb_tree_black;
        x->parent->parent->color = __rb_tree_red;
        x = x->parent->parent;
      }
        /********* b ***********/
        // y节点不存在或颜色为黑色
      else {
          // x是父节点的右孩子
        if (x == x->parent->right) {
            // 当前节点为x的父节点
            // 左旋
          x = x->parent;
          __rb_tree_rotate_left(x, root);
        }
          // x父节点颜色改为黑色
          // 兄弟节点颜色改为红色
          // 以祖节点为节点右旋
        x->parent->color = __rb_tree_black;
        x->parent->parent->color = __rb_tree_red;
        __rb_tree_rotate_right(x->parent->parent, root);
      }
    }
     /************ 2 **********/
    // x的父节点是x的祖节点的右孩子
    else {
        // 有y为x父节点的兄弟
      __rb_tree_node_base* y = x->parent->parent->left;
        /********* a ***********/
        // y存在并且颜色为红色
      if (y && y->color == __rb_tree_red) {
          // 将父节点和兄弟节点颜色都改为黑色
          // 祖节点改为红色
          // 当前节点(x)为祖节点
        x->parent->color = __rb_tree_black;
        y->color = __rb_tree_black;
        x->parent->parent->color = __rb_tree_red;
        x = x->parent->parent;
      }
        /********* b ***********/
        // y节点不存在或颜色为黑色
      else {
          // x是父节点的左孩子
        if (x == x->parent->left) {
            // 当前节点为x的父节点
            // 右旋
          x = x->parent;
          __rb_tree_rotate_right(x, root);
        }
          // x父节点颜色改为黑色
          // 兄弟节点颜色改为红色
          // 以祖节点为节点左旋
        x->parent->color = __rb_tree_black;
        x->parent->parent->color = __rb_tree_red;
        __rb_tree_rotate_left(x->parent->parent, root);
      }
    }
  }
    // 将根节点调整为黑色, 因为根节点可能会被修改
  root->color = __rb_tree_black;
}
```

##### <font color=#b20>删除节点</font>

```c++
// 这里x指的当前节点, 因为后面会修改到x, 描述x就会出问题
inline __rb_tree_node_base*
__rb_tree_rebalance_for_erase(__rb_tree_node_base* z,
                              __rb_tree_node_base*& root,
                              __rb_tree_node_base*& leftmost,
                              __rb_tree_node_base*& rightmost)
{
    // y保存z节点指针
  __rb_tree_node_base* y = z;
  __rb_tree_node_base* x = 0;
  __rb_tree_node_base* x_parent = 0;
    /******* 1 *********/
    // y不存在左节点
  if (y->left == 0)             // z has at most one non-null child. y == z.
      // 则当前节点修改为x的右孩子
    x = y->right;               // x might be null.
    //  y存在左节点
    /******* 2 *********/
  else
      /******* a *********/  
    // y不存在右节点
    if (y->right == 0)          // z has exactly one non-null child.  y == z.
        // 则当前节点修改为x的左孩子
      x = y->left;              // x is not null.
      /******* b *********/ 
    // y存在左右节点
    else {                      // z has two non-null children.  Set y to
       // y修改为y的右节点
       // 如果此时y的左节点存在, 就一直往左边走
       // 当前节点为y的右节点
      y = y->right;             //   z's successor.  x might be null.
      while (y->left != 0)
        y = y->left;
      x = y->right;
    }
  /******* 1 *********/ 
  // 以上的操作就是为了找到z的边的最先的那个节点为y
  // y节点被修改过
  if (y != z) {                 // relink y in place of z.  y is z's successor
      // z的左节点的父节点指向y
      // y的左节点指向x的左节点
    z->left->parent = y; 
    y->left = z->left;
      /******* a *********/
      // y不为z的右节点
    if (y != z->right) {
        // 保存y的父节点
      x_parent = y->parent;
        // y的左或右节点存在(x), 则x的父节点指向y的父节点
      if (x) x->parent = y->parent;
       	// y的父节点指向的左孩子指向x
        // y的右孩子指向z的右孩子
        // z的右孩子的父节点指向y
      y->parent->left = x;      // y must be a left child
      y->right = z->right;
      z->right->parent = y;
    }
      // y是z的右节点
    else
      x_parent = y;  
      
      // z是根节点
    if (root == z)
      root = y;
      // z是父节点的左孩子
    else if (z->parent->left == z)
      z->parent->left = y;	// z的父节点的左孩子指向y
      // z是父节点的右孩子
    else 
      z->parent->right = y;	// z的父节点的右孩子指向y
      // y的父节点指向z的父节点
      // 交换y和z的节点颜色
      // y修改为z
    y->parent = z->parent;
    __STD::swap(y->color, z->color);
    y = z;
    // y now points to node to be actually deleted
  }
    /******* 2 *********/
    // y没有被修改过
  else {                        // y == z
    x_parent = y->parent;
    if (x) x->parent = y->parent;   // x的父节点指向y的父节点
    if (root == z)
      root = x;
    else 
      if (z->parent->left == z)
        z->parent->left = x;
      else
        z->parent->right = x;
    if (leftmost == z) 
      if (z->right == 0)        // z->left must be null also
        leftmost = z->parent;
    // makes leftmost == header if z == root
      else
        leftmost = __rb_tree_node_base::minimum(x);
    if (rightmost == z)  
      if (z->left == 0)         // z->right must be null also
        rightmost = z->parent;  
    // makes rightmost == header if z == root
      else                      // x == z->left
        rightmost = __rb_tree_node_base::maximum(x);
  }
   /************ 1 **************/
    // y节点的颜色不为红色
  if (y->color != __rb_tree_red) { 
      // x不为根节点并且x为空或颜色为黑色
      // 下面的分析与上面一样, 这里就不在做详细的分析了, 只要分析的时候画好图就行了
    while (x != root && (x == 0 || x->color == __rb_tree_black))
      if (x == x_parent->left) {
        __rb_tree_node_base* w = x_parent->right;
        if (w->color == __rb_tree_red) {
          w->color = __rb_tree_black;
          x_parent->color = __rb_tree_red;
          __rb_tree_rotate_left(x_parent, root);
          w = x_parent->right;
        }
        if ((w->left == 0 || w->left->color == __rb_tree_black) &&
            (w->right == 0 || w->right->color == __rb_tree_black)) {
          w->color = __rb_tree_red;
          x = x_parent;
          x_parent = x_parent->parent;
        } else {
          if (w->right == 0 || w->right->color == __rb_tree_black) {
            if (w->left) w->left->color = __rb_tree_black;
            w->color = __rb_tree_red;
            __rb_tree_rotate_right(w, root);
            w = x_parent->right;
          }
          w->color = x_parent->color;
          x_parent->color = __rb_tree_black;
          if (w->right) w->right->color = __rb_tree_black;
          __rb_tree_rotate_left(x_parent, root);
          break;
        }
      } else {                  // same as above, with right <-> left.
        __rb_tree_node_base* w = x_parent->left;
        if (w->color == __rb_tree_red) {
          w->color = __rb_tree_black;
          x_parent->color = __rb_tree_red;
          __rb_tree_rotate_right(x_parent, root);
          w = x_parent->left;
        }
        if ((w->right == 0 || w->right->color == __rb_tree_black) &&
            (w->left == 0 || w->left->color == __rb_tree_black)) {
          w->color = __rb_tree_red;
          x = x_parent;
          x_parent = x_parent->parent;
        } else {
          if (w->left == 0 || w->left->color == __rb_tree_black) {
            if (w->right) w->right->color = __rb_tree_black;
            w->color = __rb_tree_red;
            __rb_tree_rotate_left(w, root);
            w = x_parent->left;
          }
          w->color = x_parent->color;
          x_parent->color = __rb_tree_black;
          if (w->left) w->left->color = __rb_tree_black;
          __rb_tree_rotate_right(x_parent, root);
          break;
        }
      }
    if (x) x->color = __rb_tree_black;
  }
  return y;
}
```

### RB-tree结构分析

#### rb-tree类型定义

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree {
protected:
  typedef void* void_pointer;
  typedef __rb_tree_node_base* base_ptr;	// 定义节点指针
  typedef __rb_tree_node<Value> rb_tree_node;	// 定义节点
  typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;	// 定义空间配置器
  typedef __rb_tree_color_type color_type;	
public:
	// 满足traits编程
  typedef Key key_type;
  typedef Value value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef rb_tree_node* link_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
protected:
  size_type node_count; // keeps track of size of tree
  link_type header;  	// 头节点, 不是根节点, 头节点是指向根节点
  Compare key_compare;	// 伪函数
public:
	// 定义迭代器
  typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
  typedef __rb_tree_iterator<value_type, const_reference, const_pointer> 
          const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
  typedef reverse_bidirectional_iterator<iterator, value_type, reference,
                                         difference_type>
          reverse_iterator; 
  typedef reverse_bidirectional_iterator<const_iterator, value_type,
                                         const_reference, difference_type>
          const_reverse_iterator;
  ...
};
```

##### 构造析构函数

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree {
	...
public:                          // allocation/deallocation
  rb_tree(const Compare& comp = Compare())
    : node_count(0), key_compare(comp) { init(); }

  rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) : node_count(0), key_compare(x.key_compare)
  { 
    header = get_node();	// 分配空间
    color(header) = __rb_tree_red;	// 默认节点的颜色设置为红色
    if (x.root() == 0) {	// 如果x为根节点
      root() = 0;
      leftmost() = header;	// 左孩子指向根节点
      rightmost() = header;	// 右孩子指向根节点
    }
    else {
      __STL_TRY {
        root() = __copy(x.root(), header);
      }
      __STL_UNWIND(put_node(header));
      leftmost() = minimum(root());	// 左孩子始终指向最小的节点
      rightmost() = maximum(root());	// 右孩子始终指向最大的节点
    }
    node_count = x.node_count;
  }
  
  // 析构函数
  ~rb_tree() {
    clear();	// 清除或有节点
    put_node(header);	// 释放所有空间
  }
  ...
};
```

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree {
	...
protected:
	// 分配空间
  link_type get_node() { return rb_tree_node_allocator::allocate(); }	
  	// 释放空间
  void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }
	// 调用构造函数
  link_type create_node(const value_type& x) {
    link_type tmp = get_node();
    __STL_TRY {
      construct(&tmp->value_field, x);
    }
    // 分配失败释放所有空间
    __STL_UNWIND(put_node(tmp));
    return tmp;
  }
	
	// 分配节点, 并初始化
  link_type clone_node(link_type x) {
    link_type tmp = create_node(x->value_field);
    tmp->color = x->color;
    tmp->left = 0;
    tmp->right = 0;
    return tmp;
  }

	// 释放节点
  void destroy_node(link_type p) {
    destroy(&p->value_field);
    put_node(p);
  }
private:
  void init() {
    header = get_node();
    color(header) = __rb_tree_red; // used to distinguish header from 
                                   // root, in iterator.operator++
    root() = 0;
    leftmost() = header;
    rightmost() = header;
  }
public:
  void clear() {
    if (node_count != 0) {
      __erase(root());
      leftmost() = header;
      root() = 0;
      rightmost() = header;
      node_count = 0;
    }
  } 
  ...
};
```

##### 节点属性

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree {
	...
protected:
  link_type& root() const { return (link_type&) header->parent; }		// 获取根节点
  link_type& leftmost() const { return (link_type&) header->left; }		// 最小节点
  link_type& rightmost() const { return (link_type&) header->right; }	// 最大节点

	// 当前节点的左节点
	// 当前节点的右节点
  static link_type& left(link_type x) { return (link_type&)(x->left); }
  static link_type& right(link_type x) { return (link_type&)(x->right); }
  static link_type& parent(link_type x) { return (link_type&)(x->parent); }
  static reference value(link_type x) { return x->value_field; }	// 当前节点的数据
  static const Key& key(link_type x) { return KeyOfValue()(value(x)); }
  static color_type& color(link_type x) { return (color_type&)(x->color); }

  static link_type& left(base_ptr x) { return (link_type&)(x->left); }
  static link_type& right(base_ptr x) { return (link_type&)(x->right); }
  static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
  static reference value(base_ptr x) { return ((link_type)x)->value_field; }
  static const Key& key(base_ptr x) { return KeyOfValue()(value(link_type(x)));} 
  static color_type& color(base_ptr x) { return (color_type&)(link_type(x)->color); }

	// 最小节点
  static link_type minimum(link_type x) { 
    return (link_type)  __rb_tree_node_base::minimum(x);
  }
  	// 最大节点
  static link_type maximum(link_type x) {
    return (link_type) __rb_tree_node_base::maximum(x);
  }
  
public:    
                                // accessors:
  Compare key_comp() const { return key_compare; }
  // begin() 获取的是最小节点
  iterator begin() { return leftmost(); }
  const_iterator begin() const { return leftmost(); }
  // end() 头节点
  iterator end() { return header; }
  const_iterator end() const { return header; }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { 
    return const_reverse_iterator(end()); 
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { 
    return const_reverse_iterator(begin());
  } 
  bool empty() const { return node_count == 0; }		// 树为空
  size_type size() const { return node_count; }			// 节点计数
  size_type max_size() const { return size_type(-1); }	
  ...
};
```

##### swap

rb-tree交换也并不是交换所有的节点, 只是交换了头节点, 节点数和比较伪函数.

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree {
	...
public:    
  void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& t) {
    __STD::swap(header, t.header);
    __STD::swap(node_count, t.node_count);
    __STD::swap(key_compare, t.key_compare);
  }
  ...
};
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, 
                 rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
  x.swap(y);
}
```

##### 重载

```c++
// 相等比较
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, 
                       const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, 
                      const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

// 重载=运算符
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
  if (this != &x) {
                                // Note that Key may be a constant type.
    clear();	// 清除所有节点
    node_count = 0;
    key_compare = x.key_compare;        
      // 将每个节点进行赋值
    if (x.root() == 0) {
      root() = 0;
      leftmost() = header;
      rightmost() = header;
    }
    else {
      root() = __copy(x.root(), header);
      leftmost() = minimum(root());
      rightmost() = maximum(root());
      node_count = x.node_count;
    }
  }
  return *this;
}
```

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree {
	...
private:
  iterator __insert(base_ptr x, base_ptr y, const value_type& v);
  link_type __copy(link_type x, link_type p);
  void __erase(link_type x);
public:
  rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& 
  operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);
    
public:
                                // insert/erase
  pair<iterator,bool> insert_unique(const value_type& x);
  iterator insert_equal(const value_type& x);

  iterator insert_unique(iterator position, const value_type& x);
  iterator insert_equal(iterator position, const value_type& x);

#ifdef __STL_MEMBER_TEMPLATES  
  template <class InputIterator>
  void insert_unique(InputIterator first, InputIterator last);
  template <class InputIterator>
  void insert_equal(InputIterator first, InputIterator last);
#else /* __STL_MEMBER_TEMPLATES */
  void insert_unique(const_iterator first, const_iterator last);
  void insert_unique(const value_type* first, const value_type* last);
  void insert_equal(const_iterator first, const_iterator last);
  void insert_equal(const value_type* first, const value_type* last);
#endif /* __STL_MEMBER_TEMPLATES */

  void erase(iterator position);
  size_type erase(const key_type& x);
  void erase(iterator first, iterator last);
  void erase(const key_type* first, const key_type* last);
     

public:
                                // set operations:
  iterator find(const key_type& x);
  const_iterator find(const key_type& x) const;
  size_type count(const key_type& x) const;
  iterator lower_bound(const key_type& x);
  const_iterator lower_bound(const key_type& x) const;
  iterator upper_bound(const key_type& x);
  const_iterator upper_bound(const key_type& x) const;
  pair<iterator,iterator> equal_range(const key_type& x);
  pair<const_iterator, const_iterator> equal_range(const key_type& x) const;

public:
                                // Debugging.
  bool __rb_verify() const;
};
```

##### <font color=#b20>插入</font>

```c++
// 插入
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
__insert(base_ptr x_, base_ptr y_, const Value& v) 
{
    // x为要插入的节点
    // y : 插入节点的父节点
    // v : 插入节点的数据
  link_type x = (link_type) x_;
  link_type y = (link_type) y_;
  link_type z;

    // 如果y是头节点, 将插入的节点设置为根节点
  if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
    z = create_node(v);	
      // 左节点指向z
    left(y) = z;                // also makes leftmost() = z when y == header
      // y是头节点, 根节点就是z
    if (y == header) {
      root() = z;
      rightmost() = z;
    }
      // 如果y是最小节点, 则把z修改为最小节点
    else if (y == leftmost())
      leftmost() = z;           // maintain leftmost() pointing to min node
  }
  else {
    z = create_node(v);
    right(y) = z;
    if (y == rightmost())
      rightmost() = z;          // maintain rightmost() pointing to max node
  }
  parent(z) = y;
  left(z) = 0;
  right(z) = 0;
  __rb_tree_rebalance(z, header->parent);
  ++node_count;	// 节点数++
  return iterator(z);	// 返回节点z迭代器
}

// 找到值应该插入的地方
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v)
{
  link_type y = header;
  link_type x = root();
  while (x != 0) {
    y = x;
    x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
  }
  return __insert(x, y, v);
}
```

**不能重复的插入**. 这里出现的`pair`结构下节分析, 这里只要知道这个它( ) true表示插入成功, false表示已经存在了.

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v)
{
  link_type y = header;
  link_type x = root();
  bool comp = true;
    // 找到合适的节点
  while (x != 0) {
    y = x;
    comp = key_compare(KeyOfValue()(v), key(x));
    x = comp ? left(x) : right(x);
  }
  iterator j = iterator(y);   
  if (comp)
    if (j == begin())     
      return pair<iterator,bool>(__insert(x, y, v), true);
    else
      --j;
    // 找到插入的节点是否已经存在了
  if (key_compare(key(j.node), KeyOfValue()(v)))
    return pair<iterator,bool>(__insert(x, y, v), true);
  return pair<iterator,bool>(j, false);
}

template <class Key, class Val, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator 
rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_unique(iterator position,const Val& v) 
{
  if (position.node == header->left) // begin()
    if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
      return __insert(position.node, position.node, v);
  // first argument just needs to be non-null 
    else
      return insert_unique(v).first;
  else if (position.node == header) // end()
    if (key_compare(key(rightmost()), KeyOfValue()(v)))
      return __insert(0, rightmost(), v);
    else
      return insert_unique(v).first;
  else {
    iterator before = position;
    --before;
    if (key_compare(key(before.node), KeyOfValue()(v))
        && key_compare(KeyOfValue()(v), key(position.node)))
      if (right(before.node) == 0)
        return __insert(0, before.node, v); 
      else
        return __insert(position.node, position.node, v);
    // first argument just needs to be non-null 
    else
      return insert_unique(v).first;
  }
}
```

```c++
template <class Key, class Val, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator 
rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_equal(iterator position,
                                                            const Val& v) {
  if (position.node == header->left) // begin()
    if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
      return __insert(position.node, position.node, v);
  // first argument just needs to be non-null 
    else
      return insert_equal(v);
  else if (position.node == header) // end()
    if (!key_compare(KeyOfValue()(v), key(rightmost())))
      return __insert(0, rightmost(), v);
    else
      return insert_equal(v);
  else {
    iterator before = position;
    --before;
    if (!key_compare(KeyOfValue()(v), key(before.node))
        && !key_compare(key(position.node), KeyOfValue()(v)))
      if (right(before.node) == 0)
        return __insert(0, before.node, v); 
      else
        return __insert(position.node, position.node, v);
    // first argument just needs to be non-null 
    else
      return insert_equal(v);
  }
}
```

这里只选择了两个, 其中还有几个重载函数没有列出

```c++
// 范围的插入(可以重复)
template <class K, class V, class KoV, class Cmp, class Al> template<class II>
void rb_tree<K, V, KoV, Cmp, Al>::insert_equal(II first, II last) {
  for ( ; first != last; ++first)
    insert_equal(*first);
}

// 范围插入(不可重复)
template <class K, class V, class KoV, class Cmp, class Al> template<class II>
void rb_tree<K, V, KoV, Cmp, Al>::insert_unique(II first, II last) {
  for ( ; first != last; ++first)
    insert_unique(*first);
}
```

##### <font color=#b20>删除</font>

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline void
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
  link_type y = (link_type) __rb_tree_rebalance_for_erase(position.node,
                                                          header->parent,
                                                          header->left,
                                                          header->right);
  destroy_node(y);	// 释放空间
  --node_count;		// 节点数--
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__erase(link_type x) {
                                // erase without rebalancing
   // 递归删除x指向的所有节点
  while (x != 0) {
    __erase(right(x));
    link_type y = left(x);
    destroy_node(x);
    x = y;
  }
}

// 范围删除节点
// 调用erase函数
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first, 
                                                            iterator last) {
  if (first == begin() && last == end())
    clear();
  else
    while (first != last) erase(first++);
}
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key* first, 
                                                            const Key* last) {
  while (first != last) erase(*first++);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key& x) {
  pair<iterator,iterator> p = equal_range(x);
  size_type n = 0;
	// 计算长度, erase进行删除
  distance(p.first, p.second, n);
  erase(p.first, p.second);
  return n;	// 返回删除的个数
}
```

##### 复制

```c++
template <class K, class V, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<K, V, KeyOfValue, Compare, Alloc>::link_type 
rb_tree<K, V, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p) {
                                // structural copy.  x and p must be non-null.
  // 复制x的节点为top
  // top的父节点指向p
  link_type top = clone_node(x);
  top->parent = p;
 
  __STL_TRY {
  	// x的右节点存在
    if (x->right)
    	// 递归复制右节点
      top->right = __copy(right(x), top);
    p = top;
    // x指向左节点
    x = left(x);

	// 只要x节点存在
    while (x != 0) {
    	// y为复制当前x的节点
    	// p的左孩子指向y, y的父节点指向p
      link_type y = clone_node(x);
      p->left = y;
      y->parent = p;
      // x的右节点存在
      if (x->right)
      	// 递归复制右节点
        y->right = __copy(right(x), y);
      p = y;
      x = left(x);
    }
  }
  // 一个分配失败就销毁所有的空间
  __STL_UNWIND(__erase(top));

  return top;
}
```

##### <font color=#b20>find</font>

```c++
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) {
  link_type y = header;        // Last node which is not less than k. 
  link_type x = root();        // Current node. 

	// 像二叉树一样通过节点比较
  while (x != 0) 
    if (!key_compare(key(x), k))
      y = x, x = left(x);
    else
      x = right(x);

  iterator j = iterator(y);   
  return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) const {
  link_type y = header; /* Last node which is not less than k. */
  link_type x = root(); /* Current node. */

  while (x != 0) {
    if (!key_compare(key(x), k))
      y = x, x = left(x);
    else
      x = right(x);
  }
  const_iterator j = const_iterator(y);   
  return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}
```

##### count

```c++
// 计算RB-tree中k出现的次数
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const Key& k) const {
  pair<const_iterator, const_iterator> p = equal_range(k);
  size_type n = 0;
  distance(p.first, p.second, n);
  return n;
}
```

```c++
// 计算红黑树有多少个黑节点
inline int __black_count(__rb_tree_node_base* node, __rb_tree_node_base* root)
{
  if (node == 0)
    return 0;
  else {
    int bc = node->color == __rb_tree_black ? 1 : 0;
    if (node == root)
      return bc;
    else
      return bc + __black_count(node->parent, root);
  }
}
```

```c++
// 检查是否符合rb-tree
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
bool 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_verify() const
{
    // 空树
  if (node_count == 0 || begin() == end())
    return node_count == 0 && begin() == end() &&
      header->left == header && header->right == header;
  
    // 最左节点到根节点的黑色节点数量为 len
  int len = __black_count(leftmost(), root());
    // 遍历每个节点
  for (const_iterator it = begin(); it != end(); ++it) {
    link_type x = (link_type) it.node;
    link_type L = left(x);
    link_type R = right(x);
	
      // 节点是红色如果子节点也是红色就不满足
    if (x->color == __rb_tree_red)
      if ((L && L->color == __rb_tree_red) ||
          (R && R->color == __rb_tree_red))
        return false;

    if (L && key_compare(key(x), key(L)))
      return false;
    if (R && key_compare(key(R), key(x)))
      return false;

    if (!L && !R && __black_count(x, root()) != len)
      return false;
  }

  if (leftmost() != __rb_tree_node_base::minimum(root()))
    return false;
  if (rightmost() != __rb_tree_node_base::maximum(root()))
    return false;

  return true;
}
```
