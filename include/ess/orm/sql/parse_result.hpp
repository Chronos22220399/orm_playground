#pragma once
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/sql/parser.hpp>

namespace ess::orm::sql {

// SQL解析结果数据聚合类型
template <meta::FixedString SQL> struct SqlParseResult {
  using str_type = meta::FixedString<SQL.size()>;

  // 编译期解析结果
  static constexpr auto lex_result = Lexer(SQL).template tokenize<128>();
  static constexpr auto parse_result = Parser(lex_result.tokens).parse();

  // 查询接口
  static constexpr std::size_t placeholder_count() {
    return parse_result.semantic_result.placeholder_count;
  }

  static constexpr const auto &get_parse_result() { return parse_result; }

  static constexpr const auto &get_lex_result() { return lex_result; }

  // 隐式转换保持兼容（转换为FixedString）
  constexpr operator str_type() const { return SQL; }

  // 获取原始SQL字符串
  static constexpr auto str() -> str_type { return SQL; }

  // 获取SQL字符串视图
  static constexpr std::string_view view() { return SQL.view(); }
};

} // namespace ess::orm::sql