#pragma once
#include <array>
#include <ess/orm/meta.hpp>
#include <ess/orm/sql/token.hpp>

namespace ess::orm::sql {
// 最大 token 数量
inline constexpr std::size_t max_tokens = 128;

// ============ 词法分析结果 ==============
template <std::size_t N> struct LexResult {
  std::array<Token, N> tokens{};
  std::size_t count = 0;
  bool has_error = false;
  std::size_t err_pos = 0;
  std::string_view err_msg{};

  constexpr Token operator[](std::size_t idx) {
    return idx < count ? tokens[idx] : make_token(TokenType::END, 0, 0, 1, 1);
  }
};

// =========== 词法分析器 ===============
template <std::size_t N> class Lexer {
  meta::FixedString<N> m_src;
  std::size_t m_pos = 0;
  std::size_t m_line = 1;
  std::size_t m_col = 1;

public:
  constexpr explicit Lexer(meta::FixedString<N> src) : m_src(src) {}

  constexpr Token next_token() {
    std::size_t start_pos = m_pos;
    std::size_t start_line = m_line;
    std::size_t start_col = m_col;

    char c = advance();

    switch (c) {
    // 处理单字符
    case ',':
      return make_token(TokenType::COMMA, start_pos, 1, start_line, start_col);
    case '.':
      return make_token(TokenType::DOT, start_pos, 1, start_line, start_col);
    case '*':
      return make_token(TokenType::STAR, start_pos, 1, start_line, start_col);
    case '(':
      return make_token(TokenType::LPAREN, start_pos, 1, start_line, start_col);
    case ')':
      return make_token(TokenType::RPAREN, start_pos, 1, start_line, start_col);
    case '+':
      return make_token(TokenType::PLUS, start_pos, 1, start_line, start_col);
    case '-':
      return make_token(TokenType::MINUS, start_pos, 1, start_line, start_col);
    case '/':
      return make_token(TokenType::SLASH, start_pos, 1, start_line, start_col);
    case '?':
      return make_token(TokenType::PLACEHOLDER, start_pos, 1, start_line,
                        start_col);
    case '=':
      return make_token(TokenType::EQ, start_pos, 1, start_line, start_col);

    // 处理多字符
    case '<':
      if (peek() == '=') {
        advance();
        return make_token(TokenType::LE, start_pos, 2, start_line, start_col);
      } else if (peek() == '>') {
        advance();
        return make_token(TokenType::NE, start_pos, 2, start_line, start_col);
      } else {
        return make_token(TokenType::LT, start_pos, 1, start_line, start_col);
      }
    case '>':
      if (peek() == '=') {
        advance();
        return make_token(TokenType::GE, start_pos, 2, start_line, start_col);
      } else {
        return make_token(TokenType::GT, start_pos, 1, start_line, start_col);
      }
    case '!':
      if (peek() == '=') {
        advance();
        return make_token(TokenType::NE, start_pos, 2, start_line, start_col);
      }
      return make_token(TokenType::UNKNOWN, start_pos, m_pos - start_pos,
                        start_line, start_col);
    case '\'':
      return scan_string('\'', start_pos, start_line, start_col);
      // 不同 sql 标准遵循的规则不一样，支持 "
      // 的标准将其作为标识符，因此这里同理
    case '\"':
      return scan_identifier(start_pos, start_line, start_col);
    }

    // 标识符或关键字
    if (is_alpha(c)) {
      return scan_identifier(start_pos, start_line, start_col);
    }

    // 数字
    if (is_number(c)) {
      return scan_number(start_pos, start_line, start_col);
    }

    return make_token(TokenType::UNKNOWN, start_pos, m_pos - start_pos,
                      start_line, start_col);
  }

private:
  constexpr bool at_end() const { return m_pos >= m_src.size(); }

  constexpr char peek() const {
    return m_pos < m_src.size() ? m_src[m_pos] : '\0';
  }

  constexpr char peek_next() const {
    return m_pos + 1 < m_src.size() ? m_src[m_pos + 1] : '\0';
  }

  constexpr char advance() {
    char c = m_src[m_pos++];
    if (c == '\n') {
      m_line++;
      m_col = 1;
    } else {
      m_col++;
    }
    return c;
  }

  constexpr void skip_whitespace() {
    while (!at_end()) {
      char c = peek();
      if (is_space(c)) {
        advance();
      } else if (c == '-' && peek_next() == '-') { // 跳过注释
        while (!at_end() && peek() != '\n')
          advance();
      } else {
        break;
      }
    }
  }

  constexpr Token scan_number(std::size_t start_pos, std::size_t line,
                              std::size_t col) {

    while (!at_end() && is_number(peek()))
      advance();
    // 对于小数
    if (peek() == '.') {
      // 消耗小数点
      advance();
      if (!is_number(peek())) {
        return make_token(TokenType::UNKNOWN, m_pos - start_pos, 0, line, col);
      }
      // 消耗剩余数字
      while (!at_end() && is_number(peek()))
        advance();
    }
    return make_token(TokenType::NUMBER, start_pos, m_pos - start_pos, line,
                      col);
  }

  // 'It''s a book'
  constexpr Token scan_string(char quote, std::size_t start_pos,
                              std::size_t line, std::size_t col) {
    while (!at_end()) {
      // 处理转义引号
      if (peek() == quote && peek_next() == quote) {
        advance();
        advance();
        // 处理结尾
      } else if (peek() == quote) {
        advance();
        return make_token(TokenType::UNKNOWN, start_pos, m_pos - start_pos,
                          line, col);
        // 普通字符
      } else {
        advance();
      }
    }
    return make_token(TokenType::UNKNOWN, start_pos, m_pos - start_pos, line,
                      col);
  }

  constexpr Token scan_identifier(std::size_t start_pos, std::size_t line,
                                  std::size_t col) {
    return make_token(TokenType::UNKNOWN, start_pos, m_pos - start_pos, line,
                      col);
  }

  static constexpr bool is_space(char c) {
    return c == '\n' || c == ' ' || c == '\t' || c == '\r';
  }

  static constexpr bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  static constexpr bool is_number(char c) { return (c >= '0' && c <= '9'); }

  static constexpr bool is_alnum(char c) { return is_alpha(c) || is_number(c); }
};
} // namespace ess::orm::sql
