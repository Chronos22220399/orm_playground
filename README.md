# Ess-Orm

## 说明

1. 当前本仓库为实验性仓库，由于云服务器到期，先将此仓库用于多台设备间的进步同步，当前仅做试验性的增量更新。在技术路径完整探索后，会将该仓库中的代码经处理后
   迁移至 `ess` 仓库下作为 `orm` 子项目存在。当前版本为 `version 0.0.1`，完整实现 `mvp` 并迁移后即为 `Ess-Orm version 0.1.0`

2. 当前目录下 `bench`、`scripts` 目录均为上个试验期遗留下的，现在正在重构封装的实现，因此目前仅供参考，可能无法正常使用

3. 库采用 `Header Source` 架构，不提供 `single header`，头文件用于提供接口定义、实现 `DDL` 定义与`DDL`校验、`SQl` 校验。

4. 本人当前忙于准备复试，暂时不会继续更新，复试结束后会恢复更新。

---

## 构建与安装

> 库固定使用动态库 + 系统 SQLite3，无需额外配置。

### 基本构建

```bash
# 克隆仓库
git clone git@github.com:Chronos22220399/orm_playground.git
cd orm_playground

# 创建构建目录并配置
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/your/custom/path ..

# 构建
cmake --build . -j 4

# 安装
cmake --install .
```

### 安装位置

默认安装到 `/usr/local/`：

- 头文件：`/usr/local/include/ess/orm/`
- 库文件：`/usr/local/lib/libess_orm.so`
- CMake配置：`/usr/local/lib/cmake/ess_orm/`
- pkg-config：`/usr/local/lib/pkgconfig/ess_orm.pc`

### 在项目中使用

#### CMake

```cmake
find_package(ess_orm REQUIRED)
target_link_libraries(your_target PRIVATE ess::orm)
```

#### pkg-config

```bash
pkg-config --cflags --libs ess_orm
```

#### 直接包含

```cpp
#include <ess/orm/orm.hpp>  // 主包含文件
// 或单独包含需要的头文件
#include <ess/orm/core/runtime.hpp>
```

### 版本信息

当前版本：`0.0.1` (实验性)

- 库名称：`ess_orm`
- 命名空间：`ess::orm`
- 最低C++标准：`C++20`

---

## 快速开始

### 1. 克隆与构建

```bash
# 克隆仓库
git clone git@github.com:Chronos22220399/orm_playground.git
cd orm_playground

# 创建构建目录并配置
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/your/custom/path ..

# 构建
cmake --build . -j 4

# 安装
cmake --install .
```

### 2. 用户配置

在你的项目目录中创建 `ess_orm_user_config.hpp`：

```cpp
// configs/ess_orm_user_config.hpp
#pragma once
#include <ess/orm/common/meta.hpp>
#include <ess/orm/core/dialect.hpp>

// 定义你的数据库
struct MainDB {
  static constexpr std::string_view connection_url = "./data/my.db";
  static constexpr std::size_t pool_size = 10;
};

// 配置
struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;
  using databases = std::tuple<MainDB>;
  using default_db = MainDB; // 默认使用 databases 内的第一个数据库
};
```

### 3. 项目 CMake 配置

```cmake
cmake_minimum_required(VERSION 3.21)
project(my_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# 设置库路径
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install)
find_package(ess_orm REQUIRED)

add_executable(my_app main.cpp)

# 关键：添加配置目录到 include 路径
target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/install/include
    ${CMAKE_CURRENT_SOURCE_DIR}/configs
)

target_link_libraries(my_app PRIVATE ess::orm)
```

### 4. 使用示例

