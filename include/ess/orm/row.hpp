#pragma once
#include <std.hpp>

namespace ess::orm {

using DBValue = std::variant<int, long long, double, std::string, bool>;

class Row {
  std::unordered_map<std::string, DBValue> m_data;

public:
  struct Proxy {
    const DBValue &m_value;

    template <typename T> operator T() const {
      return std::visit(
          [](auto &&val) -> T {
            using V = std::decay_t<decltype(val)>;
            // 原本的类型
            if constexpr (std::is_same_v<T, V>) {
              return val;
              // 可强制转换的数值类型
            } else if constexpr (std::is_arithmetic_v<T> &&
                                 std::is_arithmetic_v<V>) {
              return static_cast<T>(val);
            } else if constexpr (std::is_same_v<std::string, T> &&
                                 std::is_same_v<std::string, V>) {
              return val;
            } else {
              throw std::runtime_error("Type missmatch: cannot convert");
            }
          },
          m_value);
    }

    template <typename T> T as() const { return std::get<T>(m_value); }

    const DBValue &get_variant() const { return m_value; }
  };

  Row() = default;

  Row(Row &&) noexcept = default;

  Row &operator=(Row &&) noexcept = default;

  Row(Row const &) = default;

  Row &operator=(Row const &) = default;

  Row(std::string key, DBValue value) {
    m_data.emplace(std::move(key), std::move(value));
  }

  template <typename Iter> Row(Iter beg, Iter end) : m_data(beg, end) {}

  Proxy operator[](std::string const &key) const {
    auto it = m_data.find(key);
    if (it == m_data.end()) {
      throw std::out_of_range("column not found: " + key);
    }
    return {it->second};
  }

  template <typename T> std::optional<T> get_if(std::string const &key) const {
    auto it = m_data.find(key);
    if (it != m_data.end()) {
      if (auto *p = std::get_if<T>(&it->second)) {
        return *p;
      }
    }
    return std::nullopt;
  }

  void add_column(std::string key, DBValue value) {
    m_data.emplace(std::move(key), std::move(value));
  }

  // 元数据
  size_t column_count() const noexcept { return m_data.size(); }

  bool has_column(std::string const &key) const noexcept {
    return m_data.contains(key);
  }

  bool empty() const noexcept { return m_data.empty(); }

  // 迭代器
  auto begin() noexcept { return m_data.begin(); }

  auto end() noexcept { return m_data.end(); }

  auto begin() const noexcept { return m_data.begin(); }

  auto end() const noexcept { return m_data.end(); }
};
} // namespace ess::orm
