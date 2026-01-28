#pragma once
#include <array>
#include <string_view>

namespace ess::orm::sql {
// ========== token 类型 =============
// clang-format off
enum class TokenType : uint8_t {
  // keywords
  SELECT, INSERT, UPDATE, DELETE,
  FROM, INTO, VALUES,
  WHERE, SET,
  AND, OR, NOT, IN, LIKE, BETWEEN, IS, NULL_,
  ORDER, BY, ASC, DESC,
  LIMIT, OFFSET,
  JOIN, LEFT, RIGHT, INNER, OUTER, ON,
  AS, DISTINCT,
  COUNT, SUM, AVG, MAX, MIN,
  GROUP, HAVING,
  CREATE, TABLE, DROP, ALTER,
  INDEX, PRIMARY, KEY, FOREIGN, REFERENCES,
  DEFAULT, UNQUE, CHECK,

  // other
  IDENTIFIER, NUMBER, STRING, PLACEHOLDER,

  // operator
  COMMA, DOT, STAR, LPAREN, RPAREN,
  EQ, NE, LT, GT, LE, GE,
  PLUS, MINUS, SLASH,

  // specialty
  END, UNKNOWN
};
// clang-format on

// ========== token 结构 =============
struct Token {
  TokenType type = TokenType::UNKNOWN;
  std::size_t pos = 0;    // 起始位置
  std::size_t length = 0; // 数据长度
  std::size_t line = 1;   // 行号
  std::size_t col = 1;    // 列号

  constexpr bool is(TokenType t) const { return t == type; }
  constexpr bool is_keyword() const { return type <= TokenType::CHECK; }
  constexpr bool is_identifier() const { return type == TokenType::IDENTIFIER; }
  constexpr bool is_end() const { return type == TokenType::END; }
  constexpr bool is_unknown() const { return type == TokenType::UNKNOWN; }
};

inline constexpr Token make_token(TokenType type, std::size_t pos,
                                  std::size_t len, std::size_t line,
                                  std::size_t col) {
  return Token{
      .type = type, .pos = pos, .length = len, .line = line, .col = col};
}

// ========== 关键字表 =============
struct KeywordEntry {
  std::string_view name;
  TokenType type;
};

inline constexpr std::array<KeywordEntry, 45> Keywords = {{
    {"SELECT", TokenType::SELECT},
    {"INSERT", TokenType::INSERT},
    {"UPDATE", TokenType::UPDATE},
    {"DELETE", TokenType::DELETE},
    {"FROM", TokenType::FROM},
    {"INTO", TokenType::INTO},
    {"VALUES", TokenType::VALUES},
    {"WHERE", TokenType::WHERE},
    {"SET", TokenType::SET},
    {"AND", TokenType::AND},
    {"OR", TokenType::OR},
    {"NOT", TokenType::NOT},
    {"IN", TokenType::IN},
    {"LIKE", TokenType::LIKE},
    {"BETWEEN", TokenType::BETWEEN},
    {"IS", TokenType::IS},
    {"NULL", TokenType::NULL_},
    {"ORDER", TokenType::ORDER},
    {"BY", TokenType::BY},
    {"ASC", TokenType::ASC},
    {"DESC", TokenType::DESC},
    {"LIMIT", TokenType::LIMIT},
    {"OFFSET", TokenType::OFFSET},
    {"JOIN", TokenType::JOIN},
    {"LEFT", TokenType::LEFT},
    {"RIGHT", TokenType::RIGHT},
    {"INNER", TokenType::INNER},
    {"OUTER", TokenType::OUTER},
    {"ON", TokenType::ON},
    {"AS", TokenType::AS},
    {"DISTINCT", TokenType::DISTINCT},
    {"COUNT", TokenType::COUNT},
    {"SUM", TokenType::SUM},
    {"AVG", TokenType::AVG},
    {"MAX", TokenType::MAX},
    {"MIN", TokenType::MIN},
    {"GROUP", TokenType::GROUP},
    {"HAVING", TokenType::HAVING},
    {"CREATE", TokenType::CREATE},
    {"TABLE", TokenType::TABLE},
    {"DROP", TokenType::DROP},
    {"ALTER", TokenType::ALTER},
    {"PRIMARY", TokenType::PRIMARY},
    {"KEY", TokenType::KEY},
    {"DEFAULT", TokenType::DEFAULT},
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
  return TokenType::IDENTIFIER;
}
} // namespace ess::orm::sql
