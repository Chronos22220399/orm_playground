#pragma once
#include <array>
#include <ess/orm/common/meta.hpp>
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
  std::size_t err_len = 0;
  std::size_t err_line = 0;
  std::size_t err_col = 0;
  std::string_view err_msg{};

  constexpr Token operator[](std::size_t idx) const {
    return idx < count ? tokens[idx] : make_token(TokenType::End, 0, 0, 1, 1);
  }
};

// =========== 词法分析器 ===============
template <std::size_t N> class Lexer {
  meta::FixedString<N> m_src;
  std::size_t m_pos = 0;
  std::size_t m_line = 1;
  std::size_t m_col = 1;
  std::string_view err_msg{};

public:
  constexpr explicit Lexer(meta::FixedString<N> src) : m_src(src) {}

  template <std::size_t MaxTokens>
  [[nodiscard]] constexpr LexResult<MaxTokens> tokenize() {
    LexResult<MaxTokens> result;

    while (result.count < MaxTokens) {
      auto token = next_token();
      result.tokens[result.count++] = token;

      if (token.type == TokenType::Unknown) {
        result.has_error = true;
        result.err_pos = token.pos;
        result.err_len = token.len;
        result.err_msg = err_msg;
        result.err_line = m_line;
        result.err_col = m_col;
        break;
      }

      if (token.type == TokenType::End)
        break;
    }

    return result;
  };

private:
  constexpr Token next_token() {
    skip_whitespace();
    if (at_end())
      return make_token(TokenType::End, m_pos, 0, m_line, m_col);

    std::size_t start_pos = m_pos;
    std::size_t start_line = m_line;
    std::size_t start_col = m_col;

    char c = advance();

    switch (c) {
    // 处理单字符
    case ',':
      return make_token(TokenType::Comma, start_pos, 1, start_line, start_col);
    case '.': {
      if (!at_end()) {
        if (is_number(peek())) {
          return scan_number(start_pos, start_line, start_col);
        }
        if (peek() == '.') {
          advance();
          return make_token(TokenType::Unknown, start_pos, m_pos - start_pos,
                            start_line, start_col);
        }
      }
      return make_token(TokenType::Dot, start_pos, 1, start_line, start_col);
    }
    case '*':
      return make_token(TokenType::Star, start_pos, 1, start_line, start_col);
    case '(':
      return make_token(TokenType::Lparen, start_pos, 1, start_line, start_col);
    case ')':
      return make_token(TokenType::Rparen, start_pos, 1, start_line, start_col);
    case '+':
      return make_token(TokenType::Plus, start_pos, 1, start_line, start_col);
    case '-':
      return make_token(TokenType::Minus, start_pos, 1, start_line, start_col);
    case '/':
      return make_token(TokenType::Slash, start_pos, 1, start_line, start_col);
    case '?':
      return make_token(TokenType::PlaceHolder, start_pos, 1, start_line,
                        start_col);
    case '=':
      return make_token(TokenType::Eq, start_pos, 1, start_line, start_col);

    // 处理多字符
    case '<':
      if (peek() == '=') {
        advance();
        return make_token(TokenType::Le, start_pos, 2, start_line, start_col);
      } else if (peek() == '>') {
        advance();
        return make_token(TokenType::Ne, start_pos, 2, start_line, start_col);
      } else {
        return make_token(TokenType::Lt, start_pos, 1, start_line, start_col);
      }
    case '>':
      if (peek() == '=') {
        advance();
        return make_token(TokenType::Ge, start_pos, 2, start_line, start_col);
      } else {
        return make_token(TokenType::Gt, start_pos, 1, start_line, start_col);
      }
    case '!':
      if (peek() == '=') {
        advance();
        return make_token(TokenType::Ne, start_pos, 2, start_line, start_col);
      }
      return make_token(TokenType::Unknown, start_pos, m_pos - start_pos,
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

    err_msg = "Unknown symbol";
    return make_token(TokenType::Unknown, start_pos, m_pos - start_pos,
                      start_line, start_col);
  }

private:
  constexpr bool at_end() const {
    return m_pos >= m_src.size() || m_src[m_pos] == '\0';
  }

  constexpr char peek() const {
    if (at_end())
      return '\0';
    return m_src[m_pos];
  }

  constexpr char peek_next() const {
    if (m_pos + 1 >= m_src.size())
      return '\0';
    return m_src[m_pos + 1];
  }

  constexpr char advance() {
    if (at_end())
      return '\0';
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
      if (at_end() || !is_number(peek())) {
        err_msg = "Trailing dot in numeric literal";
        return make_token(TokenType::Unknown, start_pos, m_pos - start_pos,
                          line, col);
      }
      // 消耗剩余数字
      while (!at_end() && is_number(peek()))
        advance();
    }
    return make_token(TokenType::Number, start_pos, m_pos - start_pos, line,
                      col);
  }

  // 'It''s a book'
  constexpr Token scan_string(char quote, std::size_t start_pos,
                              std::size_t line, std::size_t col) {
    while (!at_end()) {
      if (peek() == quote) {
        advance();
        if (!at_end() && peek() == quote) {
          // 处理转义引号
          advance();
        } else {
          return make_token(TokenType::String, start_pos, m_pos - start_pos,
                            line, col);
        }
        // 普通字符
      } else {
        advance();
      }
    }
    err_msg = "Unterminated string literal";
    return make_token(TokenType::Unknown, start_pos, m_pos - start_pos, line,
                      col);
  }

  constexpr Token scan_identifier(std::size_t start_pos, std::size_t line,
                                  std::size_t col) {
    while (!at_end() && (is_alnum(peek()) || peek() == '_')) {
      advance();
    }

    auto text = std::string_view(m_src).substr(start_pos, m_pos - start_pos);
    TokenType type = lookup_keyword(text);

    if (type != TokenType::Identifier) {
      return make_token(type, start_pos, m_pos - start_pos, line, col);
    }

    return make_token(TokenType::Identifier, start_pos, m_pos - start_pos, line,
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
