#pragma once
#include <ess/orm/meta.hpp>
#include <string_view>

namespace ess::orm::parser {

enum class SqlType { SELECT, INSERT, UPDATE, DELETE, UNKNOWN };

// 通过编译期哈希降低提高匹配速度
constexpr uint32_t fnv1a_hash(std::string_view str) {
  uint32_t hash = 2166136261u;
  for (char c : str) {
    hash ^= static_cast<uint8_t>(c);
    hash *= 16777619u;
  }
  return hash;
}

template <meta::FixedString Sql> constexpr SqlType begin_with() {
  using namespace meta;
  if constexpr (Sql.size() < 7)
    return SqlType::UNKNOWN;

  if constexpr (fs_starts_with(Sql, "SELECT"_fs))
    return SqlType::SELECT;
  if constexpr (fs_starts_with(Sql, "INSERT"_fs))
    return SqlType::INSERT;
  if constexpr (fs_starts_with(Sql, "UPDATE"_fs))
    return SqlType::UPDATE;
  if constexpr (fs_starts_with(Sql, "DELETE"_fs))
    return SqlType::DELETE;
  return SqlType::UNKNOWN;
}

} // namespace ess::orm::parser
