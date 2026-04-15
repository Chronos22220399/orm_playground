#pragma once
#include <array>
#include <ess/orm/common/meta.hpp>
#include <ess/orm/sql/parser_decl.hpp>
#include <string_view>

namespace ess::orm::sql::semantic {

// 编译期字符串比较工具
template <meta::FixedString Str1, meta::FixedString Str2>
constexpr bool strings_equal() {
  if constexpr (Str1.size() != Str2.size()) {
    return false;
  }

  for (std::size_t i = 0; i < Str1.size(); ++i) {
    if (Str1[i] != Str2[i]) {
      return false;
    }
  }
  return true;
}

// 编译期提取子字符串
template <meta::FixedString SQL, std::size_t Pos, std::size_t Len>
constexpr auto extract_substring() {
  // 创建一个新的FixedString来存储子字符串
  constexpr std::size_t NewSize = Len + 1; // +1 for null terminator
  char buffer[NewSize] = {};

  for (std::size_t i = 0; i < Len && (Pos + i) < SQL.size(); ++i) {
    buffer[i] = SQL[Pos + i];
  }
  buffer[Len] = '\0';

  return meta::FixedString<NewSize>(buffer);
}

// 列名提取器
template <meta::FixedString SQL> class ColumnNameExtractor {
public:
  constexpr ColumnNameExtractor() = default;

  // 从Column结构中提取列名
  template <std::size_t Pos, std::size_t Len>
  static constexpr auto extract_column_name() {
    return extract_substring<SQL, Pos, Len>();
  }

  // 检查列名是否匹配
  template <typename ColumnType, meta::FixedString ExpectedName>
  static constexpr bool column_name_matches(const ColumnType &col) {
    constexpr auto extracted = extract_column_name(col);
    return strings_equal<extracted, ExpectedName>();
  }

  // 从列数组中查找特定列名
  template <typename ColumnArray, std::size_t N, meta::FixedString ColName>
  static constexpr bool find_column_in_array(const ColumnArray &columns,
                                             std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
      if (column_name_matches<decltype(columns[i]), ColName>(columns[i])) {
        return true;
      }
    }
    return false;
  }
};

} // namespace ess::orm::sql::semantic