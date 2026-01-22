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

---

## Clang 下 lambda 过早类型转换（在使用尾部返回类型声明时在分支被优化前做了类型转换）

#### 修改前

```cpp
template <typename T> T get_converted() const {
  return std::visit(
      [](auto &&val) -> T {
        using V = std::decay_t<decltype(val)>;
        // 原本的类型
        if constexpr (std::is_same_v<T, V>) {
          return val;
          // 可强制转换的数值类型
        } else if constexpr (std::is_arithmetic_v<T> &&
                              std::is_arithmetic_v<V>) {
          return static_cast<T>(val);
        } else if constexpr (std::is_same_v<bool, T> &&
                              std::is_arithmetic_v<V>) {
          return static_cast<bool>(val);
        } else {
          throw std::runtime_error(
              "Type missmatch: cannot convert database value");
        }
      },
      m_value);
  }
};
```

#### 报错信息如下

```cpp
/Users/wuming/code/orm_playground/include/ess/orm/row.hpp:39:22: error: no viable conversion from returned value of type
      'bool' to function return type 'std::string'
      |                             ^
/Users/wuming/code/orm_playground/src/schema.cpp:63:31: note: in instantiation
      'ess::orm::Row::Proxy::as<std::string>' requested here
   63 |     auto title = row["title"].as<std::string>();
      |                               ^
/opt/homebrew/Cellar/llvm/21.1.8/bin/../include/c++/v1/string:1001:13: note: c
      conversion from 'bool' to 'const string &' for 1st argument
   39 |               return static_cast<bool>(val);
      |                      ^~~~~~~~~~~~~~~~~~~~~~
/Users/wuming/code/orm_playground/src/schema.cpp:63:31: note: in instantiation
      'ess::orm::Row::Proxy::as<std::string>' requested here
   63 |     auto title = row["title"].as<std::string>();
      |                               ^
/opt/homebrew/Cellar/llvm/21.1.8/bin/../include/c++/v1/string:1001:13: note: c
      conversion from 'bool' to 'const string &' for 1st argument
 1001 |   constexpr basic_string(const basic_string& __str)
```

#### 错误原因

在 Clang 下，当 T 的类型为 string、V 被推断为 bool 时，即使 is_same_v<bool, T> 结果为 false,
但是由于 Clang 针对带有返回类型的 lambda（此时用 -> T 声明了，等价于 -> std::string）
无论表达式是否合法，都会去校验内部表达式的合法性，而因为直接的使用了`static_cast<bool>(val);`,
此时由于返回结果不依赖于需要检查的模版参数 T 或 V，则在 else if constexpr 的这段分支
被优化前 `return static_cast<bool>(val)` 一定会被执行，于是就出现了这个错误

```cpp
} else if constexpr (std::is_same_v<bool, T> &&
  std::is_arithmetic_v<V>) {
return static_cast<bool>(val);
```

#### 修改后

```cpp
template <typename T> T get_converted() const {
  return std::visit(
      [](auto &&val) -> T {
        using V = std::decay_t<decltype(val)>;
        // 原本的类型
        if constexpr (std::is_same_v<T, V>) {
          return val;
          // 可强制转换的数值类型
        } else if constexpr ((std::is_arithmetic_v<T> ||
                              std::is_same_v<bool, T>) &&
                              std::is_arithmetic_v<V>) {
          return static_cast<T>(val);
        } else {
          throw std::runtime_error(
              "Type missmatch: cannot convert database value");
        }
      },
      m_value);
}
```

修改后的代码将 bool 改为了 T，这样编译器就必须以来 constexpr 表达式的判断结果了，则在上述情况下，
不会存在 static_cast<bool>(val)[val 为 std::string]

> 其余更改则是逻辑上的一点优化

---

## 异常无法正常捕获

#### 问题出处

```cpp
Proxy operator[](std::string const &key) const {
  auto it = m_data.find(key);
  if (it == m_data.end()) {
    std::string err_msg = "column not found: " + key;
    throw std::out_of_range(err_msg);
  }
  return {it->second};
}

try {
  throw std::out_of_range("正常的");
} catch (std::exception &e) {
  fmt::println("能正常捕获的错误: {}", e.what());
}

Row row;

try {
  auto v = row["missing"];
} catch (const std::exception &e) {
  std::cout << "caught: " << e.what() << '\n';
}


```

错误如下：

```cpp
能正常捕获的错误: 正常的
libc++abi: terminating due to uncaught exception of type std::out_of_range: colu
mn not found: missing
```

```cpp
❯ lldb ./build/orm
(lldb) target create "./build/orm"
Current executable set to '/Users/wuming/code/orm_playground/build/orm' (arm64).
(lldb) run
Process 93737 launched: '/Users/wuming/code/orm_playground/build/orm' (arm64)
能正常捕获的错误: 正常的
libc++abi: terminating due to uncaught exception of type std::out_of_range: colu
mn not found: missing
Process 93737 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = signal SIGABRT
    frame #0: 0x000000019c666a60 libsystem_kernel.dylib`__pthread_kill + 8
libsystem_kernel.dylib`__pthread_kill:
->  0x19c666a60 <+8>:  b.lo   0x19c666a80    ; <+40>
    0x19c666a64 <+12>: pacibsp
    0x19c666a68 <+16>: stp    x29, x30, [sp, #-0x10]!
    0x19c666a6c <+20>: mov    x29, sp
(lldb) bt
* thread #1, queue = 'com.apple.main-thread', stop reason = signal SIGABRT
  * frame #0: 0x000000019c666a60 libsystem_kernel.dylib`__pthread_kill + 8
    frame #1: 0x000000019c69ec20 libsystem_pthread.dylib`pthread_kill + 288
    frame #2: 0x000000019c5aba30 libsystem_c.dylib`abort + 180
    frame #3: 0x0000000100623a74 libc++abi.1.dylib`__abort_message + 92
    frame #4: 0x00000001005fcb70 libc++abi.1.dylib`demangling_terminate_handler(
) + 244
    frame #5: 0x000000010061ddc0 libc++abi.1.dylib`std::__terminate(void (*)())
+ 12
    frame #6: 0x0000000100623138 libc++abi.1.dylib`__gxx_personality_v0 + 1488
    frame #7: 0x00000001a92e80a0 libunwind.dylib`unwind_phase2 + 596
    frame #8: 0x00000001a92e8240 libunwind.dylib`_Unwind_Resume + 252
    frame #9: 0x0000000100004ad4 orm`ess::orm::Row::operator[](this=0x000000016f
dfd9a0, key="missing") const at allocate.h:0:10 [opt]
    frame #10: 0x0000000100004884 orm`main at schema.cpp:64:14 [opt]
    frame #11: 0x000000019c3160e0 dyld`start + 2360
(lldb)
orm │ signal SIGABRT
```

经过测试，将 `std::out_of_range(err_msg)` 改为 `std::out_of_range("xxx")` [任意编译期确定的字符串常量] 就不会出现问题
