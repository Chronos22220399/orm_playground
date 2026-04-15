#pragma once
#include <ess/orm/sql/parse_result.hpp>

namespace ess::orm::sql {

// 基础验证概念：占位符数量匹配
template <typename SqlResult, std::size_t ArgCount>
concept valid_sql_basic = (SqlResult::placeholder_count() == ArgCount);

// 列存在性验证（暂时禁用）
template <typename Table, typename SqlResult>
concept columns_exist =
    true; // TODO: 实现列存在性检查，暂时返回true以保持兼容性

// 类型兼容性验证（占位符实现，需要类型映射系统）
template <typename Table, typename SqlResult>
concept types_compatible =
    true; // TODO: 实现类型兼容性检查，暂时返回true以保持兼容性

// 完整验证概念：带表类型
template <typename Table, typename SqlResult, std::size_t ArgCount>
concept valid_sql_for_table =
    valid_sql_basic<SqlResult, ArgCount> && columns_exist<Table, SqlResult> &&
    types_compatible<Table, SqlResult>;

} // namespace ess::orm::sql