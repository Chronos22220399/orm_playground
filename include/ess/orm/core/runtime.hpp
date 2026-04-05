#pragma once
#include <ess/orm/core/impl.hpp>
#include <ess/orm/core/result.hpp>
#include <ess/orm/core/transaction.hpp>
#include <functional>
#include <optional>

namespace ess::orm {
/*
 * @author: Ess
 */
template <concepts::table_type Table, meta::FixedString SQL,
          template <typename...> class Container = std::vector,
          typename ContainerSize =
              core::ContainerSize<config::default_container_size>,
          typename... Args>
auto query(Args &&...args) {
  return impl::query_impl<core::table_tag<Table>, SQL, Container,
                          ContainerSize>(core::conn_ptr_wrapper{nullptr},
                                         std::forward<Args>(args)...);
}

/*
 * @author: ess
 */
template <meta::FixedString SQL,
          template <typename...> class Container = std::vector,
          typename ContainerSize =
              core::ContainerSize<config::default_container_size>,
          typename... Args>
auto query(Args &&...args) {
  return impl::query_impl<core::table_tag<void>, SQL, Container, ContainerSize>(
      core::conn_ptr_wrapper{nullptr}, std::forward<Args>(args)...);
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
      loan.emplace(Context::instance().conn_pool().acquire());
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
