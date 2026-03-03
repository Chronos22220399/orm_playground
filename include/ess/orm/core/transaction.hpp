#pragma once
#include <ess/orm/common_concept.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/core/connection.hpp>
#include <ess/orm/error.hpp>

namespace ess::orm::core {

struct Read {};

struct Write {};

template <typename T>
concept transaction_mode = std::is_same_v<T, Read> || std::is_same_v<T, Write>;

template <transaction_mode Mode, concepts::database_type DB> class Transaction {
public:
  explicit Transaction(Connection &conn) : m_conn(&conn) {}

  ~Transaction() {
    try {
      rollback();
    } catch (std::exception const &e) {
      throw std::runtime_error(get_cur_loc_info() +
                               ": rollback failed: " + e.what());
    }
  }

  void begin() {
    constexpr auto mode =
        std::is_same_v<Mode, Read> ? TxMode::READ : TxMode::WRITE;
    m_conn->begin_transaction(mode);
  }

  void commit() {
    m_conn->commit();
    m_finished = true;
  }

  void rollback() {
    if (m_finished)
      return;
    m_conn->rollback();
    m_finished = true;
  }

  Connection &connection() const { return *m_conn; }

  template <concepts::database_type DataBase, meta::FixedString SQL,
            typename... Args>
  auto query(Args &&...args) {}

private:
  Connection *m_conn = nullptr;
  bool m_finished = false;
};

} // namespace ess::orm::core
