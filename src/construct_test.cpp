#include <cassert>
#include <fmt/color.h>
#include <fmt/format.h>

class Test {
public:
  // 默认构造
  Test() { fmt::println("Test()        默认构造"); }

  // 拷贝构造
  Test(const Test &) { fmt::println("Test(const&)  拷贝构造"); }

  // 移动构造
  Test(Test &&) noexcept { fmt::println("Test(&&)      移动构造"); }

  // 拷贝赋值
  Test &operator=(const Test &) {
    fmt::println("operator=(const&)  拷贝赋值");
    return *this;
  }

  // 移动赋值
  Test &operator=(Test &&) noexcept {
    fmt::println("operator=(&&)      移动赋值");
    return *this;
  }

  // 析构
  ~Test() { fmt::println("~Test()       析构"); }
};

// 按值返回（用于观察 NRVO / 移动 / 拷贝）
Test make_test() {
  fmt::println("进入 make_test()");
  Test t;
  fmt::println("即将 return t");
  return t;
}

// 按值传参（观察拷贝 / 移动）
void take_by_value(Test t) { fmt::println("进入 take_by_value()"); }

// 按引用传参（无构造）
void take_by_ref(const Test &t) { fmt::println("进入 take_by_ref()"); }

int main() {
  fmt::println("---- 默认构造 ----");
  Test a;

  fmt::println("\n---- 拷贝构造 ----");
  Test b = a;

  fmt::println("\n---- 移动构造 ----");
  Test c = Test{};

  fmt::println("\n---- 拷贝赋值 ----");
  b = a;

  fmt::println("\n---- 移动赋值 ----");
  b = Test{};

  fmt::println("\n---- 函数按值传参（左值） ----");
  take_by_value(a);

  fmt::println("\n---- 函数按值传参（右值） ----");
  take_by_value(Test{});

  fmt::println("\n---- 函数按引用传参 ----");
  take_by_ref(a);

  fmt::println("\n---- 函数返回值 ----");
  Test d = make_test();

  fmt::println("\n---- main 结束 ----");
}
