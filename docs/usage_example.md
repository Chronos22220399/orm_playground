# Usage Example

本文档展示 `Ess-Orm` 当前版本下的一个最基础使用流程，包括：

- 用户配置
- 实体定义
- 初始化运行时上下文
- 创建表
- 插入数据
- 查询并映射到实体

> 以下示例基于当前实验性版本，接口后续可能仍会调整。

---

## 1. 用户配置

在你的项目目录中创建 `configs/ess_orm_user_config.hpp`：

```cpp
// configs/ess_orm_user_config.hpp
#pragma once
#include <ess/orm/common/meta.hpp>
#include <ess/orm/core/dialect.hpp>

// 定义数据库
struct MainDB {
  static constexpr std::string_view connection_url = "./data/my.db"; // 此处数据库自行创建，或是使用已有数据库的路
  static constexpr std::size_t pool_size = 10;
};

// 用户配置
struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;
  using databases = std::tuple<MainDB>;
  using default_db = MainDB;
};
```

---

## 2. 项目 CMake 配置

```cmake
cmake_minimum_required(VERSION 3.21)
project(my_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置库路径
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install)
find_package(ess_orm REQUIRED)

add_executable(my_app main.cpp)

# 添加安装头文件目录与用户配置目录
target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/install/include
    ${CMAKE_CURRENT_SOURCE_DIR}/configs
)

target_link_libraries(my_app PRIVATE ess::orm)
```

---

## 3. 完整示例

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
  // 初始化上下文
  Context::instance().init();

  // 创建表
  auto sql = Goods::Schema::make_create_table_ddl();
  Context::instance().conn_pool().acquire()->execute_raw(sql);

  auto count = ess::orm::query<"SELECT * FROM goods">().size();

  if (count > 0)
    goto k;

  // 插入测试数据
  for (int i = 0; i < 10; ++i) {
    ess::orm::query<"INSERT INTO goods VALUES (?, ?, ?, ?)">(
        i, "number_" + std::to_string(i), i * 10.0, 0);
  }

k:
  // 查询并映射为实体
  auto goods = ess::orm::query<Goods, "SELECT * FROM goods"_sql>();
  cout << goods.size() << endl;

  for (auto &g : goods) {
    cout << "id: " << g.id << "\t" << "title: " << g.title << "\t"
         << "price: " << g.price << "\t" << "stock: " << g.stock << endl;
  }

  return 0;
}
```

---

## 4. 编译运行

```bash
cmake -B build
cmake --build build -j 4
./build/my_app
```

---

## 5. 说明

上面的示例完成了以下事情：

- 定义一个 `Goods` 实体
- 通过 `Schema` 绑定表结构
- 初始化 ORM 上下文
- 自动生成并执行建表 SQL
- 插入测试数据
- 执行查询
- 将结果映射为 `std::vector<Goods>`

---

## 6. 注意事项

- 用户配置文件通过 include 路径提供，无需安装到系统目录
- 当前需要手动调用：

```cpp
Context::instance().init();
```

- 当前版本仍处于实验阶段，部分接口后续可能调整
- 示例中直接使用了原生 SQL 语句，后续会逐步完善编译期校验与映射能力

---

## 7. 下一步

你可以在此基础上继续尝试：

- 添加更多字段类型
- 使用默认值 / 主键 / 自增等属性
- 编写带 `WHERE` / `GROUP BY` / `ORDER BY` 的查询
- 尝试结果映射到动态 `Row`
- 测试编译期 SQL 校验能力

相关文档：

- [SQL 支持情况](sql_support.md)
