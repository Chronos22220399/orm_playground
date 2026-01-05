#include <core.hpp>

namespace ess {
template <typename Iter, typename Callable, typename... Args>
void foreach (Iter current, Iter end, Callable op, Args const &...args) {
  while (current != end) {
    std::invoke(op, args..., *current);
    ++current;
  }
}
} // namespace ess

template <typename T> void print(T t) { fmt::println("{}", t); }

template <typename Func> auto wrapper(Func &&func) {
  return [fn = std::forward<Func>(func)]() {
    std::invoke(std::forward<Func>(fn));
  };
}

class MyClass {
public:
  void memfunc(int n) const { ESS_FUNC_LOG(); }
};

template <typename Callable, typename... Args>
decltype(auto) call(Callable &&op, Args &&...args) {
  if constexpr (std::is_same_v<std::invoke_result_t<Callable, Args...>, void>) {
    std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
    return;
  } else {
    decltype(auto) ret{
        std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...)};
    return ret;
  }
}

template <typename T1, typename T2,
          typename RT = std::decay_t<decltype(true ? std::declval<T1>()
                                                   : std::declval<T2>())>>
RT max(T1 a, T2 b) {
  return a > b ? a : b;
}

template <typename T>
concept is_not_reference = !std::is_reference_v<T>;

template <is_not_reference T> struct RefMem {
private:
  T zero{};

public:
  RefMem() : zero() {}
};

template <typename T, int &SZ> struct Arr {
private:
  std::vector<T> elems;

public:
  Arr() : elems(SZ, 1) {}

  void print() const {
    fmt::println("{}", SZ);
    for (int i = 0; i < SZ; ++i) {
      fmt::println("{}", elems[i]);
    }
  }
};

int size = 10;

auto func() {
  struct result_t {
    const std::string reason;
    const bool success;
  };
  return result_t{.reason = "no reason", .success = true};
}

template <typename T> class Cont {
private:
  T *elems;

public:
  template <typename U = T>
  typename std::conditional_t<std::is_move_constructible_v<U>, T &&, T &> foo();
};

struct Node {
  std::string data;
  Cont<Node> next;
};

template <typename T> decltype(auto) bad(T &&t) { return std::forward<T>(t); }

int main() {
  int a = 10;
  static_assert(std::is_reference_v<decltype(bad(10))>);

  // std::vector vec{1, 3, 4};
  // MyClass mc;
  // call([&vec]() {
  //   ess::foreach (vec.begin(), vec.end(),
  //                 [](auto const &n) { fmt::println("{}", n); });
  // });
}
