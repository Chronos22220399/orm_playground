#pragma once
#include <ess/orm/sql/error.hpp>
#include <ess/orm/sql/lexer.hpp>

namespace ess::orm::sql {

// SELECT 语句信息
struct SelectInfo {
  std::size_t columns_start = 0;
  std::size_t columns_count = 0;
  // 标记是否为 SELECT *
  bool is_start = false;

  // 标记子查询的索引
  std::size_t subquery_node_idx;
};

// INSERT 语句信息
struct InsertInfo {
  std::size_t columns_start;
  std::size_t columns_count;

  bool has_columns = true;
  std::size_t subquery_node_idx = 0;
};

// UPDATE 语句信息
struct UpdateInfo {};

// DELETE 语句信息
struct DeleteInfo {};

struct SyntaxNode {
  TokenType type = TokenType::UNKNOWN;

  std::variant<std::monostate, SelectInfo, InsertInfo, UpdateInfo, DeleteInfo>
      detail;
};

template <std::size_t MaxNodes> struct SyntaxResult {
  bool valid = true;
  SqlError error{};

  std::array<SyntaxNode, MaxNodes> nodes{};
  std::size_t node_cnt;

  constexpr const SyntaxNode &root() const { return nodes[0]; }
};

} // namespace ess::orm::sql
