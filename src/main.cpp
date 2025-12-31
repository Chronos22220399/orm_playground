#include <core.hpp>

namespace meta {
template <size_t N> struct FixedString {
  char m_str[N];

  constexpr FixedString(const char (&str)[N]) {
    for (int i = 0; i < N; ++i)
      m_str[i] = str[i];
  }

  constexpr char &operator[](size_t idx) {
    assert(idx < N && "index out of range");
    return m_str[idx];
  }

  template <size_t Idx> constexpr const char &get() const {
    static_assert(Idx < N, "index out of range");
    return m_str[Idx];
  }

  constexpr size_t size() const { return std::size(m_str); }
};
} // namespace meta

namespace concepts {
template <typename T>
concept is_sql_literal = std::is_integral_v<T> || std::is_floating_point_v<T> ||
                         std::is_same_v<T, bool> || std::is_enum_v<T>;
}

namespace detail {
struct AttributeTag {};
} // namespace detail

struct PrimaryKey : detail::AttributeTag {};

struct Unique : detail::AttributeTag {};

struct AutoIncrement : detail::AttributeTag {};

struct NotNull : detail::AttributeTag {};

template <auto Value>
  requires concepts::is_sql_literal<decltype(Value)>
struct DefaultValue {
  static constexpr auto value = Value;
};

template <meta::FixedString Expr> struct DefaultExpr {
  static_assert(                                          //
      Expr.size() > 1 &&                                  // not null
          Expr.template get<0>() != '\'' &&               //
          Expr.template get<0>() != '\"' &&               //
          Expr.template get<Expr.size() - 2>() != '\'' && //
          Expr.template get<Expr.size() - 2>() != '\"',   //
      "Default Expression must be a SQL expressoin, not a string literal" //
  );
  static constexpr auto value = Expr;
};

template <meta::FixedString Name> struct SerializedName : detail::AttributeTag {
  static constexpr meta::FixedString value = Name;
};

int main() {
  auto expr = DefaultExpr<"now()">::value;
  return 0;
}
