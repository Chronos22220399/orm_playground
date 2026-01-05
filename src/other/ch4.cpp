#include <core.hpp>

template <typename> inline constexpr bool always_false = false;

//
template <typename T, int N, int M>
consteval bool less(T const (&a)[N], T const (&b)[M]) {
  for (int i = 0; i < N && i < M; ++i) {
    if (a[i] < b[i])
      return true;
    if (b[i] < a[i])
      return false;
  }
  return N < M;
}

template <typename T> void print(T first) { fmt::println("{}", first); }

template <typename T, typename... Types> void print(T first, Types... others) {
  print(first);
  fmt::println("{}", sizeof...(Types));
  print(others...);
}

template <typename... T> auto foldSum(T... s) { return (1 + ... + s); }

struct Node {
  Node *left;
  Node *right;
  int value = 0;

  Node(int value = 0) : left(nullptr), right(nullptr), value(value) {}
};

template <typename... Nexts> Node *print(Node *node, Nexts... nexts) {
  if (!node)
    return nullptr;
  fmt::print("{} ", node->value);
  ((node = node->*nexts, fmt::println("{}", node->value)), ...);
  return node;
}

template <typename... Args> void foldPrint(Args... args) {
  (fmt::print("{} ", args), ...);
}

template <typename T, typename... Types>
consteval bool isHomogenius(T, Types...) {
  return (std::is_same_v<T, Types> && ...);
}

template <typename Container1, typename Container2, typename... Indeices>
void printElems(Container1 const &colors, Container2 const &container,
                Indeices... idx) {
  (fmt::print(fmt::fg(colors[idx]), "{}\n", container[idx]), ...);
}

template <typename T, std::size_t N> struct MyArray {
  using value_type = T;
  static constexpr std::size_t size = N;

  template <typename... Args>
    requires(sizeof...(Args) == N)
  MyArray(Args const &&...args) {}
};

template <typename T, typename... Args>
  requires(std::is_same_v<T, Args> && ...)
MyArray(T, Args...) -> MyArray<T, 1 + sizeof...(Args)>;

/*
 * @summary: 基类参数、函数不一定被继承
 */
template <typename T> class Base {
public:
  void bar();
};

template <typename T> class Derived : Base<T> {
public:
  void foo() {
    this->bar();
    // or
    Base<T>::bar();
  }
};

int main() {
  // MyArray a(112, 23, 23, 23245);
  // fmt::println("{}", a.size);
  //
  // using fc = fmt::color;

  // std::vector<fc> vc{fc::azure, fc::aquamarine, fc::turquoise, fc::plum,
  //                    fc::powder_blue};
  // std::vector<int> vec{1, 2, 3, 4, 5};
  // printElems(vc, vec, 0, 1, 2, 3, 4);

  // if constexpr (isHomogenius(1, 2, 's', 4)) {
  //   fmt::print(fg(fmt::color::red),
  //              "all of those parameters are the int type\n");
  // } else {
  //   fmt::print(fmt::fg(fmt::color::blue), "some parameters not the int
  //   type\n");
  // };

  // foldPrint(1, 2, 3);
  // auto node1 = new Node{10};
  // auto node2 = new Node{20};
  // node1->left = node2;
  // node2->left = new Node{340};
  // print(node1, &Node::left, &Node::left);

  // print(1, 2, 3);
  // fmt::println("{}", foldSum(1, 2, 3));
  return 0;
}
