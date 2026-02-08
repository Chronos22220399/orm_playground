// parser/parser.hpp
#pragma once
#include <ess/orm/sql/ast/statement.hpp>
#include <ess/orm/sql/lexer.hpp>

namespace ess::orm::sql {

template <std::size_t SrcLen, std::size_t MaxTokens = 256> class _Parser {
  meta::FixedString<SrcLen> m_src;
  LexResult<MaxTokens> m_tokens;
  std::size_t m_pos = 0;

  // 错误状态
  bool m_has_error = false;
  std::string_view m_err_msg{};
  std::size_t m_err_pos = 0;
  std::size_t m_err_len = 0;
  std::size_t m_err_line = 0;
  std::size_t m_err_col = 0;

  ast::ParseContext m_ctx{};

public:
  // ========== 解析结果 ==========
  struct Result {
    ast::ParseContext ctx{};
    bool has_error = false;
    std::size_t err_pos = 0;
    std::size_t err_len = 0; // 错误 token 长度
    std::size_t err_line = 0;
    std::size_t err_col = 0;
    std::string_view err_msg{};

    constexpr const ast::StmtNode &root() const { return ctx.root(); }
    constexpr bool is_select() const { return root().is_select(); }
    constexpr bool is_insert() const { return root().is_insert(); }
    constexpr bool is_update() const { return root().is_update(); }
    constexpr bool is_delete() const { return root().is_delete(); }
  };

  // ========== 构造函数 ==========
  constexpr explicit _Parser(meta::FixedString<SrcLen> src)
      : m_src(src), m_tokens(Lexer(src).template tokenize<MaxTokens>()) {}

  // ========== 解析入口 ==========
  constexpr Result parse() {
    Result result;

    // 检查词法错误
    if (m_tokens.has_error) {
      result.has_error = true;
      result.err_msg = m_tokens.err_msg;
      result.err_pos = m_tokens.err_pos;
      result.err_len = m_tokens.err_len;
      result.err_line = m_tokens.err_line;
      result.err_col = m_tokens.err_col;
      return result;
    }

    // 分配主语句
    auto root_ref = m_ctx.alloc_stmt();
    if (!root_ref) {
      set_error("Statement allocation failed");
      return make_result(result);
    }

    // 根据首个 token 解析
    switch (current().type) {
    case TokenType::Select:
      m_ctx[root_ref].init_select();
      parse_select(m_ctx[root_ref].as_select());
      break;
    case TokenType::Insert:
      m_ctx[root_ref].init_insert();
      parse_insert(m_ctx[root_ref].as_insert());
      break;
    case TokenType::Update:
      m_ctx[root_ref].init_update();
      parse_update(m_ctx[root_ref].as_update());
      break;
    case TokenType::Delete:
      m_ctx[root_ref].init_delete();
      parse_delete(m_ctx[root_ref].as_delete());
      break;
    default:
      set_error("Expected SELECT, INSERT, UPDATE, or DELETE");
    }

    return make_result(result);
  }

private:
  // ========== 错误处理 ==========
  constexpr void set_error(std::string_view msg) {
    if (m_has_error)
      return;

    m_has_error = true;
    m_err_msg = msg;
    auto tok = current();
    m_err_pos = tok.pos;
    m_err_len = tok.len;
    m_err_line = tok.line;
    m_err_col = tok.col;
  }

  constexpr void set_error_at(std::string_view msg, const Token &tok) {
    if (m_has_error)
      return;

    m_has_error = true;
    m_err_msg = msg;
    m_err_pos = tok.pos;
    m_err_len = tok.len;
    m_err_line = tok.line;
    m_err_col = tok.col;
  }

  // 在上一个 token 后面报错（用于缺失 token 的情况）
  constexpr void set_error_after_prev(std::string_view msg) {
    if (m_has_error)
      return;

    m_has_error = true;
    m_err_msg = msg;

    // 使用前一个 token 的结束位置
    if (m_pos > 0) {
      auto prev = m_tokens[m_pos - 1];
      m_err_pos = prev.pos + prev.len;
      m_err_len = 0;
      m_err_line = prev.line;
      m_err_col = prev.col + prev.len;
    } else {
      auto tok = current();
      m_err_pos = tok.pos;
      m_err_len = tok.len;
      m_err_line = tok.line;
      m_err_col = tok.col;
    }
  }

  constexpr Result &make_result(Result &result) {
    result.has_error = m_has_error;
    result.err_msg = m_err_msg;
    result.err_pos = m_err_pos;
    result.err_len = m_err_len;
    result.err_line = m_err_line;
    result.err_col = m_err_col;
    result.ctx = m_ctx;
    return result;
  }

  constexpr bool ok() const { return !m_has_error; }

  // ========== Token 操作 ==========
  constexpr Token current() const { return m_tokens[m_pos]; }

  constexpr Token prev() const {
    return m_pos > 0 ? m_tokens[m_pos - 1] : m_tokens[0];
  }

  constexpr Token peek(std::size_t offset = 1) const {
    return m_tokens[m_pos + offset];
  }

  constexpr bool check(TokenType type) const { return current().type == type; }

  constexpr bool at_end() const { return check(TokenType::End); }

  constexpr Token advance() {
    if (!at_end())
      m_pos++;
    return m_tokens[m_pos - 1];
  }

  constexpr bool match(TokenType type) {
    if (check(type)) {
      advance();
      return true;
    }
    return false;
  }

  constexpr bool expect(TokenType type, std::string_view msg) {
    if (check(type)) {
      advance();
      return true;
    }
    set_error(msg);
    return false;
  }

  constexpr ast::Identifier make_id(const Token &tok) const {
    return {.pos = tok.pos, .len = tok.len};
  }

  constexpr ast::SourceLocation make_loc(const Token &tok) const {
    return ast::SourceLocation(tok);
  }

  // ========== 表达式解析 ==========
  constexpr ast::ExprRef parse_expr() {
    if (!ok())
      return ast::ExprRef::invalid();
    return parse_or_expr();
  }

  constexpr ast::ExprRef parse_or_expr() {
    auto left = parse_and_expr();
    if (!ok() || !left)
      return ast::ExprRef::invalid();

    while (match(TokenType::Or)) {
      auto right = parse_and_expr();
      if (!ok() || !right)
        return ast::ExprRef::invalid();
      left = make_binary(ast::BinaryOp::Or, left, right);
    }
    return left;
  }

  constexpr ast::ExprRef parse_and_expr() {
    auto left = parse_not_expr();
    if (!ok() || !left)
      return ast::ExprRef::invalid();

    while (match(TokenType::And)) {
      auto and_tok = prev();
      auto right = parse_not_expr();
      if (!ok())
        return ast::ExprRef::invalid();
      if (!right) {
        set_error_after_prev("Expected expression after AND");
        return ast::ExprRef::invalid();
      }
      left = make_binary(ast::BinaryOp::And, left, right);
    }
    return left;
  }

  constexpr ast::ExprRef parse_not_expr() {
    if (!ok())
      return ast::ExprRef::invalid();

    // NOT
    if (match(TokenType::Not)) {
      auto not_tok = prev();

      // NOT EXISTS
      if (check(TokenType::Exists)) {
        return parse_exists_expr(true);
      }

      // NOT IN / NOT BETWEEN / NOT LIKE 由 parse_comparison 处理
      auto operand = parse_comparison();
      if (!ok())
        return ast::ExprRef::invalid();
      if (!operand) {
        set_error_after_prev("Expected expression after NOT");
        return ast::ExprRef::invalid();
      }
      return make_unary(ast::UnaryOp::Not, operand);
    }

    // EXISTS
    if (check(TokenType::Exists)) {
      return parse_exists_expr(false);
    }

    return parse_comparison();
  }

  constexpr ast::ExprRef parse_exists_expr(bool is_not) {
    auto exists_tok = advance(); // 消费 EXISTS

    if (!expect(TokenType::Lparen, "Expected '(' after EXISTS")) {
      return ast::ExprRef::invalid();
    }

    if (!check(TokenType::Select)) {
      set_error("Expected SELECT inside EXISTS");
      return ast::ExprRef::invalid();
    }

    auto sub_ref = m_ctx.alloc_stmt();
    if (!sub_ref) {
      set_error("Subquery allocation failed");
      return ast::ExprRef::invalid();
    }

    m_ctx[sub_ref].init_select();
    parse_select(m_ctx[sub_ref].as_select());
    if (!ok())
      return ast::ExprRef::invalid();

    if (!expect(TokenType::Rparen, "Expected ')' after EXISTS subquery")) {
      return ast::ExprRef::invalid();
    }

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Exists;
    e.loc = make_loc(exists_tok);
    e.subquery_stmt_idx = sub_ref.index;
    e.exists_is_not = is_not;
    return ref;
  }

  constexpr ast::ExprRef parse_comparison() {
    if (!ok())
      return ast::ExprRef::invalid();

    auto left = parse_additive();
    if (!ok() || !left)
      return ast::ExprRef::invalid();

    // 比较操作符
    if (check(TokenType::Eq) || check(TokenType::Ne) || check(TokenType::Lt) ||
        check(TokenType::Le) || check(TokenType::Gt) || check(TokenType::Ge)) {

      auto op_tok = advance();
      ast::BinaryOp op;
      switch (op_tok.type) {
      case TokenType::Eq:
        op = ast::BinaryOp::Eq;
        break;
      case TokenType::Ne:
        op = ast::BinaryOp::Ne;
        break;
      case TokenType::Lt:
        op = ast::BinaryOp::Lt;
        break;
      case TokenType::Le:
        op = ast::BinaryOp::Le;
        break;
      case TokenType::Gt:
        op = ast::BinaryOp::Gt;
        break;
      case TokenType::Ge:
        op = ast::BinaryOp::Ge;
        break;
      default:
        op = ast::BinaryOp::Eq;
        break;
      }

      auto right = parse_additive();
      if (!ok())
        return ast::ExprRef::invalid();
      if (!right) {
        set_error_after_prev("Expected expression after comparison operator");
        return ast::ExprRef::invalid();
      }
      return make_binary(op, left, right);
    }

    // NOT IN / NOT BETWEEN / NOT LIKE (这些 NOT 不在 parse_not_expr 处理)
    bool is_not = false;
    if (check(TokenType::Not)) {
      auto next = peek().type;
      if (next == TokenType::In || next == TokenType::Between ||
          next == TokenType::Like) {
        is_not = true;
        advance(); // 消费 NOT
      }
    }

    if (check(TokenType::In))
      return parse_in_expr(left, is_not);
    if (check(TokenType::Between))
      return parse_between_expr(left, is_not);
    if (check(TokenType::Like))
      return parse_like_expr(left, is_not);
    if (check(TokenType::Is))
      return parse_is_null_expr(left);

    return left;
  }

  constexpr ast::ExprRef parse_additive() {
    if (!ok())
      return ast::ExprRef::invalid();

    auto left = parse_multiplicative();
    if (!ok() || !left)
      return ast::ExprRef::invalid();

    while (check(TokenType::Plus) || check(TokenType::Minus)) {
      auto op_tok = advance();
      ast::BinaryOp op = (op_tok.type == TokenType::Plus) ? ast::BinaryOp::Add
                                                          : ast::BinaryOp::Sub;

      auto right = parse_multiplicative();
      if (!ok())
        return ast::ExprRef::invalid();
      if (!right) {
        set_error_after_prev("Expected expression after operator");
        return ast::ExprRef::invalid();
      }
      left = make_binary(op, left, right);
      if (!left)
        return ast::ExprRef::invalid();
    }
    return left;
  }

  constexpr ast::ExprRef parse_multiplicative() {
    if (!ok())
      return ast::ExprRef::invalid();

    auto left = parse_unary();
    if (!ok() || !left)
      return ast::ExprRef::invalid();

    while (check(TokenType::Star) || check(TokenType::Slash)) {
      auto op_tok = advance();
      ast::BinaryOp op = (op_tok.type == TokenType::Star) ? ast::BinaryOp::Mul
                                                          : ast::BinaryOp::Div;

      auto right = parse_unary();
      if (!ok())
        return ast::ExprRef::invalid();
      if (!right) {
        set_error_after_prev("Expected expression after operator");
        return ast::ExprRef::invalid();
      }
      left = make_binary(op, left, right);
      if (!left)
        return ast::ExprRef::invalid();
    }
    return left;
  }

  constexpr ast::ExprRef parse_unary() {
    if (!ok())
      return ast::ExprRef::invalid();

    if (check(TokenType::Minus) || check(TokenType::Plus)) {
      auto op_tok = advance();
      ast::UnaryOp op = (op_tok.type == TokenType::Minus) ? ast::UnaryOp::Minus
                                                          : ast::UnaryOp::Plus;

      auto operand = parse_primary();
      if (!ok())
        return ast::ExprRef::invalid();
      if (!operand) {
        set_error_after_prev("Expected expression after unary operator");
        return ast::ExprRef::invalid();
      }
      return make_unary(op, operand);
    }
    return parse_primary();
  }

  constexpr ast::ExprRef parse_primary() {
    if (!ok())
      return ast::ExprRef::invalid();

    // 括号或子查询
    if (match(TokenType::Lparen)) {
      if (check(TokenType::Select)) {
        return parse_scalar_subquery();
      }
      auto expr = parse_expr();
      if (!ok())
        return ast::ExprRef::invalid();
      if (!expr) {
        set_error("Expected expression after '('");
        return ast::ExprRef::invalid();
      }
      if (!expect(TokenType::Rparen, "Expected ')'")) {
        return ast::ExprRef::invalid();
      }
      return expr;
    }

    // 数字
    if (check(TokenType::Number)) {
      auto tok = advance();
      auto ref = m_ctx.alloc_expr();
      if (!ref) {
        set_error("Expression allocation failed");
        return ast::ExprRef::invalid();
      }
      auto &e = m_ctx.expr(ref);
      e.kind = ast::Expr::Kind::Literal;
      e.loc = make_loc(tok);
      e.literal_pos = tok.pos;
      e.literal_len = tok.len;
      e.literal_is_string = false;
      return ref;
    }

    // 字符串
    if (check(TokenType::String)) {
      auto tok = advance();
      auto ref = m_ctx.alloc_expr();
      if (!ref) {
        set_error("Expression allocation failed");
        return ast::ExprRef::invalid();
      }
      auto &e = m_ctx.expr(ref);
      e.kind = ast::Expr::Kind::Literal;
      e.loc = make_loc(tok);
      e.literal_pos = tok.pos;
      e.literal_len = tok.len;
      e.literal_is_string = true;
      return ref;
    }

    // NULL
    if (match(TokenType::Null)) {
      auto ref = m_ctx.alloc_expr();
      if (!ref) {
        set_error("Expression allocation failed");
        return ast::ExprRef::invalid();
      }
      auto &e = m_ctx.expr(ref);
      e.kind = ast::Expr::Kind::Null;
      e.loc = make_loc(prev());
      return ref;
    }

    // 占位符
    if (check(TokenType::PlaceHolder)) {
      auto tok = advance();
      auto ref = m_ctx.alloc_expr();
      if (!ref) {
        set_error("Expression allocation failed");
        return ast::ExprRef::invalid();
      }
      auto &e = m_ctx.expr(ref);
      e.kind = ast::Expr::Kind::Placeholder;
      e.loc = make_loc(tok);
      return ref;
    }

    // 聚合函数
    if (is_aggregate()) {
      return parse_aggregate();
    }

    // 标识符（函数调用或列引用）
    if (check(TokenType::Identifier)) {
      if (peek().type == TokenType::Lparen) {
        return parse_function();
      }
      return parse_column_ref();
    }

    // 无法识别
    // 不在这里报错，让调用者决定
    return ast::ExprRef::invalid();
  }

  // ========== 特殊表达式解析 ==========
  constexpr ast::ExprRef parse_in_expr(ast::ExprRef left, bool is_not) {
    auto in_tok = advance(); // IN

    if (!expect(TokenType::Lparen, "Expected '(' after IN")) {
      return ast::ExprRef::invalid();
    }

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::In;
    e.loc = make_loc(in_tok);
    e.in_expr_idx = left.index;
    e.in_is_not = is_not;

    if (check(TokenType::Select)) {
      e.in_is_subquery = true;
      auto sub_ref = m_ctx.alloc_stmt();
      if (!sub_ref) {
        set_error("Subquery allocation failed");
        return ast::ExprRef::invalid();
      }
      m_ctx[sub_ref].init_select();
      parse_select(m_ctx[sub_ref].as_select());
      if (!ok())
        return ast::ExprRef::invalid();
      e.in_subquery_idx = sub_ref.index;
    } else {
      e.in_is_subquery = false;
      e.in_values_start = m_ctx.expr_pool.value_count;

      do {
        auto val = parse_expr();
        if (!ok())
          return ast::ExprRef::invalid();
        if (!val) {
          set_error("Expected expression in IN list");
          return ast::ExprRef::invalid();
        }
        m_ctx.expr_pool.value_indices[m_ctx.expr_pool.value_count++] =
            val.index;
        e.in_values_count++;
      } while (match(TokenType::Comma));
    }

    if (!expect(TokenType::Rparen, "Expected ')' after IN list")) {
      return ast::ExprRef::invalid();
    }
    return ref;
  }

  constexpr ast::ExprRef parse_between_expr(ast::ExprRef left, bool is_not) {
    auto between_tok = advance(); // BETWEEN

    auto low = parse_additive();
    if (!ok())
      return ast::ExprRef::invalid();
    if (!low) {
      set_error_after_prev("Expected expression after BETWEEN");
      return ast::ExprRef::invalid();
    }

    if (!expect(TokenType::And, "Expected AND in BETWEEN expression")) {
      return ast::ExprRef::invalid();
    }

    auto high = parse_additive();
    if (!ok())
      return ast::ExprRef::invalid();
    if (!high) {
      set_error_after_prev("Expected expression after AND in BETWEEN");
      return ast::ExprRef::invalid();
    }

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Between;
    e.loc = make_loc(between_tok);
    e.between_expr_idx = left.index;
    e.between_low_idx = low.index;
    e.between_high_idx = high.index;
    e.between_is_not = is_not;
    return ref;
  }

  constexpr ast::ExprRef parse_like_expr(ast::ExprRef left, bool is_not) {
    auto like_tok = advance(); // LIKE

    auto pattern = parse_primary();
    if (!ok())
      return ast::ExprRef::invalid();
    if (!pattern) {
      set_error_after_prev("Expected pattern after LIKE");
      return ast::ExprRef::invalid();
    }

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Like;
    e.loc = make_loc(like_tok);
    e.like_expr_idx = left.index;
    e.like_pattern_idx = pattern.index;
    e.like_is_not = is_not;
    return ref;
  }

  constexpr ast::ExprRef parse_is_null_expr(ast::ExprRef left) {
    auto is_tok = advance(); // IS
    bool is_not = match(TokenType::Not);

    if (!expect(TokenType::Null, "Expected NULL after IS")) {
      return ast::ExprRef::invalid();
    }

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::IsNull;
    e.loc = make_loc(is_tok);
    e.is_null_expr_idx = left.index;
    e.is_null_is_not = is_not;
    return ref;
  }

  constexpr ast::ExprRef parse_scalar_subquery() {
    auto sub_ref = m_ctx.alloc_stmt();
    if (!sub_ref) {
      set_error("Subquery allocation failed");
      return ast::ExprRef::invalid();
    }

    m_ctx[sub_ref].init_select();
    parse_select(m_ctx[sub_ref].as_select());
    if (!ok())
      return ast::ExprRef::invalid();

    if (!expect(TokenType::Rparen, "Expected ')' after subquery")) {
      return ast::ExprRef::invalid();
    }

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Subquery;
    e.subquery_stmt_idx = sub_ref.index;
    return ref;
  }

  constexpr ast::ExprRef parse_column_ref() {
    auto tok = advance();
    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Column;
    e.loc = make_loc(tok);

    if (match(TokenType::Dot)) {
      if (!check(TokenType::Identifier) && !check(TokenType::Star)) {
        set_error("Expected column name after '.'");
        return ast::ExprRef::invalid();
      }
      auto next = advance();

      if (match(TokenType::Dot)) {
        if (!check(TokenType::Identifier)) {
          set_error("Expected column name after '.'");
          return ast::ExprRef::invalid();
        }
        auto col = advance();
        e.column_name.schema = make_id(tok);
        e.column_name.table = make_id(next);
        e.column_name.name = make_id(col);
      } else {
        e.column_name.table = make_id(tok);
        e.column_name.name = make_id(next);
      }
    } else {
      e.column_name.name = make_id(tok);
    }
    return ref;
  }

  constexpr ast::ExprRef parse_function() {
    auto name_tok = advance();

    if (!expect(TokenType::Lparen, "Expected '(' after function name")) {
      return ast::ExprRef::invalid();
    }

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Function;
    e.loc = make_loc(name_tok);
    e.func_name = make_id(name_tok);
    e.func_args_start = m_ctx.expr_pool.value_count;
    e.func_is_aggregate = false;

    if (!check(TokenType::Rparen)) {
      do {
        auto arg = parse_expr();
        if (!ok())
          return ast::ExprRef::invalid();
        if (!arg) {
          set_error("Expected expression in function arguments");
          return ast::ExprRef::invalid();
        }
        m_ctx.expr_pool.value_indices[m_ctx.expr_pool.value_count++] =
            arg.index;
        e.func_args_count++;
      } while (match(TokenType::Comma));
    }

    if (!expect(TokenType::Rparen, "Expected ')' after function arguments")) {
      return ast::ExprRef::invalid();
    }
    return ref;
  }

  constexpr ast::ExprRef parse_aggregate() {
    auto name_tok = advance();

    if (!expect(TokenType::Lparen, "Expected '(' after aggregate function")) {
      return ast::ExprRef::invalid();
    }

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Function;
    e.loc = make_loc(name_tok);
    e.func_name = make_id(name_tok);
    e.func_is_aggregate = true;
    e.func_distinct = match(TokenType::Distinct);

    if (match(TokenType::Star)) {
      // COUNT(*)
    } else if (!check(TokenType::Rparen)) {
      e.func_args_start = m_ctx.expr_pool.value_count;
      auto arg = parse_expr();
      if (!ok())
        return ast::ExprRef::invalid();
      if (!arg) {
        set_error("Expected expression in aggregate function");
        return ast::ExprRef::invalid();
      }
      m_ctx.expr_pool.value_indices[m_ctx.expr_pool.value_count++] = arg.index;
      e.func_args_count = 1;
    }

    if (!expect(TokenType::Rparen, "Expected ')' after aggregate function")) {
      return ast::ExprRef::invalid();
    }
    return ref;
  }

  // ========== 表达式构建辅助 ==========
  constexpr ast::ExprRef make_binary(ast::BinaryOp op, ast::ExprRef left,
                                     ast::ExprRef right) {
    if (!left || !right)
      return ast::ExprRef::invalid();

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Binary;
    e.binary_op = op;
    e.binary_left_idx = left.index;
    e.binary_right_idx = right.index;
    return ref;
  }

  constexpr ast::ExprRef make_unary(ast::UnaryOp op, ast::ExprRef operand) {
    if (!operand)
      return ast::ExprRef::invalid();

    auto ref = m_ctx.alloc_expr();
    if (!ref) {
      set_error("Expression allocation failed");
      return ast::ExprRef::invalid();
    }

    auto &e = m_ctx.expr(ref);
    e.kind = ast::Expr::Kind::Unary;
    e.unary_op = op;
    e.unary_operand_idx = operand.index;
    return ref;
  }

  constexpr bool is_aggregate() const {
    auto t = current().type;
    return t == TokenType::Count || t == TokenType::Sum ||
           t == TokenType::Avg || t == TokenType::Max || t == TokenType::Min;
  }

  constexpr bool is_join_keyword() const {
    auto t = current().type;
    return t == TokenType::Join || t == TokenType::Left ||
           t == TokenType::Right || t == TokenType::Inner ||
           t == TokenType::Outer || t == TokenType::Cross;
  }

  // ========== SELECT 语句解析 ==========
  constexpr void parse_select(ast::SelectStmt &stmt) {
    if (!expect(TokenType::Select, "Expected SELECT"))
      return;

    if (match(TokenType::Distinct))
      stmt.distinct = true;
    else if (match(TokenType::All))
      stmt.all = true;

    parse_result_columns(stmt);
    if (!ok())
      return;

    if (match(TokenType::From)) {
      parse_from_clause(stmt);
      if (!ok())
        return;
    }

    if (match(TokenType::Where)) {
      stmt.has_where = true;
      stmt.where_expr = parse_expr();
      if (!ok())
        return;
      if (!stmt.where_expr) {
        set_error_after_prev("Expected expression after WHERE");
        return;
      }
    }

    if (match(TokenType::Group)) {
      if (!expect(TokenType::By, "Expected BY after GROUP"))
        return;
      parse_group_by(stmt);
      if (!ok())
        return;
    }

    if (match(TokenType::Having)) {
      stmt.has_having = true;
      stmt.having_expr = parse_expr();
      if (!ok())
        return;
      if (!stmt.having_expr) {
        set_error_after_prev("Expected expression after HAVING");
        return;
      }
    }

    if (match(TokenType::Order)) {
      if (!expect(TokenType::By, "Expected BY after ORDER"))
        return;
      parse_order_by(stmt);
      if (!ok())
        return;
    }

    if (match(TokenType::Limit)) {
      stmt.has_limit = true;
      stmt.limit_expr = parse_expr();
      if (!ok())
        return;
      if (!stmt.limit_expr) {
        set_error_after_prev("Expected expression after LIMIT");
        return;
      }

      if (match(TokenType::Offset)) {
        stmt.has_offset = true;
        stmt.offset_expr = parse_expr();
        if (!ok())
          return;
        if (!stmt.offset_expr) {
          set_error_after_prev("Expected expression after OFFSET");
          return;
        }
      }
    }
  }

  constexpr void parse_result_columns(ast::SelectStmt &stmt) {
    do {
      if (!ok())
        return;

      ast::ResultColumn col;

      if (check(TokenType::Identifier) && peek().type == TokenType::Dot &&
          peek(2).type == TokenType::Star) {
        col.kind = ast::ResultColumn::Kind::TableStar;
        col.table = make_id(advance());
        advance(); // .
        advance(); // *
      } else if (match(TokenType::Star)) {
        col.kind = ast::ResultColumn::Kind::Star;
      } else {
        col.kind = ast::ResultColumn::Kind::Expr;
        col.expr = parse_expr();
        if (!ok())
          return;
        if (!col.expr) {
          set_error("Expected column expression");
          return;
        }

        if (match(TokenType::As)) {
          if (!check(TokenType::Identifier)) {
            set_error("Expected alias after AS");
            return;
          }
          col.alias = make_id(advance());
        } else if (check(TokenType::Identifier) && !current().is_keyword()) {
          col.alias = make_id(advance());
        }
      }

      stmt.columns[stmt.column_count++] = col;
    } while (match(TokenType::Comma));
  }

  constexpr void parse_from_clause(ast::SelectStmt &stmt) {
    do {
      if (!ok())
        return;

      ast::SelectTableRef ref;

      if (match(TokenType::Lparen)) {
        if (!check(TokenType::Select)) {
          set_error("Expected SELECT in subquery");
          return;
        }

        ref.kind = ast::SelectTableRef::Kind::Subquery;

        auto sub_ref = m_ctx.alloc_stmt();
        if (!sub_ref) {
          set_error("Subquery allocation failed");
          return;
        }

        m_ctx[sub_ref].init_select();
        parse_select(m_ctx[sub_ref].as_select());
        if (!ok())
          return;
        ref.subquery_ref = sub_ref;

        if (!expect(TokenType::Rparen, "Expected ')' after subquery"))
          return;

        if (match(TokenType::As) || check(TokenType::Identifier)) {
          if (current().type == TokenType::As)
            advance();
          if (!check(TokenType::Identifier)) {
            set_error("Expected alias for derived table");
            return;
          }
          ref.alias = make_id(advance());
        } else {
          set_error("Derived table must have an alias");
          return;
        }
      } else {
        ref.kind = ast::SelectTableRef::Kind::Table;
        parse_table_name(ref.table_name);
        if (!ok())
          return;

        if (match(TokenType::As)) {
          if (!check(TokenType::Identifier)) {
            set_error("Expected alias after AS");
            return;
          }
          ref.alias = make_id(advance());
        } else if (check(TokenType::Identifier) && !current().is_keyword()) {
          ref.alias = make_id(advance());
        }
      }

      stmt.from_tables[stmt.from_count++] = ref;

      while (is_join_keyword()) {
        parse_join(stmt);
        if (!ok())
          return;
      }
    } while (match(TokenType::Comma));
  }

  constexpr void parse_join(ast::SelectStmt &stmt) {
    ast::SelectTableRef ref;
    ref.kind = ast::SelectTableRef::Kind::Join;

    if (match(TokenType::Left)) {
      ref.join.join_type = TokenType::Left;
      match(TokenType::Outer);
    } else if (match(TokenType::Right)) {
      ref.join.join_type = TokenType::Right;
      match(TokenType::Outer);
    } else if (match(TokenType::Inner)) {
      ref.join.join_type = TokenType::Inner;
    } else if (match(TokenType::Outer)) {
      ref.join.join_type = TokenType::Outer;
    } else if (match(TokenType::Cross)) {
      ref.join.join_type = TokenType::Cross;
    }

    if (!expect(TokenType::Join, "Expected JOIN"))
      return;

    if (match(TokenType::Lparen) && check(TokenType::Select)) {
      auto sub_ref = m_ctx.alloc_stmt();
      if (!sub_ref) {
        set_error("Subquery allocation failed");
        return;
      }

      m_ctx[sub_ref].init_select();
      parse_select(m_ctx[sub_ref].as_select());
      if (!ok())
        return;
      ref.subquery_ref = sub_ref;

      if (!expect(TokenType::Rparen, "Expected ')' after subquery"))
        return;

      if (match(TokenType::As) || check(TokenType::Identifier)) {
        if (current().type == TokenType::As)
          advance();
        ref.alias = make_id(advance());
      }
    } else {
      parse_table_name(ref.table_name);
      if (!ok())
        return;

      if (match(TokenType::As)) {
        ref.alias = make_id(advance());
      } else if (check(TokenType::Identifier) && !current().is_keyword()) {
        ref.alias = make_id(advance());
      }
    }

    if (match(TokenType::On)) {
      ref.join.on_condition = parse_expr();
      if (!ok())
        return;
      if (!ref.join.on_condition) {
        set_error_after_prev("Expected expression after ON");
        return;
      }
    }

    stmt.from_tables[stmt.from_count++] = ref;
  }

  constexpr void parse_group_by(ast::SelectStmt &stmt) {
    do {
      if (!ok())
        return;
      auto expr = parse_expr();
      if (!ok())
        return;
      if (!expr) {
        set_error("Expected expression in GROUP BY");
        return;
      }
      stmt.group_bys[stmt.group_by_count++] = expr;
    } while (match(TokenType::Comma));
  }

  constexpr void parse_order_by(ast::SelectStmt &stmt) {
    do {
      if (!ok())
        return;

      ast::OrderByItem item;
      item.expr = parse_expr();
      if (!ok())
        return;
      if (!item.expr) {
        set_error("Expected expression in ORDER BY");
        return;
      }

      if (match(TokenType::Desc)) {
        item.desc = true;
      } else {
        match(TokenType::Asc);
      }

      stmt.order_bys[stmt.order_by_count++] = item;
    } while (match(TokenType::Comma));
  }

  // ========== INSERT 语句解析 ==========
  constexpr void parse_insert(ast::InsertStmt &stmt) {
    if (!expect(TokenType::Insert, "Expected INSERT"))
      return;
    if (!expect(TokenType::Into, "Expected INTO after INSERT"))
      return;

    parse_table_name(stmt.table_name);
    if (!ok())
      return;

    if (match(TokenType::Lparen)) {
      do {
        if (!check(TokenType::Identifier)) {
          set_error("Expected column name");
          return;
        }
        stmt.columns[stmt.column_count++] = make_id(advance());
      } while (match(TokenType::Comma));

      if (!expect(TokenType::Rparen, "Expected ')' after column list"))
        return;
    }

    if (match(TokenType::Values)) {
      stmt.source = ast::InsertStmt::ValueSource::Values;
      parse_values_list(stmt);
    } else if (check(TokenType::Select)) {
      stmt.source = ast::InsertStmt::ValueSource::Select;

      auto sub_ref = m_ctx.alloc_stmt();
      if (!sub_ref) {
        set_error("Subquery allocation failed");
        return;
      }

      m_ctx[sub_ref].init_select();
      parse_select(m_ctx[sub_ref].as_select());
      if (!ok())
        return;
      stmt.select_ref = sub_ref;
    } else if (match(TokenType::Default)) {
      if (!expect(TokenType::Values, "Expected VALUES after DEFAULT"))
        return;
      stmt.source = ast::InsertStmt::ValueSource::DefaultValues;
    } else {
      set_error("Expected VALUES, SELECT, or DEFAULT VALUES");
    }
  }

  constexpr void parse_values_list(ast::InsertStmt &stmt) {
    do {
      if (!ok())
        return;

      if (!expect(TokenType::Lparen, "Expected '(' before values"))
        return;

      ast::ValueRow row;
      do {
        row.values[row.count] = parse_expr();
        if (!ok())
          return;
        if (!row.values[row.count]) {
          set_error("Expected expression in VALUES");
          return;
        }
        row.count++;
      } while (match(TokenType::Comma));

      if (!expect(TokenType::Rparen, "Expected ')' after values"))
        return;

      stmt.value_rows[stmt.row_count++] = row;
    } while (match(TokenType::Comma));
  }

  // ========== UPDATE 语句解析 ==========
  constexpr void parse_update(ast::UpdateStmt &stmt) {
    if (!expect(TokenType::Update, "Expected UPDATE"))
      return;

    parse_table_name(stmt.table_name);
    if (!ok())
      return;

    if (match(TokenType::As)) {
      if (!check(TokenType::Identifier)) {
        set_error("Expected alias after AS");
        return;
      }
      stmt.alias = make_id(advance());
    } else if (check(TokenType::Identifier) && peek().type == TokenType::Set) {
      stmt.alias = make_id(advance());
    }

    if (!expect(TokenType::Set, "Expected SET"))
      return;

    do {
      if (!ok())
        return;

      ast::UpdateSetItem item;

      if (!check(TokenType::Identifier)) {
        set_error("Expected column name in SET clause");
        return;
      }
      auto tok = advance();

      if (match(TokenType::Dot)) {
        item.column.table = make_id(tok);
        if (!check(TokenType::Identifier)) {
          set_error("Expected column name after '.'");
          return;
        }
        tok = advance();
      }
      item.column.name = make_id(tok);

      if (!expect(TokenType::Eq, "Expected '=' in SET clause"))
        return;

      item.value = parse_expr();
      if (!ok())
        return;
      if (!item.value) {
        set_error_after_prev("Expected expression in SET clause");
        return;
      }

      stmt.sets[stmt.set_count++] = item;
    } while (match(TokenType::Comma));

    if (match(TokenType::From)) {
      parse_update_from_clause(stmt);
      if (!ok())
        return;
    }

    if (match(TokenType::Where)) {
      stmt.has_where = true;
      stmt.where_expr = parse_expr();
      if (!ok())
        return;
      if (!stmt.where_expr) {
        set_error_after_prev("Expected expression after WHERE");
        return;
      }
    }

    if (match(TokenType::Order)) {
      if (!expect(TokenType::By, "Expected BY after ORDER"))
        return;
      parse_update_order_by(stmt);
      if (!ok())
        return;
    }

    if (match(TokenType::Limit)) {
      stmt.has_limit = true;
      stmt.limit_expr = parse_expr();
      if (!ok())
        return;
      if (!stmt.limit_expr) {
        set_error_after_prev("Expected expression after LIMIT");
        return;
      }
    }
  }

  constexpr void parse_update_from_clause(ast::UpdateStmt &stmt) {
    do {
      if (!ok())
        return;

      ast::SelectTableRef ref;
      ref.kind = ast::SelectTableRef::Kind::Table;

      parse_table_name(ref.table_name);
      if (!ok())
        return;

      if (match(TokenType::As)) {
        if (!check(TokenType::Identifier)) {
          set_error("Expected alias after AS");
          return;
        }
        ref.alias = make_id(advance());
      } else if (check(TokenType::Identifier) && !current().is_keyword()) {
        ref.alias = make_id(advance());
      }

      stmt.from_tables[stmt.from_count++] = ref;
    } while (match(TokenType::Comma));
  }

  constexpr void parse_update_order_by(ast::UpdateStmt &stmt) {
    do {
      if (!ok())
        return;

      ast::OrderByItem item;
      item.expr = parse_expr();
      if (!ok())
        return;
      if (!item.expr) {
        set_error("Expected expression in ORDER BY");
        return;
      }

      if (match(TokenType::Desc)) {
        item.desc = true;
      } else {
        match(TokenType::Asc);
      }

      stmt.order_bys[stmt.order_by_count++] = item;
    } while (match(TokenType::Comma));
  }

  // ========== DELETE 语句解析 ==========
  constexpr void parse_delete(ast::DeleteStmt &stmt) {
    if (!expect(TokenType::Delete, "Expected DELETE"))
      return;
    if (!expect(TokenType::From, "Expected FROM after DELETE"))
      return;

    parse_table_name(stmt.table_name);
    if (!ok())
      return;

    if (match(TokenType::As)) {
      if (!check(TokenType::Identifier)) {
        set_error("Expected alias after AS");
        return;
      }
      stmt.alias = make_id(advance());
    } else if (check(TokenType::Identifier) && !current().is_keyword()) {
      stmt.alias = make_id(advance());
    }

    if (match(TokenType::Where)) {
      stmt.has_where = true;
      stmt.where_expr = parse_expr();
      if (!ok())
        return;
      if (!stmt.where_expr) {
        set_error_after_prev("Expected expression after WHERE");
        return;
      }
    }

    if (match(TokenType::Order)) {
      if (!expect(TokenType::By, "Expected BY after ORDER"))
        return;
      parse_delete_order_by(stmt);
      if (!ok())
        return;
    }

    if (match(TokenType::Limit)) {
      stmt.has_limit = true;
      stmt.limit_expr = parse_expr();
      if (!ok())
        return;
      if (!stmt.limit_expr) {
        set_error_after_prev("Expected expression after LIMIT");
        return;
      }
    }
  }

  constexpr void parse_delete_order_by(ast::DeleteStmt &stmt) {
    do {
      if (!ok())
        return;

      ast::OrderByItem item;
      item.expr = parse_expr();
      if (!ok())
        return;
      if (!item.expr) {
        set_error("Expected expression in ORDER BY");
        return;
      }

      if (match(TokenType::Desc)) {
        item.desc = true;
      } else {
        match(TokenType::Asc);
      }

      stmt.order_bys[stmt.order_by_count++] = item;
    } while (match(TokenType::Comma));
  }

  // ========== 表名解析 ==========
  constexpr void parse_table_name(ast::QualifiedName &name) {
    if (!check(TokenType::Identifier)) {
      set_error("Expected table name");
      return;
    }

    auto tok = advance();

    if (match(TokenType::Dot)) {
      name.schema = make_id(tok);
      if (!check(TokenType::Identifier)) {
        set_error("Expected table name after '.'");
        return;
      }
      tok = advance();
    }

    name.name = make_id(tok);
  }
};

} // namespace ess::orm::sql
