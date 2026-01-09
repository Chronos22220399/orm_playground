## 0/1 作为 DefaultValue 的 nttp 时被 bool 的分派吞噬

##### DefaultValue 的定义如下

```cpp
// 默认值
template <auto> struct DefaultValue {};

template <concepts::sql_default_value auto Value>
struct DefaultValue<Value> : detail::AttributeTag {
  using semantic_type = meta::sql_value_tag<Value>::type;
  static constexpr auto value = Value;
};
```

##### sql_value_tag 原本的写法如下:

```cpp
template <auto> struct sql_value_tag;

template <bool Value> struct sql_value_tag<Value> {
  using type = sql_boolean;
};

// enum 使用的值语义
template <auto Value>
  requires std::is_enum_v<decltype(Value)>
struct sql_value_tag<Value> {
  using type = sql_integer;
};

template <std::integral auto Value> struct sql_value_tag<Value> {
  using type = sql_integer;
};

template <std::floating_point auto Value> struct sql_value_tag<Value> {
  using type = sql_floating;
};

template <FixedString Value> struct sql_value_tag<Value> {
  using type = sql_text;
};

template <SqlNull Value> struct sql_value_tag<Value> {
  using type = sql_null;
};
```

> 这里的问题在于用 nttp 作为了区分方式，而 0/1 作为 nttp 传入时即使使用了 (int)0/1 做强制类型
> 转换，仍然会被做类型压缩（可能是因为这个过程中编译期发现没有显示的使用其类型，于是编译器进
> 行了优化，即默认断定为最小的类型（bool），即仍然为普通的 0/1，会导致可能被推断为 bool 类型

为了修复这个 bug,于是考虑设法得到其类型后再进行分派，于是这里添加了新的类型 `sql_value_from_type`

#### 修改后

```cpp
template <typename> struct sql_value_from_type {};

template <> struct sql_value_from_type<bool> {
  using type = sql_boolean;
};

// enum 使用的值语义
template <typename T>
  requires std::is_enum_v<T>
struct sql_value_from_type<T> {
  using type = sql_integer;
};

template <std::integral T> struct sql_value_from_type<T> {
  using type = sql_integer;
};

template <std::floating_point T> struct sql_value_from_type<T> {
  using type = sql_floating;
};

template <std::size_t N> struct sql_value_from_type<meta::FixedString<N>> {
  using type = sql_text;
};

template <> struct sql_value_from_type<SqlNull> {
  using type = sql_null;
};

template <auto Value> struct sql_value_tag {
  // 为了解决 0/1 作为 nttp 时无法分辨为 bool 还是 int 导致的歧义故引入
  // sql_value_from_type
  using type =
      typename sql_value_from_type<std::remove_cvref_t<decltype(Value)>>::type;
};
```
