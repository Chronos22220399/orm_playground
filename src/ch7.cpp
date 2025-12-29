#include <core.hpp>

template <typename T, std::size_t L1, std::size_t L2>
auto bars(T (&arg1)[L1], T (&arg2)[L2]) {}

// 同下
// template <typename T>
// using EnableIfString =
//     std::enable_if_t<std::is_constructible<std::string, T>::value>;
template <typename T>
using EnableIfString =
    std::enable_if_t<std::is_convertible<T, std::string>::value>;

class Person {
private:
  std::string name;

public:
  // generic constructor for passed initial name:
  template <typename STR, typename = EnableIfString<STR>>
  explicit Person(STR &&n) : name(std::forward<STR>(n)) {
    std::cout << "TMPL-CONSTR for ’" << name << "’\n";
  }
  // copy and move constructor:
  Person(Person const &p) : name(p.name) {
    std::cout << "COPY-CONSTR Person ’" << name << "’\n";
  }
  Person(Person &&p) : name(std::move(p.name)) {
    std::cout << "MOVE-CONSTR Person ’" << name << "’\n";
  }
};

template <typename T> class C {
public:
  C() = default;
  C(C const volatile &) = delete;

  template <typename U,
            typename = std::enable_if_t<std::is_constructible_v<int, U>>>
  C(C<U> const &) {
    fmt::println("tmpl copy constructor");
  }
};

template <typename T>
  requires(!std::is_const_v<T>)
void foo(T &t) {
  if constexpr (std::is_compound_v<T>) {
    fmt::println("is compound type");
  } else {
    fmt::println("not const int");
  }
}

template <typename T> void passR(T &&arg) {
  if constexpr (!std::is_reference_v<T>)
    T x;
}

template <typename T> void bar(T const &t1, T const &t2) {
  fmt::println("{}", __PRETTY_FUNCTION__);
  if constexpr (std::is_same_v<T, const char *>) {
    fmt::println("is const char *");
  } else if constexpr (std::is_same_v<T, char[3]>) {
    fmt::println("is char[3]");
  }
}

constexpr bool is_prime(unsigned p) {
  if (p < 2)
    return false;
  if (p % 2 == 0)
    return p == 2;
  for (unsigned i = 3; i * i <= p; i += 2) {
    if (p % i == 0)
      return false;
  }
  return true;
}

template <unsigned p> struct IsPrime {
  static constexpr bool value = is_prime(p);
};

int main() { fmt::println("{}", IsPrime<((2 << 21) + 1)>::value); }