```cpp
#include <ess/orm/orm.hpp>
#include <iostream>

using namespace std;
using namespace ess::orm;
using namespace ess::orm::sql;
using namespace ess::orm::dsl;
using namespace ess::orm::config;
using namespace ess::orm::attribute;

// 定义实体
struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  int stock = 0;

  using Database = default_db;
  using Schema = dsl::Schema<                             //
      "goods",                                            //
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>, //
      Field<"title", &Goods::title>,                      //
      Field<"price", &Goods::price>,                      //
      Field<"stock", &Goods::stock>                       //
      >;
};

int main() {
  // 注册数据库（必须）
  // 后续会将这里直接用 instance().init() 统一自动处理
  Context::instance().init();

  // 创建表
  auto sql = Goods::Schema::make_create_table_ddl();
  Context::instance().conn_pool().acquire()->execute_raw(sql);

  auto count = ess::orm::query<"SELECT * FROM goods">().size();

  if (count > 0)
    goto k;

  for (int i = 0; i < 10; ++i) {
    ess::orm::query<"INSERT INTO goods VALUES (?, ?, ?, ?)">(
        i, "number_" + std::to_string(i), i * 10.0, 0);
  }

k:
  // 查询
  auto goods = ess::orm::query<Goods, "SELECT * FROM goods"_sql>();
  cout << goods.size() << endl;

  for (auto &g : goods) {
    cout << "id: " << g.id << "\t" << "title: " << g.title << "\t"
         << "price: " << g.price << "\t" << "stock: " << g.stock << endl;
  }

  return 0;
}
```

### 5. 编译运行

```shell
cmake -B build
cmake --build build -j 4

./build/my_app
```

### 5. 注意事项

- 用户的配置文件通过 `-I` 路径在编译时检测，无需安装到系统
- 必须手动调用 `Context::instance().register_db<YourDB>()` 注册数据库
- 库本身不包含默认配置，配置完全由用户决定

本项目在使用CPP生态下的一些库开发了几个项目的后端后想出，在曾经的开发过程中，经常面对 ddl 定义与代码难以同步更改、使用的orm库在处理复杂查询时难用或是需要手写sql、事务处理麻烦
的情况，因此有了一下目标。

### 核心功能列表

