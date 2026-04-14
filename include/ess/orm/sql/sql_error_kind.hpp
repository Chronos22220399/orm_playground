#pragma once
#include <cstdint>

namespace ess::orm::sql {

enum class SQLErrorKind : uint8_t {
  None,
  InvalidToken,
  UnknownBeginning,
  ExpectedStarOrColumns,
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
  UnknownError,
};

} // namespace ess::orm::sql