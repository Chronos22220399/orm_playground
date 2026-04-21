#include <iostream>
#include <string_view>

void bind_one(int index, std::string_view param) {
  std::cout << "string_view: " << param << std::endl;
}

void bind_one(int index, int param) {
  std::cout << "int: " << param << std::endl;
}

void bind_one(int index, bool param) {
  std::cout << "bool: " << param << std::endl;
}

int main() {
  // 测试 const char* 会调用哪个重载
  bind_one(1, "hello");

  // 测试显式转换
  bind_one(2, std::string_view("world"));

  return 0;
}