# Ess-Orm

一个实验性的 C++20 ORM 项目，当前以 **SQLite3 + 动态库** 为主，目标是提供：

- C++ 类型定义与 DDL 绑定
- 编译期 SQL 校验
- 类型安全的结果映射
- 类型安全的事务处理
- 可扩展的配置系统

> 当前仓库为实验性仓库，版本为 **0.0.1**。  
> 在技术路径稳定、MVP 完整后，将整理并迁移至 `ess` 仓库下作为 `orm` 子项目继续维护。

---

## 目录

- [项目状态](#项目状态)
- [文档导航](#文档导航)
- [当前特性概览](#当前特性概览)
- [当前支持的 SQL 范围](#当前支持的-sql-范围)
- [构建与安装](#构建与安装)
  - [基本构建](#基本构建)
  - [默认安装位置](#默认安装位置)
- [在项目中使用](#在项目中使用)
  - [CMake](#cmake)
  - [pkg-config](#pkg-config)
  - [直接包含](#直接包含)
- [版本信息](#版本信息)
- [快速开始](#快速开始)
  - [1. 用户配置](#1-用户配置)
  - [2. 项目 CMake 配置](#2-项目-cmake-配置)
  - [3. 完整示例](#3-完整示例)
- [性能基准测试](#性能基准测试)
- [注意事项](#注意事项)
- [设计目标](#设计目标)
- [核心功能列表](#核心功能列表)
  - [1. C++ 定义与 DDL 定义绑定机制](#1-c-定义与-ddl-定义绑定机制)
  - [2. 编译期校验机制](#2-编译期校验机制)
  - [3. 结果映射机制](#3-结果映射机制)
  - [4. 类型安全的配置系统](#4-类型安全的配置系统)
  - [5. 序列化与反序列化](#5-序列化与反序列化)
  - [6. 类型安全的事务处理功能](#6-类型安全的事务处理功能)
  - [7. 无感知缓存机制](#7-无感知缓存机制)
  - [8. 基于协程的异步版本支持](#8-基于协程的异步版本支持)
  - [9. 多数据库支持](#9-多数据库支持)
- [观望列表](#观望列表)
- [补充文档](#补充文档)

---

## 项目状态

- 当前处于 **实验 / 重构 / 迁移阶段**
- 当前重点在于：
  - DDL 绑定机制
  - 编译期 SQL 词法 / 语法校验
  - 运行时查询与结果映射能力迁移
- 当前 `bench`、`scripts` 目录为上一试验阶段遗留内容，仅供参考
- 近期由于个人在准备复试，更新频率会较低，复试结束后恢复

---

## 文档导航

- [使用示例](docs/usage_example.md)
- [SQL 支持情况](docs/sql_support.md)

---

## 当前特性概览

### 已完成 / 基本可用

- [x] C++ 定义与 DDL 定义绑定
- [x] 类型安全的配置系统
- [x] 结果映射机制（已实现，部分迁移中）
- [x] 类型安全的事务处理功能（已实现，迁移中）
- [x] 编译期 SQL `Lexer`
- [x] 实验性编译期 SQL `Parser`
- [x] `SELECT` 查询的部分静态语法分析

### 进行中

- [ ] 编译期 SQL 校验机制完善
- [ ] 编译期列信息导出
- [ ] 结果映射模块迁移整理
- [ ] 序列化与反序列化
- [ ] 无感知缓存机制

### 后续计划

- [ ] 多数据库支持
- [ ] 协程异步版本
- [ ] 更完整的 SQL 支持
- [ ] 更强的语义校验
- [ ] 更便捷的数据操纵方式

---

## 当前支持的 SQL 范围

当前 parser 主要面向 `SELECT` 语句，已支持一部分常用子集，例如：

- `SELECT * FROM goods`
- `SELECT id, title FROM goods WHERE id > ?`
- `SELECT * FROM goods WHERE id IN (SELECT id FROM goods)`
- `SELECT * FROM goods GROUP BY id HAVING COUNT(id) > ?`
- `SELECT * FROM goods ORDER BY id DESC`

完整支持列表见：

- [SQL 支持情况](docs/sql_support.md)

---

## 构建与安装

> 当前固定使用 **动态库 + 系统 SQLite3**。

### 基本构建

```bash
git clone git@github.com:Chronos22220399/orm_playground.git
cd orm_playground

mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/your/custom/path ..
cmake --build . -j 4
cmake --install .
```

### 默认安装位置

默认安装到 `/usr/local/`：

- 头文件：`/usr/local/include/ess/orm/`
- 库文件：`/usr/local/lib/libess_orm.so`
- CMake 配置：`/usr/local/lib/cmake/ess_orm/`
- pkg-config：`/usr/local/lib/pkgconfig/ess_orm.pc`

---

## 在项目中使用

### CMake

```cmake
find_package(ess_orm REQUIRED)
target_link_libraries(your_target PRIVATE ess::orm)
```

### pkg-config

```bash
pkg-config --cflags --libs ess_orm
```

### 直接包含

```cpp
#include <ess/orm/orm.hpp>
// 或按需包含
#include <ess/orm/core/runtime.hpp>
```

---

## 版本信息

- 当前版本：`0.0.1`
- 库名称：`ess_orm`
- 命名空间：`ess::orm`
- 最低 C++ 标准：`C++20`

---

## 快速开始

### 1. 用户配置

在你的项目中创建 `configs/ess_orm_user_config.hpp`：

```cpp
#pragma once
#include <ess/orm/common/meta.hpp>
#include <ess/orm/core/dialect.hpp>

struct MainDB {
  static constexpr std::string_view connection_url = "./data/my.db";
  static constexpr std::size_t pool_size = 10;
};

struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;
  using databases = std::tuple<MainDB>;
  using default_db = MainDB;
};
```

### 2. 项目 CMake 配置

```cmake
cmake_minimum_required(VERSION 3.21)
project(my_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install)
find_package(ess_orm REQUIRED)

add_executable(my_app main.cpp)

target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/install/include
    ${CMAKE_CURRENT_SOURCE_DIR}/configs
)

target_link_libraries(my_app PRIVATE ess::orm)
```

### 3. 完整示例

完整示例请查看：

- [使用示例](docs/usage_example.md)

---

## 性能基准测试

本项目包含完整的性能基准测试套件，用于比较ORM与原生SQLite3 API的性能差异。

### 快速运行基准测试

使用 `run.sh` 脚本快速运行各种基准测试：

```bash
# 运行所有基准测试
./run.sh benchmark all

# 运行特定类型的基准测试
./run.sh benchmark basic      # 基础测试（原始版本）
./run.sh benchmark fair       # 公平测试（优化对比）
./run.sh benchmark comp       # 全面测试（不同数据规模）
./run.sh benchmark file       # 文件数据库测试

# 分析已有结果
./run.sh benchmark analyze
```

### 基准测试类型

1. **基础基准测试** (`basic`): 最初的基准测试版本，包含基本的CRUD操作测试
2. **公平基准测试** (`fair`): 优化后的公平对比测试，SQLite3使用预编译语句和性能优化
3. **全面基准测试** (`comp`): 测试不同数据规模（10, 100, 1000, 10000条记录）的性能
4. **文件数据库基准测试** (`file`): 使用文件数据库（非内存数据库）的性能测试

### 性能结果摘要

#### 内存数据库性能开销（ORM vs SQLite3）
- **批量插入**: 平均 28.1% 开销
- **批量查询**: 平均 18.5% 开销  
- **批量更新**: 平均 41.6% 开销
- **批量删除**: 平均 34.9% 开销

#### 文件数据库 vs 内存数据库
- **插入操作**: 文件数据库比内存数据库慢 5-10倍
- **查询操作**: 文件数据库比内存数据库慢 2-5倍
- **更新/删除**: 文件数据库比内存数据库慢 3-8倍

### 关键发现
1. **ORM在查询操作上表现最佳**：开销最小（14-23%）
2. **批量效应明显**：随着批量增大，每项操作成本下降
3. **文件数据库I/O影响大**：小批量操作受磁盘I/O影响显著
4. **ORM开销稳定**：在不同场景下保持合理的性能开销

### 详细文档
更多详细信息请查看 [bench/BENCHMARK_README.md](bench/BENCHMARK_README.md)

## 注意事项

- 用户配置文件无需安装到系统全局目录。在编译时，该文件可通过 CMake 的包含目录（Include Directories）进行指定；若使用原生编译指令，亦可通过 -I /your/path/ess_orm_user_config 参数将配置路径注入编译器。
- 当前需要显式初始化运行时上下文：

```cpp
Context::instance().init();
```

- 当前版本仍处于实验阶段，部分接口后续可能调整
- 库本身不内置固定默认业务配置，配置完全由用户定义

---

## 设计目标

这个项目来源于实际后端开发中的一些痛点：

- DDL 定义和 C++ 类型定义难以长期保持同步
- 复杂查询场景下，纯 ORM 表达能力不足，最终仍需回退手写 SQL
- 事务控制容易分散且缺少类型约束
- 运行时错误偏多，希望尽可能把一部分问题提前到编译期暴露

因此当前项目希望在以下方向上做探索：

- 用 C++ 类型系统绑定 DDL
- 保留手写 SQL 的灵活性
- 对 SQL 做编译期词法 / 语法校验
- 在运行时维持轻量、可控的 ORM 能力

---

## 核心功能列表

### 1. C++ 定义与 DDL 定义绑定机制

- [x] 已实现基础能力

```cpp
enum class GoodsStatus : int { Normal = 0, Disabled, Deleted };

struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal;
  bool enabled = true;

  using Database = default_db;
  using Schema = Schema<
      "goods",
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>,
      Field<"stock", &Goods::stock, DefaultValue<0>>,
      Field<"status", &Goods::status, DefaultValue<GoodsStatus::Deleted>>,
      Field<"enabled", &Goods::enabled, DefaultValue<true>>
      >;
};
```

> 当前支持整数、浮点数、字符串、整型枚举、`bool` 等基础类型。
> 后续会继续扩展日期时间类型及不同数据库方言下的类型交集。

---

### 2. 编译期校验机制

- [ ] 进行中

当前已完成：

- [x] `Lexer`
- [x] 实验性 `Parser`
- [x] 部分 `SELECT` 查询语法校验
- [x] 基础列名导出

示例：

```cpp
constexpr LexResult<128> lex_res = Lexer(Sql).template tokenize<128>();
static_assert(lex_res.tokens[0].type == TokenType::Select);

constexpr auto parse_res = Parser(lex_res.tokens).parse();
constexpr auto names = parse_res.column_names;
```

完整支持情况请见：

- [SQL 支持情况](docs/sql_support.md)

---

### 3. 结果映射机制

- [x] 已实现，整理迁移中

```cpp
std::vector<Goods> goods = query<"SELECT * FROM goods WHERE id > ?">(0);

// Row 形式
int id = row.get_if<int>("id").value();
auto title = row.get_if<std::string>("title").value();
```

> 当前支持自定义类型映射与动态行映射两种方式。

---

### 4. 类型安全的配置系统

- [x] 已实现

```cpp
struct MainDB {
  static constexpr std::string_view connection_url = "./data/test.db";
};

struct LoggerDB {
  static constexpr std::string_view connection_url = "./data/test1.db";
  static constexpr std::size_t pool_size = 4;
};

struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;
  using databases = std::tuple<MainDB, LoggerDB>;
  using default_db = MainDB;
};
```

---

### 5. 序列化与反序列化

- [ ] 未完成

> 后续会基于现有萃取工具实现，并保留用户自定义扩展能力。

---

### 6. 类型安全的事务处理功能

- [x] 已实现，迁移中

```cpp
transaction<Write>([](auto &txs) {
  transaction<Read, LoggerDB>([](auto &tx) {
    auto res = tx.template query<Log, "SELECT * FROM log">();
    for (auto &l : res) {
      std::cout << l.id << std::endl;
    }
  });

  transaction<Write>([](auto &tx) {
    auto res = tx.template query_rows<Goods, "SELECT * FROM goods">();
    for (auto &g : res) {
      std::cout << g["id"].template as<int>() << std::endl;
    }
  });
});
```

SQLite3 下的嵌套事务规则：

| 外层  | 内层  | 行为         |
| ----- | ----- | ------------ |
| Write | Write | ✅ SAVEPOINT |
| Write | Read  | ✅ 允许降级  |
| Read  | Read  | ✅ SAVEPOINT |
| Read  | Write | ❌ 禁止升级  |
| 无    | Write | ✅ 正常      |
| 无    | Read  | ✅ 正常      |

> 外层 `Read`，内层 `Write` 时若未设置 `busy_timeout` 会导致死锁

---

### 7. 无感知缓存机制

- [ ] 未完成

> 后续计划先提供基础 LRU，再考虑可替换策略。

---

### 8. 基于协程的异步版本支持

- [ ] 未完成

> 计划在 `0.2.0` 或之后的版本探索。

---

### 9. 多数据库支持

- [x] SQLite3
- [ ] MySQL
- [ ] PostgreSQL

---

## 观望列表

- 提供类似 Django 的迁移机制
- 提供类似 MyBatis Plus 的便捷数据操纵方式
- 改进结果映射设计
- 提供自定义类型适配注入层
- 优化 DDL 模板中间结构，降低编译时间

---

## 补充文档

- [使用示例](docs/usage_example.md)
- [SQL 支持情况](docs/sql_support.md)
