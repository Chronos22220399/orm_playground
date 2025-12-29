#include <core.hpp>

class Circle {
public:
  static void draw() { ESS_FUNC_LOG(); }
};

class Rectangle {
public:
  void draw() const { ESS_FUNC_LOG(); }
};

template <typename T>
concept GeoObj = requires(T x) {
  { x.draw() } -> std::same_as<void>;
  // { x.center_of_gravity() } -> std::same_as<void>;
};

template <GeoObj GeoObj> void myDraw(GeoObj const &obj) { obj.draw(); }

// MARK: 模拟迭代器
template <typename... Ts> struct typelist;

template <typename T, std::size_t Idx> struct nth_type {};

template <typename T, typename... Ts> struct nth_type<typelist<T, Ts...>, 0> {
  using type = T;
};

template <typename T, typename... Ts, std::size_t Idx>
struct nth_type<typelist<T, Ts...>, Idx> : nth_type<typelist<Ts...>, Idx - 1> {
};

int main() {
  using ts = typelist<int, double, float, char>;
  using second_type = nth_type<ts, 2>::type;
  static_assert(std::is_same_v<second_type, float>);

  // myDraw(Circle{});
  // myDraw(Rectangle{});
}
