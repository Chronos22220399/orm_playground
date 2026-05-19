#pragma once
#include <cstdint>

namespace ess::orm::sql {

enum class SQLErrorKind : uint8_t {
  None,
  InvalidToken,
  UnknownBeginning,
  ExpectedStarOrColumns,
  ExpectedColumnsAfterComma,
  ExpectedFrom,
  ExpectedTableAfterFrom,
  ExpectedIdentifier,
  ExpectedIdentifierInWhereClause,
  ExpectedOperator,
  ExpectedLiteralOrPlaceHolder,
  ExpectedRightParen,
  ExpectedLeftParenAfterIn,
  ExpectedLeftParenAfterExists,
  ExpectedLiteralOrPlaceHolderInList,
  ExpectedStringLiteralAfterLike,
  ExpectedAndInBetweenClause,
  ExpectedNullAfterNotInIsClause,
  ExpectedNullAfterInInIsClause,
  ExpectedByAfterGroup,
  ExpectedByAfterGroupInGroupByClause,
  ExpectedIdentifierAfterByInGroupByClause,
  HavingWithoutGroupBy,
  ExpectedByAfterOrderInOrderByClause,
  ExpectedIdentifierAfterByInOrderByClause,
  ExpectedLeftParenAfterAggregate,
  ExpectedIdentifierInAggregate,
  ExpectedOperatorAfterAggregate,
  ExpectedInOrLikeAfterNot,
  InvalidEnd,
  NullComparisonNotAllowed,
  InvalidIsNullOperand,
  MissingOperator,
  InvalidSelectItem,
  ExpectedInsert,
  ExpectedUpdate,
  ExpectedDelete,
  ExpectedInto,
  ExpectedSet,
  ExpectedFromAfterDelete,
  ExpectedValuesOrSelect,
  ExpectedValue,
  ExpectedAssignment,

  // 语义错误（新增）
  PlaceholderCountMismatch, // 占位符数量不匹配
  ColumnNotFound,           // 列不存在
  TableMismatch,            // 表名不匹配
  TypeMismatch,             // 类型不兼容
  InvalidFunctionCall,      // 函数调用无效
  FunctionNotFound,         // 函数不存在
  InvalidFunctionArgs,      // 函数参数无效
  AggregateWithoutGroupBy,  // 聚合缺少GROUP BY
  IncompleteStatement,      // 语句不完整
  InvalidNullComparison,    // 无效的NULL比较
  ExpectedExpression,       // 期望表达式但未找到

  UnknownError,
};

} // namespace ess::orm::sql
