#pragma once
#include <ess/orm/sql/parse_result.hpp>

namespace ess::orm::sql {

// MARK: 带表类型查询的语义验证限制
// query<Table, SQL>只能映射Table表的字段，因此必须禁止：
// 1. JOIN查询（无法保证只包含Table表的列）
// 2. 列别名（列名改变，无法映射）
// 3. 表达式列（列名是表达式文本）
// 4. 聚合函数（列名是函数文本）
// 5. 字面量（列名是字面量）

// 基础验证：占位符数量匹配
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
    types_compatible<Table, SqlResult> &&
    !SqlResult::has_joins() &&          // 禁止JOIN查询
    !SqlResult::has_column_aliases() && // 禁止列别名
    !SqlResult::has_expressions() &&    // 禁止表达式列
    !SqlResult::has_aggregates() &&     // 禁止聚合函数
    !SqlResult::has_literals();         // 禁止字面量

} // namespace ess::orm::sql