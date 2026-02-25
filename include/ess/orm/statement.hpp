#pragma once
#include <ess/orm/defines.hpp>
#include <ess/orm/sql_destroier.hpp>
#include <memory>
#include <string_view>

namespace ess::orm {

class ESS_ORM_API Connection;

class ESS_ORM_API Statement {
  using StatPtr = std::unique_ptr<sqlite3_stmt, SqliteDestroier>;
  using ExpanedSqlPtr = std::unique_ptr<char, SqliteDestroier>;
  // 通过 weak_ptr 保证 m_stmt 在 sqlite3* 之前被释放（Statement 在 Connection
  // 之前）
  std::weak_ptr<Connection> m_conn_ref;
  StatPtr m_stmt;

  class ESS_ORM_API StatementGuard {
    Statement &m_stmt;

  public:
    explicit StatementGuard(Statement &stmt) : m_stmt(stmt) {}

    StatementGuard(StatementGuard const &) = delete;

    StatementGuard &operator=(StatementGuard const &) = delete;

    ~StatementGuard();
  };

public:
  Statement() = default;

  Statement(std::shared_ptr<Connection> conn, std::string_view sql);

  Statement(Statement const &) = delete;

  Statement(Statement &&other) noexcept = default;

  Statement &operator=(Statement &&other) noexcept = default;

  // 在cpp中实现，确保调用方使用Statement后能够正确析构
  ~Statement();

  sqlite3_stmt *get() const { return m_stmt.get(); }

  sqlite3 *get_db_handle() const;

  void prepare(sqlite3 *db, std::string_view sql);

  void bind_params(auto &&...args) {
    constexpr auto count = sizeof...(args);
    [&]<size_t... I>(std::index_sequence<I...>) {
      (bind_one(I + 1, args), ...);
    }(std::make_index_sequence<count>{});

    // TODO: debug 模式下使用
    // auto expanded = expanded_sql();
    // fmt::println("SQL: {}", expanded);
  }

  std::string expanded_sql();

  void reset();

  void clear_bindings();

  bool next();

  [[nodiscard]] StatementGuard scope_guard() { return StatementGuard{*this}; }

private:
  void bind_one(int index, bool param);

  void bind_one(int index, int param);

  void bind_one(int index, int64_t param);

  void bind_one(int index, double param);

  void bind_one(int index, std::string_view param);

  void bind_one(int index, std::string const &param);

  void bind_one(int index, const char *param);

  void bind_one(int index);
};
} // namespace ess::orm
