#pragma once
#include <ess/orm/core/impl.hpp>
#include <ess/orm/core/result.hpp>
#include <ess/orm/core/transaction.hpp>
#include <ess/orm/sql/concepts.hpp>
#include <functional>
#include <optional>

namespace ess::orm {
/*
 * @author: Ess
 */
// 版本1：带表类型，FixedString（不校验）
template <                                                //
    concepts::table_type Table,                           //
    meta::FixedString SQL,                                //
    concepts::database_type DB = config::default_db,      //
    template <typename...> class Container = std::vector, //
    typename ContainerSize =
        core::ContainerSize<config::default_container_size>, //
    typename... Args                                         //
    >
auto query(Args &&...args) {
  return impl::query_impl<core::table_tag<Table>, SQL, DB, Container,
                          ContainerSize>(core::conn_ptr_wrapper{nullptr},
                                         std::forward<Args>(args)...);
}

// 版本2：无表类型，FixedString（不校验）
template <                                                //
    meta::FixedString SQL,                                //
    concepts::database_type DB = config::default_db,      //
    template <typename...> class Container = std::vector, //
    typename ContainerSize =
        core::ContainerSize<config::default_container_size>, //
    typename... Args                                         //
    >
auto query(Args &&...args) {
  return impl::query_impl<core::table_tag<void>, SQL, DB, Container,
                          ContainerSize>(core::conn_ptr_wrapper{nullptr},
                                         std::forward<Args>(args)...);
}

// 版本3：带表类型，SqlParseResult（完整校验）
template <                                                //
    concepts::table_type Table,                           //
    auto ParsedSQL,                                       //
    concepts::database_type DB = config::default_db,      //
    template <typename...> class Container = std::vector, //
    typename ContainerSize =
        core::ContainerSize<config::default_container_size>, //
    typename... Args                                         //
    >
  requires sql::valid_sql_for_table<Table, decltype(ParsedSQL), sizeof...(Args)>
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
  return impl::query_impl<core::table_tag<Table>, SQLType::str(), DB, Container,
                          ContainerSize>(core::conn_ptr_wrapper{nullptr},
                                         std::forward<Args>(args)...);
}

// 版本4：无表类型，SqlParseResult（只校验占位符数量）
template <                                                //
    auto ParsedSQL,                                       //
    concepts::database_type DB = config::default_db,      //
    template <typename...> class Container = std::vector, //
    typename ContainerSize =
        core::ContainerSize<config::default_container_size>, //
    typename... Args                                         //
    >
  requires sql::valid_sql_basic<decltype(ParsedSQL), sizeof...(Args)>
auto query(Args &&...args) {
  using SQLType = decltype(ParsedSQL);
  return impl::query_impl<core::table_tag<void>, SQLType::str(), DB, Container,
                          ContainerSize>(core::conn_ptr_wrapper{nullptr},
                                         std::forward<Args>(args)...);
}

namespace impl {
using LoanType =
    std::decay_t<decltype(Context::instance().conn_pool().acquire())>;
/*
 * @author: ess
 */
template <concepts::database_type DB> struct TxState {
  static thread_local std::optional<LoanType> active_loan;
  static thread_local core::TxMode active_mode;
};

/*
 * @author: ess
 */
template <concepts::database_type DB>
thread_local std::optional<LoanType> TxState<DB>::active_loan = std::nullopt;

/*
 * @author: ess
 */
template <concepts::database_type DB>
thread_local core::TxMode TxState<DB>::active_mode = core::TxMode::WRITE;

} // namespace impl

/*
 * @author: Ess
 */
template <core::transaction_mode Mode = core::Write,
          concepts::database_type DB = config::default_db, typename Func>
  requires core::tx_callback_func<Func, Mode, DB>
auto transaction(Func &&func) {

  using ret_type = std::invoke_result_t<Func, core::Transaction<Mode, DB> &>;
  auto execute_tx = [&]() -> ret_type {
    // loan 用于根事务归还连接
    std::optional<impl::LoanType> &loan = impl::TxState<DB>::active_loan;
    bool is_root = !loan.has_value();

    constexpr auto tx_mode = std::is_same_v<Mode, core::Write>
                                 ? core::TxMode::WRITE
                                 : core::TxMode::READ;

    if constexpr (std::is_same_v<Mode, core::Write>) {
      if (!is_root && impl::TxState<DB>::active_mode == core::TxMode::READ) {
        throw std::runtime_error(
            get_cur_loc_info() +
            ": Cannot nest write transaction inside read transaction");
      }
    }

    if (is_root) {
      impl::TxState<DB>::active_mode = tx_mode;
      loan.emplace(Context::instance().conn_pool<DB>().acquire());
    }

    if (!loan.has_value()) {
      throw std::runtime_error(get_cur_loc_info() +
                               ": Cannot get a connection for transaction");
    }

    // 在最外层自动归还连接
    struct RootGuard {
      bool is_root;
      std::optional<impl::LoanType> &loan_ref;
      ~RootGuard() {
        if (is_root) {
          loan_ref.reset();
        }
      }
    } root_guard{is_root, loan};

    core::Transaction<Mode, DB> tx(*(loan.value().get()));

    tx.begin();
    try {
      if constexpr (std::is_void_v<ret_type>) {
        std::invoke(std::forward<Func>(func), tx);
        tx.commit();
        return;
      } else {
        auto result = std::invoke(std::forward<Func>(func), tx);
        tx.commit();
        return result;
      }
    } catch (...) {
      int cur_level = tx.nesting_level();
      tx.rollback();
      if (cur_level > 1) {
        throw cur_level;
      }
      throw;
    }
  };

  try {
    if constexpr (std::is_void_v<ret_type>) {
      execute_tx();
    } else {
      return execute_tx();
    }
  } catch (int level) {
    // 捕获到 int，说明内层回滚，无需处理
    // log
  } catch (...) {
    // 捕获到其他，说明是其他问题或是最外层，无需处理
    throw;
  }
}
} // namespace ess::orm
