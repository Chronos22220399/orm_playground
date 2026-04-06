#pragma once
#include <array>
#include <cstdint>
#include <string_view>

namespace ess::orm::sql {
// ========== token 类型 =============
// clang-format off
enum class TokenType : std::uint8_t {
  // keywords
  Select, Insert, Update, Delete,
  From, Into, Values,
  Where, Set,
  And, Or, Not, In, Like, Between, Is, Null,
  Order, By, Asc, Desc,
  Limit, Offset,
  Join, Left, Right, Inner, Outer, Cross, On,
  As, Distinct, All,
  Count, Sum, Avg, Max, Min,
  Group, Having,
  Exists,
  Create, Table, Drop, Alter,
  Index, Primary, Key, Foreign, References,
  Default, Unique, Check,

  // other
  Identifier, Number, String, PlaceHolder,

  // operator
  Comma, Dot, Star, Lparen, Rparen,
  Eq, Ne, Lt, Gt, Le, Ge,
  Plus, Minus, Slash,

  // specialty
  End, Unknown
};
// clang-format on

// ========== token 结构 =============
struct Token {
  TokenType type = TokenType::Unknown;
  std::size_t pos = 0;  // 起始位置
  std::size_t len = 0;  // 数据长度
  std::size_t line = 1; // 行号
  std::size_t col = 1;  // 列号

  constexpr bool is(TokenType t) const { return t == type; }
  constexpr bool is_keyword() const { return type <= TokenType::Check; }
  constexpr bool is_identifier() const { return type == TokenType::Identifier; }
  constexpr bool is_end() const { return type == TokenType::End; }
  constexpr bool is_unknown() const { return type == TokenType::Unknown; }
};

inline constexpr Token make_token(TokenType type, std::size_t pos,
                                  std::size_t len, std::size_t line,
                                  std::size_t col) {
  return Token{.type = type, .pos = pos, .len = len, .line = line, .col = col};
}

inline constexpr Token make_end() {
  return Token{.type = TokenType::End, .pos = 0, .len = 0, .line = 1, .col = 1};
}
// ========== 关键字表 =============
struct KeywordEntry {
  std::string_view name;
  TokenType type;
};

inline constexpr std::array<KeywordEntry, 48> Keywords = {{
    {"SELECT", TokenType::Select},   {"INSERT", TokenType::Insert},
    {"UPDATE", TokenType::Update},   {"DELETE", TokenType::Delete},
    {"FROM", TokenType::From},       {"INTO", TokenType::Into},
    {"VALUES", TokenType::Values},   {"WHERE", TokenType::Where},
    {"SET", TokenType::Set},         {"AND", TokenType::And},
    {"OR", TokenType::Or},           {"NOT", TokenType::Not},
    {"IN", TokenType::In},           {"LIKE", TokenType::Like},
    {"BETWEEN", TokenType::Between}, {"IS", TokenType::Is},
    {"NULL", TokenType::Null},       {"ORDER", TokenType::Order},
    {"BY", TokenType::By},           {"ASC", TokenType::Asc},
    {"DESC", TokenType::Desc},       {"LIMIT", TokenType::Limit},
    {"OFFSET", TokenType::Offset},   {"JOIN", TokenType::Join},
    {"LEFT", TokenType::Left},       {"RIGHT", TokenType::Right},
    {"INNER", TokenType::Inner},     {"OUTER", TokenType::Outer},
    {"CROSS", TokenType::Cross},     {"ON", TokenType::On},
    {"AS", TokenType::As},           {"DISTINCT", TokenType::Distinct},
    {"ALL", TokenType::All},         {"COUNT", TokenType::Count},
    {"SUM", TokenType::Sum},         {"AVG", TokenType::Avg},
    {"MAX", TokenType::Max},         {"MIN", TokenType::Min},
    {"GROUP", TokenType::Group},     {"HAVING", TokenType::Having},
    {"EXISTS", TokenType::Exists},   {"CREATE", TokenType::Create},
    {"TABLE", TokenType::Table},     {"DROP", TokenType::Drop},
    {"ALTER", TokenType::Alter},     {"PRIMARY", TokenType::Primary},
    {"KEY", TokenType::Key},         {"DEFAULT", TokenType::Default},
}};

// TODO: 后续可考虑将 Keywords 按照排好序的方式定义，然后 lookup_keyword
// 内部采用二分查找
// 或者直接采用哈希
inline constexpr TokenType lookup_keyword(std::string_view keyword) {
  for (const auto &kw : Keywords) {
    if (kw.name.size() != keyword.size())
      continue;
    // 转大写后匹配
    bool match = true;
    for (int i = 0; match && i < keyword.size(); ++i) {
      char c = keyword[i];
      char upper = (c >= 'a' && c <= 'z') ? (c - 32) : c;
      if (upper != kw.name[i])
        match = false;
    }
    if (match)
      return kw.type;
  }
  return TokenType::Identifier;
}
} // namespace ess::orm::sql
