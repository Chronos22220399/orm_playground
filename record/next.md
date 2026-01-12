# 纯 C++ 配置方案

完全使用 C++ 语法，用户只需定义结构体，无需使用任何宏。

## 1. 库代码结构

```
ess/
├── orm/
│   ├── dialect/
│   │   └── dialect.hpp
│   ├── config/
│   │   ├── default.hpp     # 默认配置
│   │   ├── traits.hpp      # 配置萃取
│   │   └── config.hpp      # 配置入口
│   └── query.hpp
```

---

## 2. 库代码实现

### dialect.hpp

```cpp
#pragma once
#include <concepts>

namespace ess::orm::dialect {

struct DialectTag {};
struct Sqlite : DialectTag {};
struct Postgres : DialectTag {};
struct MySQL : DialectTag {};

template <typename T>
concept dialect_type = std::derived_from<T, DialectTag>;

} // namespace ess::orm::dialect
```

### config/default.hpp（默认配置）

```cpp
#pragma once
#include <ess/orm/dialect/dialect.hpp>
#include <cstddef>
#include <chrono>

namespace ess::orm::config {

// 默认配置结构体 - 作为用户配置的参考模板
struct DefaultConfig {
    // 数据库方言
    using Dialect = dialect::Sqlite;

    // 是否启用 SQL 日志
    static constexpr bool sql_logging = false;

    // 连接池大小
    static constexpr std::size_t pool_size = 10;

    // 查询超时
    static constexpr std::chrono::milliseconds query_timeout{3000};

    // 表名前缀
    static constexpr auto table_prefix = "";
};

} // namespace ess::orm::config
```

### config/traits.hpp（配置萃取）

```cpp
#pragma once
#include <ess/orm/config/default.hpp>
#include <type_traits>
#include <concepts>

namespace ess::orm::config {

// 检测用户配置是否定义了某个成员
namespace detail {

// Dialect 检测
template <typename T>
concept has_dialect = requires {
    typename T::Dialect;
    requires dialect::dialect_type<typename T::Dialect>;
};

// sql_logging 检测
template <typename T>
concept has_sql_logging = requires {
    { T::sql_logging } -> std::convertible_to<bool>;
};

// pool_size 检测
template <typename T>
concept has_pool_size = requires {
    { T::pool_size } -> std::convertible_to<std::size_t>;
};

// query_timeout 检测
template <typename T>
concept has_query_timeout = requires {
    { T::query_timeout } -> std::convertible_to<std::chrono::milliseconds>;
};

// table_prefix 检测
template <typename T>
concept has_table_prefix = requires {
    { T::table_prefix };
};

} // namespace detail

// ============================================================
// 配置萃取器 - 自动合并用户配置与默认配置
// ============================================================

template <typename UserCfg, typename Default = DefaultConfig>
struct ConfigTraits {
    // 方言：优先使用用户配置
    using Dialect = std::conditional_t<
        detail::has_dialect<UserCfg>,
        typename UserCfg::Dialect,
        typename Default::Dialect
    >;

    // SQL 日志
    static constexpr bool sql_logging = []() {
        if constexpr (detail::has_sql_logging<UserCfg>) {
            return UserCfg::sql_logging;
        } else {
            return Default::sql_logging;
        }
    }();

    // 连接池大小
    static constexpr std::size_t pool_size = []() {
        if constexpr (detail::has_pool_size<UserCfg>) {
            return UserCfg::pool_size;
        } else {
            return Default::pool_size;
        }
    }();

    // 查询超时
    static constexpr auto query_timeout = []() {
        if constexpr (detail::has_query_timeout<UserCfg>) {
            return UserCfg::query_timeout;
        } else {
            return Default::query_timeout;
        }
    }();

    // 表名前缀
    static constexpr auto table_prefix = []() {
        if constexpr (detail::has_table_prefix<UserCfg>) {
            return UserCfg::table_prefix;
        } else {
            return Default::table_prefix;
        }
    }();
};

} // namespace ess::orm::config
```

### config/config.hpp（配置入口 - 唯一使用 `__has_include` 的地方）

```cpp
#pragma once
#include <ess/orm/config/traits.hpp>

namespace ess::orm::config {

// ============================================================
// 用户配置检测（这是唯一使用宏的地方）
// ============================================================

#if __has_include(<ess_orm_user_config.hpp>)
    #include <ess_orm_user_config.hpp>
    inline constexpr bool has_user_config = true;
#else
    // 用户未提供配置时，使用空结构体
    struct UserConfig {};
    inline constexpr bool has_user_config = false;
#endif

// ============================================================
// 最终配置（合并用户配置与默认配置）
// ============================================================

using Config = ConfigTraits<UserConfig>;

// 便捷访问别名
using Dialect = Config::Dialect;

inline constexpr bool sql_logging     = Config::sql_logging;
inline constexpr auto pool_size       = Config::pool_size;
inline constexpr auto query_timeout   = Config::query_timeout;
inline constexpr auto table_prefix    = Config::table_prefix;

} // namespace ess::orm::config
```

