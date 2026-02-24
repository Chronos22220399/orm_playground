#pragma once
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/core/statement.h>
#include <string_view>

namespace ess::orm::core {

// class ESS_ORM_API ConnectionPool;

class ESS_ORM_API Connection {
protected:
  Connection() = default;

public:
  Connection(Connection const &) = delete;
  Connection &operator=(Connection const &) = delete;

  virtual ~Connection() = default;

  [[nodiscard]] virtual StatementPtr prepare(std::string_view sql) = 0;
  [[nodiscard]] virtual Statement &prepare_cached(std::string_view sql) = 0;

  // transaction
  virtual void begin_transaction() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;

  virtual bool is_open() = 0;

  void execute_raw(std::string_view sql, auto &&...args) {
    auto stmt = prepare(sql);
    if constexpr (sizeof...(args) > 0) {
      stmt->bind_params(args...);
    }
    stmt->next();
  }
};

using ConnectionPtr = std::unique_ptr<Connection>;
} // namespace ess::orm::core
