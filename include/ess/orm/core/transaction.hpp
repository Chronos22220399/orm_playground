#pragma once
#include <ess/orm/common/concept.hpp>
#include <ess/orm/common/error.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/defines.hpp>
#include <ess/orm/core/impl.hpp>

namespace ess::orm::core {

template <transaction_mode Mode,
          concepts::database_type DB = config::default_db>
class Transaction {
public:
  explicit Transaction(Connection &conn) : m_conn(&conn) {}

  Transaction(Transaction const &) = delete;
  Transaction &operator=(Transaction const &) = delete;

  Transaction(Transaction &&other) noexcept
      : m_conn(other.m_conn), m_started(other.m_started),
        m_finished(other.m_finished) {
    other.m_conn = nullptr;
    other.m_started = true;
    other.m_finished = true;
  }

  Transaction &operator=(Transaction &&other) noexcept {
    if (this != &other) {
      // 通过移动构造清理自身
      Transaction dummy(std::move(*this));

      m_conn = other.m_conn;
      m_started = other.m_started;
      m_finished = other.m_finished;

      other.m_conn = nullptr;
      other.m_started = true;
      other.m_finished = true;
    }
    return *this;
  }

  ~Transaction() noexcept {
    try {
      rollback();
    } catch (std::exception const &e) {
      // log(get_cur_loc_info() +
      //                          ": rollback failed: " + e.what());
    }
  }

  void begin() {
    if (m_started)
      return;
    constexpr auto mode =
        std::is_same_v<Mode, Read> ? TxMode::READ : TxMode::WRITE;
    m_conn->begin_transaction(mode);
    m_started = true;
    m_finished = false;
  }

  void commit() {
    if (!m_started || m_finished)
      return;
    m_conn->commit();
    m_finished = true;
  }

  void rollback() {
    if (!m_started || m_finished)
      return;
    m_conn->rollback();
    m_finished = true;
  }

  int nesting_level() const { return m_conn->nesting_level(); }

  Connection &connection() const { return *m_conn; }

  // FIX: 使用 Transaction 自带的连接
  template <concepts::table_type Table, meta::FixedString SQL,
            template <typename...> class Container = std::vector,
            typename ContainerSize =
                core::ContainerSize<config::default_container_size>,
            typename... Args>
    requires std::is_same_v<typename Table::Database, DB>
  auto query(Args &&...args) {
    return impl::query_impl<core::table_tag<Table>, SQL, Container,
                            ContainerSize>(core::conn_ptr_wrapper(m_conn),
                                           std::forward<Args>(args)...);
  }

  template <meta::FixedString SQL,
            template <typename...> class Container = std::vector,
            typename ContainerSize =
                core::ContainerSize<config::default_container_size>,
            typename... Args>
  auto query(Args &&...args) {
    return impl::query_impl<core::table_tag<void>, SQL, Container,
                            ContainerSize>(core::conn_ptr_wrapper(m_conn),
                                           std::forward<Args>(args)...);
  }

private:
  Connection *m_conn = nullptr;
  bool m_started = false;
  bool m_finished = false;
};

/*
 */
template <typename Func, typename Mode, typename DB>
concept tx_callback_func = std::invocable<Func, core::Transaction<Mode, DB> &>;
} // namespace ess::orm::core