### query.hpp

```cpp
#pragma once
#include <ess/orm/config/config.hpp>
#include <ess/orm/dsl.hpp>
#include <fmt/core.h>
#include <fmt/color.h>

namespace ess::orm {

template <typename Table, meta::FixedString SQL,
          dialect::dialect_type Dialect = config::Dialect>
auto query(auto &&...args) {
    static_assert(dsl::is_table_type<Table>, "请使用持有 Schema 的 Table 类型");

    // 编译期 SQL 日志
    if constexpr (config::sql_logging) {
        // 编译期打印或记录
    }

    if constexpr (std::is_same_v<Dialect, dialect::Postgres>) {
        fmt::println("Postgres");
    } else if constexpr (std::is_same_v<Dialect, dialect::Sqlite>) {
        fmt::println("Sqlite");
    } else if constexpr (std::is_same_v<Dialect, dialect::MySQL>) {
        fmt::println("MySQL");
    }
}

// 打印当前配置
inline void print_config() {
    fmt::print(fmt::fg(fmt::color::cyan),
        "[ESS ORM Configuration]\n"
        "  User config:   {}\n"
        "  Dialect:       {}\n"
        "  SQL logging:   {}\n"
        "  Pool size:     {}\n"
        "  Query timeout: {}ms\n"
        "  Table prefix:  \"{}\"\n\n",
        config::has_user_config ? "detected" : "using defaults",
        std::is_same_v<config::Dialect, dialect::Postgres> ? "Postgres" :
        std::is_same_v<config::Dialect, dialect::Sqlite>   ? "Sqlite"   :
        std::is_same_v<config::Dialect, dialect::MySQL>    ? "MySQL"    : "Unknown",
        config::sql_logging,
        config::pool_size,
        config::query_timeout.count(),
        config::table_prefix
    );
}

} // namespace ess::orm
```

---

## 3. 用户配置文件

用户只需创建纯 C++ 结构体，**完全不使用宏**：

```cpp
// ess_orm_user_config.hpp
#pragma once
#include <ess/orm/dialect/dialect.hpp>
#include <chrono>

namespace ess::orm::config {

// 用户只需定义想要覆盖的配置项
// 未定义的项会自动使用默认值
struct UserConfig {
    // 使用 Postgres 方言
    using Dialect = dialect::Postgres;

    // 启用 SQL 日志
    static constexpr bool sql_logging = true;

    // 增大连接池
    static constexpr std::size_t pool_size = 50;

    // 不定义 query_timeout，将使用默认值 3000ms
    // 不定义 table_prefix，将使用默认值 ""
};

} // namespace ess::orm::config
```

用户也可以只覆盖部分配置：

```cpp
// ess_orm_user_config.hpp - 最简配置
#pragma once
#include <ess/orm/dialect/dialect.hpp>

namespace ess::orm::config {

struct UserConfig {
    using Dialect = dialect::Postgres;  // 只修改方言
};

} // namespace ess::orm::config
```

---

## 4. CMake 配置

### 库测试

```cmake
# tests/CMakeLists.txt
add_executable(test_orm test_query.cpp)
target_link_libraries(test_orm PRIVATE ess_orm fmt::fmt)

# 添加测试配置目录
target_include_directories(test_orm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)
```

```cpp
// tests/config/ess_orm_user_config.hpp
#pragma once
#include <ess/orm/dialect/dialect.hpp>

namespace ess::orm::config {

struct UserConfig {
    using Dialect = dialect::Postgres;
    static constexpr bool sql_logging = true;
};

} // namespace ess::orm::config
```

### 用户项目

```cmake
# 用户的 CMakeLists.txt
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ess_orm)

# 添加用户配置目录
target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/config
)
```

---

## 5. 运行示例

```cpp
// main.cpp
#include <ess/orm/query.hpp>

struct Goods {
    struct Schema { /* ... */ };
};

int main() {
    ess::orm::print_config();
    ess::orm::query<Goods, "SELECT * FROM goods"_fs>();
    return 0;
}
```

**输出（使用用户配置时）：**

```
[ESS ORM Configuration]
  User config:   detected
  Dialect:       Postgres
  SQL logging:   true
  Pool size:     50
  Query timeout: 3000ms
  Table prefix:  ""

Postgres
```

---

## 6. 方案优势

| 特性            | 说明                                        |
| --------------- | ------------------------------------------- |
| **纯 C++ 语法** | 用户配置完全是标准 C++ 结构体               |
| **LSP 友好**    | 类型、成员都有完整的代码补全和跳转          |
| **部分覆盖**    | 用户只需定义想修改的项，其余使用默认值      |
| **类型安全**    | concept 检测确保配置项类型正确              |
| **编译期计算**  | 所有配置都是 constexpr                      |
| **最小宏使用**  | 仅 `__has_include` 一处，用户完全无需接触宏 |
