#pragma once
#include <ess/orm/common/defines.hpp>
#include <ess/orm/common/meta.hpp>
#include <ess/orm/core/dialect.hpp>
#include <memory>
#include <string_view>

namespace ess::orm::core {

class ESS_ORM_API Statement {
public:
  virtual ~Statement() {}

  Statement(Statement const &) = delete;
  Statement &operator=(Statement const &) = delete;
  Statement(Statement &&) = default;
  Statement &operator=(Statement &&) = default;

  virtual bool next() = 0;
  virtual void reset() = 0;
  virtual void clear_bindings() = 0;

  virtual int column_count() const = 0;
  virtual int column_int(int index) const = 0;
  virtual int64_t column_int64(int index) const = 0;
  virtual double column_double(int index) const = 0;
  virtual std::string column_text(int index) const = 0;
  virtual bool column_is_null(int index) const = 0;
  virtual std::string column_name(int index) const = 0;
  virtual meta::ColumnType column_type(int index) const = 0;

  void bind_params(auto &&...args) {
    constexpr auto count = sizeof...(args);
    [&]<size_t... I>(std::index_sequence<I...>) {
      (bind_one(I + 1, args), ...);
    }(std::make_index_sequence<count>{});
  }

  class ESS_ORM_API Guard {
    Statement &m_stmt;

  public:
    explicit Guard(Statement &stmt) : m_stmt(stmt) {}
    Guard(Guard const &) = delete;
    Guard &operator=(Guard const &) = delete;

    ~Guard() {
      m_stmt.reset();
      m_stmt.clear_bindings();
    }
  };

  [[nodiscard]] Guard scope_guard() { return Guard{*this}; }

protected:
  Statement() = default;

  virtual void bind_one(int index, bool param) = 0;
  virtual void bind_one(int index, int param) = 0;
  virtual void bind_one(int index, int64_t param) = 0;
  virtual void bind_one(int index, double param) = 0;
  virtual void bind_one(int index, std::string_view param) = 0;
  virtual void bind_one(int index) = 0;
};

using StatementPtr = std::unique_ptr<Statement>;
} // namespace ess::orm::core
