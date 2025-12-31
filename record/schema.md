### member_pointer_traits

```C++
// 通过 auto T 捕获其余路径
template <auto> inline constexpr bool dependent_false_v = false;

template <auto T> struct MemberPointerTraits {
  static_assert(dependent_false_v<T>, "unsupported member pointer type");
};

// 捕获成员指针
template <typename Class, typename Member, Member Class::*Ptr>
struct MemberPointerTraits<Ptr> {
  using class_type = Class;
  using member_type = Member;

  static constexpr Member Class::*pointer = Ptr;
  static constexpr bool is_static = false;

  static Member &get(Class &obj) { return obj.*pointer; }
};

// 捕获静态成员指针
template <typename Member, Member *Ptr> struct MemberPointerTraits<Ptr> {
  using member_type = Member;

  static constexpr Member *pointer = Ptr;
  static constexpr bool is_static = true;

  static Member &get() { return *pointer; }
};
```

### attribute

### field

### schema