- [x] [C++定义与DDL定义绑定机制](#1-C++定义与ddl定义绑定机制)
- [ ] [编译期校验机制](#2-编译期校验机制)
  > 已完成 `lexer`，正在做 `parser` 以及优化映射所使用的列名导出
- [x] [结果映射机制](#3-结果映射机制)
  > 已实现，暂未移植
  > 该部分仍使用原生 api，若想体验，可克隆仓库后引入 `include/ess/orm/core/` 下的 `Statement` 并自行更改为相关函数
- [x] [类型安全的配置系统](#4-类型安全的配置系统)
- [ ] [序列化与反序列化](#5-序列化与反序列化)
  > 较简单，考虑在编译期校验完成后实现
- [x] [类型安全的事务处理功能](#6-类型安全的事务处理功能)
  > 已实现，迁移中
- [ ] [无感知的缓存机制](#7-无感知的缓存机制)
  > 优先级较低，最后实现
- [ ] [基于协程的异步版本支持](#8-基于协程的异步版本支持)
  > 非刚需，version 0.2.0 或更高版本实现
- [ ] [多数据库支持](#9-多数据库支持)

### 观望列表

- 提供类似 `Django` 的表迁移机制
- 提供类似 `Mybatis Plus` 的便捷数据操纵方式
- 改进 `result set mapper` 提供的映射设计
- 添加适配器层，提供用户通过偏特化注入自定义类型的功能
- 优化 `DDL` 系统所使用的中间模版定义，降低编译时间

### 1-C++定义与ddl定义绑定机制

```cpp
enum class GoodsStatus : int { Normal = 0, Disabled, Deleted };

struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal; // enum
  bool enabled = true;

  using Database = default_db;
  using Schema = Schema<
      "goods", //
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>,
      Field<"stock", &Goods::stock, DefaultValue<0>>,
      Field<"status", &Goods::status, DefaultValue<GoodsStatus::Deleted>>,
      Field<"enabled", &Goods::enabled, DefaultValue<true>> //
      >;
};
```

> 当前支持整数、浮点数、字符串、整型枚举、bool值等基本类型，下一版本将更新 Date、DateTime 等类型，在之后的版本会在更新完 sqlite3、mysql、postgres 等数据库
> 支持的类型的交集后陆续更新不同数据库各自的特殊类型

---

### 2-编译期校验机制

1. 静态语法校验内部内容一览

```cpp
// sql literal
auto res = ess::orm::sql::query<"SELECT id, name FROM goods WHERE id > 10">();

// static analysis
constexpr LexResult<128> lex_res = Lexer(Sql).template tokenize<128>();
static_assert(lex_res.tokens[0].type == TokenType::Select);
static_assert(lex_res.tokens[1].type == TokenType::Identifier);
static_assert(lex_res.tokens[2].type == TokenType::Comma);
static_assert(lex_res.tokens[3].type == TokenType::Identifier);
static_assert(lex_res.tokens[4].type == TokenType::From);
static_assert(lex_res.tokens[5].type == TokenType::Identifier);
static_assert(lex_res.tokens[lex_res.count - 1].type == TokenType::End);
```

> 当前的编译期校验仍处于实验阶段，已完成词法分析、小部分实验性的语法分析

2. 编译期列名导出

```cpp
constexpr auto parse_res = Parser(lex_res.tokens).parse();
constexpr auto names = parse_res.column_names;

constexpr size_t pos_1 = names[0].pos;
constexpr size_t len_1 = names[0].len;
static_assert(fs_equal(fs_substr<names[0].pos, names[0].len>(Sql), "id"_fs));
```

> 该阶段后续会通过解析结果导出列列表，通过列列表将映射阶段的映射表的部分构建迁移至编译期

---

### 3-结果映射机制

```cpp
// 1. 映射到自定义类型
std::vector<Goods> goods = query<"SELECT * FROM goods WHERE id > ?">(0);

// 2. 映射为 Row
int id = row.get_if<int>("id").value();
auto title = row.get_if<std::string>("title").value();
auto price = row.get_if<double>("price").value();
auto stock = row.get_if<float>("stock").value();
auto status = row.get_if<int>("status").value();
auto enabled = row.get_if<bool>("enabled").value();
```

> 当前支持自定义类型映射，动态类型映射两种方式

---

### 4-类型安全的配置系统

```cpp
// default config
struct DefaultDB {
  // 必填
  static constexpr std::string_view connection_url = "./data/test.db";
  // sqlite3 选填，mysql、postgres 必填
  static constexpr std::string_view password = "";
  // 选填
  static constexpr std::size_t pool_size = 10;
  static constexpr std::chrono::milliseconds query_timeout{3000};
  static constexpr std::chrono::milliseconds busy_timeout{30000};
  static constexpr bool enable_sql_logging = false;
  static constexpr bool enable_wal_mode = true;
};

// user config
struct MainDB {
  static constexpr std::string_view connection_url = "./data/test.db";
};

struct LoggerDB {
  static constexpr std::string_view connection_url = "./data/test1.db";
  static constexpr std::size_t pool_size = 4;
};

struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;

  using databases = std::tuple< //
      MainDB,                   //
      LoggerDB                 //
      >;

  using default_db = MainDB;
};
```

> 用户通过定义配置来覆盖默认配置，配置会被萃取，随后由 Context 读取
> 当前不考虑不同方言多数据库的版本

---

### 5-序列化与反序列化

> 暂时未更新，后续会根据内置的萃取工具结合其他第三方库实现序列化与反序列化，并支持用户基于萃取工具自行实现序列化与反序列化。

---

### 6-类型安全的事务处理功能

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

> 事务部分当前处于迁移阶段，暂时无法使用，若想体验可查询早些时侯的提交记录

#### sqlite3 下的 nest transaction 速查表

| 外层  | 内层  |              |
| ----- | ----- | ------------ |
| Write | Write | ✅ SAVEPOINT |
| Write | Read  | ✅ 允许降级  |
| Read  | Read  | ✅ SAVEPOINT |
| Read  | Write | ❌ 禁止升级  |
| 无    | Write | ✅ 正常      |
| 无    | Read  | ✅ 正常      |

---

### 7-无感知的缓存机制

> 暂未实现，后续将提供最基础的LRU置换算法，支持用户注入自定义的置换算法

### 8-基于协程的异步版本支持

> 将于版本 version 0.2.0 完成后实现，当前仅支持同步语义

### 9-多数据库支持

- [x] Sqlite3 支持
  > 基本实现支持
- [ ] Mysql 支持
  > version 0.2.0
- [ ] Postgres 支持
  > version 0.2.0
