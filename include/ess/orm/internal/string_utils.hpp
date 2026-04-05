#pragma once

#include <ess/orm/common/meta.hpp>
#include <string>
#include <type_traits>

namespace ess::orm::internal {

// 基础 to_string 实现
template <typename T> std::string to_string(T value) {
  if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
    return std::to_string(value);
  } else if constexpr (std::is_floating_point_v<T>) {
    return std::to_string(value);
  } else if constexpr (std::is_enum_v<T>) {
    return std::to_string(static_cast<std::underlying_type_t<T>>(value));
  } else if constexpr (std::is_same_v<T, bool>) {
    return value ? "true" : "false";
  } else if constexpr (std::is_same_v<T, std::string>) {
    return value;
  } else if constexpr (std::is_same_v<T, const char *>) {
    return std::string(value);
  } else if constexpr (std::is_convertible_v<T, std::string>) {
    return std::string(value);
  } else {
    // 对于 FixedString 等编译期字符串类型
    // 假设有 .str() 方法或可转换为 std::string_view
    // 暂时使用 std::string 构造
    return std::string(value);
  }
}

// 编译期字符串处理
template <meta::FixedString S> std::string to_string() {
  return std::string(S.data(), S.size());
}

// 拼接多个参数
template <typename... Args> std::string concat(Args &&...args) {
  std::string result;
  // 预留大致空间
  result.reserve((to_string(args).size() + ...));
  // 拼接
  ((result += to_string(std::forward<Args>(args))), ...);
  return result;
}

// 简化版格式化：只支持简单替换，不支持格式说明符
template <typename... Args>
std::string simple_format(std::string_view fmt, Args &&...args) {
  // 非常简单的实现：只进行字符串拼接，不处理占位符
  // 对于我们的使用场景，可能只需要拼接，不需要复杂格式化
  // 暂时实现为忽略占位符，直接拼接所有参数
  // 注意：此实现不适用于需要位置参数的情况
  // 但对于 attribute.hpp 和 dsl.hpp 中的使用可能足够
  return concat(std::forward<Args>(args)...);
}

} // namespace ess::orm::internal
