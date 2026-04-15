#pragma once
#include <ess/orm/sql/parse_result.hpp>
#include <ess/orm/sql/semantic/schema_checker.hpp>

namespace ess::orm::sql::semantic {

// 列存在性验证器
template <typename TableType, typename SqlParseResultType>
class ColumnExistenceValidator {
public:
  using SQLType = typename SqlParseResultType::str_type;

  static constexpr bool validate() {
    // TODO: 实现列存在性检查，暂时返回true以保持兼容性
    return true;
  }
};

// 列存在性检查概念
template <typename TableType, typename SqlParseResultType>
concept columns_exist_for =
    ColumnExistenceValidator<TableType, SqlParseResultType>::validate();

} // namespace ess::orm::sql::semantic