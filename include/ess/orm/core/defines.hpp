#pragma once
#include <type_traits>

namespace ess::orm::core {

struct Read {};

struct Write {};

template <typename T>
concept transaction_mode = std::is_same_v<T, Read> || std::is_same_v<T, Write>;

enum class TxMode { READ, WRITE };

/*
 * tag dispatch to distinct whether used table
 */
template <typename T> struct table_tag {
  using type = T;
};

template <> struct table_tag<void> {
  using type = void;
};

template <typename T>
concept table_wrapper_type = requires { typename T::type; };

template <size_t V> struct ContainerSize {
  static constexpr size_t value = V;
};

struct Connection;

struct conn_ptr_wrapper {
  Connection *m_value = nullptr;
  conn_ptr_wrapper(Connection *value) : m_value(value) {}

  conn_ptr_wrapper(conn_ptr_wrapper const &&) = delete;
  conn_ptr_wrapper &operator=(conn_ptr_wrapper const &&) = delete;

  conn_ptr_wrapper(conn_ptr_wrapper &&other) : m_value(other.m_value) {
    other.m_value = nullptr;
  }

  conn_ptr_wrapper &operator=(conn_ptr_wrapper &&other) {
    if (this != &other) {
      this->m_value = other.m_value;
      other.m_value = nullptr;
    }
    return *this;
  }
};

} // namespace ess::orm::core
