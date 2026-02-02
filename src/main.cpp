#include <core.hpp>

template <const int *Data, std::size_t Size> struct Source {
  inline static int depth = 1;
  inline static std::size_t cursor = 0;
  static int next() { return cursor < Size ? Data[cursor++] : -1; }
};

template <typename Input> struct Filter {
  inline static int depth = Input::depth + 1;
  static int next() {
    while (true) {
      int val = Input::next();
      if (val == -1)
        return -1;
      if (val % 2 == 0)
        return val;
    }
  }
};

template <typename Input> struct Mapper {
  inline static int depth = Input::depth + 1;
  static int next() {
    int val = Input::next();
    if (val == -1)
      return -1;
    return val * 10;
  }
};

template <typename Input> struct AddOne {
  inline static int depth = Input::depth + 1;
  static int next() {
    int val = Input::next();
    if (val == -1)
      return -1;
    return val + 1;
  }
};

static constexpr int data[] = {1, 2, 3, 4, 5};

using Pipeline = AddOne<Mapper<Filter<Source<data, std::size(data)>>>>;

template <int V> struct Constant {
  static constexpr int value = V;
};

template <char Op, typename Left, typename Right> struct BinaryOp {
  static constexpr int value =
      (Op == '+') ? (Left::value + Right::value) : (Left::value * Right::value);
};

template <std::size_t NextPos, typename Node> struct Result {
  static constexpr std::size_t pos = NextPos;
  using type = Node;
};

struct Token {
  char type;
  int val;
};

static constexpr Token tokens[] = {{'(', 0}, {'1', 1}, {'+', 0}, {'2', 2},
                                   {')', 0}, {'*', 0}, {'3', 3}};

template <std::size_t P> constexpr auto parse_add();

template <std::size_t P> constexpr auto parse_primary() {
  if constexpr (tokens[P].type == '(') {
    constexpr auto inner = parse_add<P + 1>();
    static_assert(tokens[inner.pos].type == ')', "Missing ')'");
    return Result<inner.pos + 1, typename decltype(inner)::type>{};
  } else {
    return Result<P + 1, Constant<tokens[P].val>>{};
  }
}

template <std::size_t P> constexpr auto parse_mul() {
  constexpr auto left = parse_primary<P>();
  if constexpr (left.pos < 7 && tokens[left.pos].type == '*') {
    constexpr auto right = parse_mul<left.pos + 1>();
    using NewNode = BinaryOp<'*', typename decltype(left)::type,
                             typename decltype(right)::type>;
    return Result<right.pos, NewNode>{};
  } else {
    return left;
  }
}

template <std::size_t P> constexpr auto parse_add() {
  constexpr auto left = parse_mul<P>();
  if constexpr (left.pos < 7 && tokens[left.pos].type == '+') {
    constexpr auto right = parse_add<left.pos + 1>();
    using NewNode = BinaryOp<'+', typename decltype(left)::type,
                             typename decltype(right)::type>;
    return Result<right.pos, NewNode>{};
  } else {
    return left;
  }
}

int main() {
  using FinalResult = decltype(parse_add<0>());
  std::cout << FinalResult::type::value << std::endl;
  // int val = Pipeline::next();
  // while (val != -1) {
  //   std::cout << val << std::endl;
  //   val = Pipeline::next();
  // }
  // std::cout << Pipeline::depth << std::endl;
  return 0;
}
