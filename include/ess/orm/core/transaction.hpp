#pragma once
#include <ess/orm/common/concept.hpp>
#include <ess/orm/common/error.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/defines.hpp>
#include <ess/orm/core/impl.hpp>
#include <ess/orm/sql/concepts.hpp>

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

  // 版本1：带表类型，FixedString（不校验，向后兼容）
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

  // 版本2：带表类型，SqlParseResult（完整校验）
  template <concepts::table_type Table, auto ParsedSQL,
            template <typename...> class Container = std::vector,
            typename ContainerSize =
                core::ContainerSize<config::default_container_size>,
            typename... Args>
    requires std::is_same_v<typename Table::Database, DB> &&
             sql::valid_sql_for_table<Table, decltype(ParsedSQL),
                                      sizeof...(Args)>
  auto query(Args &&...args) {
    // MARK: 带表类型查询的限制（重要更新）
    // query<Goods, SQL>只能映射Goods表的字段，因此：
    //
    // 必须禁止：
    // 1. 列别名（AS或隐式）→ 列名改变
    // 2. 表达式列 → 列名是表达式文本
    // 3. 函数调用 → 列名是函数文本
    // 4. 聚合函数 → 列名是函数文本
    // 5. 字面量 → 列名是字面量
    // 6. JOIN查询（除非能证明只包含Goods表的列）
    // 7. 无表限定的*在JOIN中
    //
    // 问题：编译期难以验证JOIN查询是否只包含主表列
    // 选项：
    // A. 完全禁止JOIN（简单安全）
    // B. 允许JOIN但运行时检查（灵活但有运行时错误风险）
    // C. 编译期分析列来源（复杂但最理想）
    using SQLType = decltype(ParsedSQL);
    return impl::query_impl<core::table_tag<Table>, SQLType::str(), Container,
                            ContainerSize>(core::conn_ptr_wrapper(m_conn),
                                           std::forward<Args>(args)...);
  }

  // 版本3：无表类型，FixedString（不校验，向后兼容）
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

  // 版本4：无表类型，SqlParseResult（只校验占位符数量）
  template <auto ParsedSQL,
            template <typename...> class Container = std::vector,
            typename ContainerSize =
                core::ContainerSize<config::default_container_size>,
            typename... Args>
    requires sql::valid_sql_basic<decltype(ParsedSQL), sizeof...(Args)>
  auto query(Args &&...args) {
    using SQLType = decltype(ParsedSQL);
    return impl::query_impl<core::table_tag<void>, SQLType::str(), Container,
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
