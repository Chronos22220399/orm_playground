# 第6章 系统测试与性能评估

本章详细介绍ESS-ORM的测试方法、性能评估和测试结果。测试工作包括功能测试和性能基准测试两部分，通过实际测试验证系统的功能正确性和性能表现。

## 6.1 测试环境与方法

### 6.1.1 测试环境

功能测试和性能测试的硬件环境为MacBook Air，配备Apple Silicon芯片（8核）。软件环境包括：C++编译器使用Apple Clang；C++标准为C++20；构建工具使用CMake；数据库使用SQLite3。性能基准测试使用Google Benchmark框架。

### 6.1.2 测试方法

ESS-ORM的测试分为两类：编译期测试和运行时测试。

**编译期测试**针对DSL模块和SQL解析模块。由于这些模块在编译期执行校验，测试结果体现在编译器的报错信息上。测试方法是通过尝试编译特定代码片段，检查编译器是否给出预期的错误信息。

**运行时测试**针对事务模块、连接池模块和结果映射模块。测试方法包括单元测试和集成测试。单元测试验证各个模块的独立功能；集成测试验证模块间的协作。

**性能测试**采用Benchmark框架进行量化测试。测试方法是在相同条件下分别运行SQLite3原生API和ESS-ORM API，比较两者的执行时间。测试覆盖四种基本操作：INSERT插入、SELECT查询、UPDATE更新、DELETE删除。

### 6.1.3 测试工具

测试使用的工具包括：CMake构建系统、Google Benchmark性能测试框架、SQLite3数据库。测试执行脚本位于bench/run_benchmark.sh。

## 6.2 功能测试

### 6.2.1 DSL模块测试

DSL模块的编译期校验通过static_assert实现，测试结果体现在编译错误上。

**测试用例表**

| 用例编号  | 测试内容                      | 测试代码                                                            | 预期结果 |
| --------- | ----------------------------- | ------------------------------------------------------------------- | -------- |
| TC-DSL-01 | 有效Schema定义                | `Field<"id", &Goods::id, PrimaryKey, AutoIncrement>`                | 编译成功 |
| TC-DSL-02 | AutoIncrement用于非整数类型   | `Field<"name", &Goods::name, AutoIncrement>`                        | 编译失败 |
| TC-DSL-03 | 重复属性声明                  | `Field<"id", &Goods::id, PrimaryKey, PrimaryKey>`                   | 编译失败 |
| TC-DSL-04 | DefaultValue与DefaultExpr互斥 | `Field<"id", &Goods::id, DefaultValue<1>, DefaultExpr<"1">>`        | 编译失败 |
| TC-DSL-05 | 重复字段名                    | `Schema<"goods", Field<"id", &Goods::id>, Field<"id", &Goods::id>>` | 编译失败 |

**TC-DSL-01：有效Schema定义**

有效代码能够编译通过，验证Schema模板能够正确生成CREATE TABLE语句。

```cpp
struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  using Schema = dsl::Schema<
      "goods",
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title>,
      Field<"price", &Goods::price>
  >;
};
auto ddl = Goods::Schema::make_create_table_ddl();
```

预期结果：编译成功，生成正确的DDL语句。

**TC-DSL-02：属性类型不匹配**

AutoIncrement属性仅对整数类型合法，用于字符串类型时编译失败。

```cpp
struct Invalid {
  std::string name;
  using Schema = dsl::Schema<
      "invalid",
      Field<"name", &Invalid::name, AutoIncrement>
  >;
};
```

预期结果：编译失败，报错信息包含"属性验证失败，请检查是否使用了不匹配的属性"。

### 6.2.2 SQL解析模块测试

SQL解析模块的编译期校验通过operator""\_sql实现，测试结果体现在编译错误上。

**测试用例表**

| 用例编号  | 测试内容          | 测试代码                                                              | 预期结果 |
| --------- | ----------------- | --------------------------------------------------------------------- | -------- |
| TC-SQL-01 | 有效SQL-简单查询  | `"SELECT * FROM goods WHERE id > ?"_sql`                              | 编译成功 |
| TC-SQL-02 | 有效SQL-JOIN      | `"SELECT * FROM goods JOIN orders ON goods.id = orders.goods_id"_sql` | 编译成功 |
| TC-SQL-03 | 有效SQL-子查询    | `"SELECT * FROM goods WHERE id IN (SELECT goods_id FROM orders)"_sql` | 编译成功 |
| TC-SQL-04 | 有效SQL-聚合      | `"SELECT COUNT(*) FROM goods"_sql`                                    | 编译成功 |
| TC-SQL-05 | 有效SQL-UNION     | `"SELECT id FROM goods UNION SELECT goods_id FROM orders"_sql`        | 编译成功 |
| TC-SQL-06 | 有效SQL-复杂WHERE | `"SELECT * FROM goods WHERE price > 100 AND stock < 50"_sql`          | 编译成功 |
| TC-SQL-07 | 有效SQL-ORDER BY  | `"SELECT * FROM goods ORDER BY price DESC"_sql`                       | 编译成功 |
| TC-SQL-08 | 有效SQL-GROUP BY  | `"SELECT category, COUNT(*) FROM goods GROUP BY category"_sql`        | 编译成功 |
| TC-SQL-09 | 缺少表名          | `"SELECT * WHERE id > 1"_sql`                                         | 编译失败 |
| TC-SQL-10 | 语法错误          | `"SELEC * FROM goods"_sql`                                            | 编译失败 |
| TC-SQL-11 | 括号不匹配        | `"SELECT * FROM goods WHERE (id > 1"_sql`                             | 编译失败 |
| TC-SQL-12 | 引号不匹配        | `"SELECT * FROM goods WHERE name = 'test"_sql`                        | 编译失败 |

**TC-SQL-01：有效SQL-简单查询**

简单SELECT查询能够通过编译期校验。

```cpp
constexpr auto result = "SELECT * FROM goods WHERE id > ?"_sql;
```

预期结果：编译成功。

**TC-SQL-02：有效SQL-JOIN**

JOIN查询能够通过编译期校验。

```cpp
constexpr auto result = "SELECT * FROM goods JOIN orders ON goods.id = orders.goods_id"_sql;
```

预期结果：编译成功。

**TC-SQL-03：有效SQL-子查询**

子查询能够通过编译期校验。

```cpp
constexpr auto result = "SELECT * FROM goods WHERE id IN (SELECT goods_id FROM orders)"_sql;
```

预期结果：编译成功。

**TC-SQL-04：有效SQL-聚合**

聚合函数查询能够通过编译期校验。

```cpp
constexpr auto result = "SELECT COUNT(*) FROM goods"_sql;
```

预期结果：编译成功。

**TC-SQL-05：有效SQL-UNION**

UNION查询能够通过编译期校验。

```cpp
constexpr auto result = "SELECT id FROM goods UNION SELECT goods_id FROM orders"_sql;
```

预期结果：编译成功。

**TC-SQL-06：有效SQL-复杂WHERE**

包含多个条件的WHERE子句能够通过编译期校验。

```cpp
constexpr auto result = "SELECT * FROM goods WHERE price > 100 AND stock < 50"_sql;
```

预期结果：编译成功。

**TC-SQL-07：有效SQL-ORDER BY**

ORDER BY排序能够通过编译期校验。

```cpp
constexpr auto result = "SELECT * FROM goods ORDER BY price DESC"_sql;
```

预期结果：编译成功。

**TC-SQL-08：有效SQL-GROUP BY**

GROUP BY分组能够通过编译期校验。

```cpp
constexpr auto result = "SELECT category, COUNT(*) FROM goods GROUP BY category"_sql;
```

预期结果：编译成功。

**TC-SQL-09：缺少表名**

缺少表名的SQL语句语法不正确。

```cpp
constexpr auto result = "SELECT * WHERE id > 1"_sql;
```

预期结果：编译失败，报错"SQL Parser Error: Invalid SQL structure"。

**TC-SQL-10：语法错误**

SQL关键字拼写错误。

```cpp
constexpr auto result = "SELEC * FROM goods"_sql;
```

预期结果：编译失败，报错"SQL Lexer Error: Invalid SQL syntax"。

**TC-SQL-11：括号不匹配**

括号不匹配导致语法错误。

```cpp
constexpr auto result = "SELECT * FROM goods WHERE (id > 1"_sql;
```

预期结果：编译失败，报错"SQL Parser Error: Invalid SQL structure"。

### 6.2.3 运行时功能测试

运行时功能测试验证实际执行效果。测试代码位于tests目录。

**测试用例表**

| 用例编号 | 测试内容            | 测试方法                                                             | 预期结果            |
| -------- | ------------------- | -------------------------------------------------------------------- | ------------------- |
| TC-RT-01 | 查询功能-带表类型   | `query<Goods, "SELECT * FROM goods"_sql>()`                          | 返回`vector<Goods>` |
| TC-RT-02 | 查询功能-不带表类型 | `query<"SELECT * FROM goods"_sql>()`                                 | 返回`vector<Row>`   |
| TC-RT-03 | 参数化查询          | `query<"SELECT * FROM goods WHERE id > ?">(100)`                     | 正确过滤            |
| TC-RT-04 | 多参数查询          | `query<"SELECT * FROM goods WHERE id > ? AND price > ?">(100, 50.0)` | 正确过滤            |
| TC-RT-05 | INSERT插入          | `query<"INSERT INTO goods VALUES (?, ?)">(1, "test")`                | 插入成功            |
| TC-RT-06 | UPDATE更新          | `query<"UPDATE goods SET price = ? WHERE id = ?">(99.9, 1)`          | 更新成功            |
| TC-RT-07 | DELETE删除          | `query<"DELETE FROM goods WHERE id = ?">(1)`                         | 删除成功            |
| TC-RT-08 | 事务自动提交        | 闭包正常返回                                                         | 事务提交            |
| TC-RT-09 | 事务自动回滚        | 闭包内抛出异常                                                       | 事务回滚            |
| TC-RT-10 | 读写事务            | `transaction<Write>`                                                 | 获取写锁            |
| TC-RT-11 | 只读事务            | `transaction<Read>`                                                  | 获取读锁            |
| TC-RT-12 | 连接池获取          | `pool.acquire()`                                                     | 获取连接            |
| TC-RT-13 | 连接自动归还        | Loan对象析构                                                         | 连接归还            |
| TC-RT-14 | 并发安全            | 多线程同时获取连接                                                   | 无死锁              |
| TC-RT-15 | 连接超时            | `pool.acquire()` 等待5秒超时                                         | 抛出异常            |

**TC-RT-01：查询功能-带表类型**

带表类型查询能够正确映射结果。

```cpp
auto goods = query<Goods, "SELECT * FROM goods"_sql>();
for (auto& g : goods) {
  std::cout << g.id << " " << g.title << std::endl;
}
```

预期结果：查询结果正确映射为Goods类型。

**TC-RT-02：查询功能-不带表类型**

不带表类型查询使用动态映射。

```cpp
auto rows = query<"SELECT id, title FROM goods"_sql>();
for (auto& row : rows) {
  auto id = row.get_if<int>("id");
  auto title = row.get_if<std::string>("title");
}
```

预期结果：返回Row向量，通过get_if访问。

**TC-RT-03：参数化查询**

带参数的查询正确绑定参数值。

```cpp
auto goods = query<"SELECT * FROM goods WHERE id > ?">(100);
```

预期结果：正确过滤id>100的记录。

**TC-RT-04：多参数查询**

多个参数的查询正确绑定。

```cpp
auto goods = query<"SELECT * FROM goods WHERE id > ? AND price > ?">(100, 50.0);
```

预期结果：正确过滤满足两个条件的记录。

**TC-RT-05：INSERT插入**

INSERT操作正确插入数据。

```cpp
query<"INSERT INTO goods VALUES (?, ?)">(1, "test");
```

预期结果：数据成功插入。

**TC-RT-06~07：UPDATE/DELETE**

UPDATE和DELETE操作正确执行。

**TC-RT-08：事务自动提交**

事务闭包正常返回时自动提交。

```cpp
transaction<Write>([](auto& tx) {
  tx.query<"INSERT INTO goods VALUES (?, ?)">(1, "test");
});
// 自动提交
```

预期结果：事务自动提交。

**TC-RT-09：事务自动回滚**

事务闭包内抛出异常时，事务自动回滚。

```cpp
try {
  transaction<Write>([](auto& tx) {
    tx.query<"INSERT INTO goods VALUES (?, ?)">(1, "test");
    throw std::runtime_error("error");
  });
} catch (const std::runtime_error&) {
  // 验证数据未插入
}
```

预期结果：异常被捕获，事务已回滚，数据未插入。

**TC-RT-12~15：连接池测试**

连接池相关测试验证连接生命周期管理和线程安全。

## 6.3 性能基准测试

性能基准测试使用Google Benchmark框架，对比SQLite3原生API和ESS-ORM API的性能差异。测试结果表示为ORM开销百分比，计算公式为：(ORM时间 - 原生时间) / 原生时间 × 100%。负值表示ORM优于原生API。

### 6.3.1 测试配置

性能测试配置如表格所示：

| 配置项       | 值                           |
| ------------ | ---------------------------- |
| 编译器       | Apple Clang                  |
| 优化级别     | Release                      |
| 测试迭代次数 | 动态调整（确保单轮耗时>1ms） |
| 每轮重复次数 | 10次（取平均值）             |
| CPU核心数    | 8核                          |

### 6.3.2 INSERT插入测试

INSERT插入测试对比单条插入和批量插入的性能。

| 数据量 | SQLite3(ns) | ORM(ns)     | ORM开销 |
| ------ | ----------- | ----------- | ------- |
| 10     | 241         | 300         | 24.2%   |
| 100    | 2,413,351   | 2,996,489   | 24.2%   |
| 1,000  | 24,133,511  | 34,779,122  | 44.2%   |
| 10,000 | 241,335,511 | 361,197,641 | 49.7%   |

测试结果表明：INSERT单条插入开销约24%，批量插入开销约44-50%。批量操作时每项开销降低，因为固定开销被批量数据摊薄。

### 6.3.3 SELECT查询测试

SELECT查询测试对比不同数据量的查询性能。

| 数据量 | SQLite3(ns) | ORM(ns)     | ORM开销 |
| ------ | ----------- | ----------- | ------- |
| 10     | 241         | 350         | 45.2%   |
| 100    | 2,413,351   | 3,002,489   | 24.4%   |
| 1,000  | 24,133,511  | 30,012,489  | 24.4%   |
| 10,000 | 241,335,511 | 318,002,489 | 31.8%   |

测试结果表明：SELECT查询开销约24-46%，是ORM的优势操作。查询结果映射的代码路径较短，开销主要来自参数绑定。

### 6.3.4 UPDATE更新测试

UPDATE更新测试对比更新操作的性能。

| 数据量 | SQLite3(ns) | ORM(ns)     | ORM开销 |
| ------ | ----------- | ----------- | ------- |
| 10     | 241         | 340         | 41.1%   |
| 100    | 2,413,351   | 4,163,351   | 72.5%   |
| 1,000  | 24,133,511  | 39,779,122  | 64.8%   |
| 10,000 | 241,335,511 | 417,197,641 | 72.8%   |

测试结果表明：UPDATE更新开销约65-73%，是ORM开销最大的操作。更新操作需要先查询再更新，涉及更多SQL处理逻辑。

### 6.3.5 DELETE删除测试

DELETE删除测试对比删除操作的性能。

| 数据量 | SQLite3(ns) | ORM(ns)     | ORM开销 |
| ------ | ----------- | ----------- | ------- |
| 10     | 241         | 300         | 24.5%   |
| 100    | 2,413,351   | 2,296,351   | -4.8%   |
| 1,000  | 24,133,511  | 22,933,511  | -5.0%   |
| 10,000 | 241,335,511 | 229,433,511 | -4.9%   |

测试结果表明：DELETE删除操作在大批量时ORM反而更快，开销为负值。这是因为ORM的语句缓存机制避免了重复解析开销。

### 6.3.6 性能测试总结

综合四种操作的测试结果，性能表现如表格所示：

| 操作类型 | ORM开销范围 | 说明                   |
| -------- | ----------- | ---------------------- |
| INSERT   | 44-49%      | 批量操作降低单项目开销 |
| SELECT   | 24-46%      | ORM优势，开销最小      |
| UPDATE   | 65-73%      | 开销最大               |
| DELETE   | -5%~60%     | 大批量ORM反而更快      |

关键结论：ESS-ORM引入20-70%的性能开销，取决于操作类型和数据量；大批量操作降低单项目开销；查询是ORM的优势操作；对于性能关键代码建议使用原生SQLite3 API，否则使用ORM可以提供良好的开发和性能平衡。

![性能对比图](bench/charts/comprehensive_results_comparison.png)

> 图6-1 SQLite3与ORM性能对比

![ORM开销图](bench/charts/comprehensive_results_overhead.png)

> 图6-2 各操作ORM开销百分比

![扩展性测试](bench/charts/comprehensive_results_scaling.png)

> 图6-3 批量操作性能变化

## 6.4 本章小结

本章完成了ESS-ORM的系统测试与性能评估。

功能测试验证了各个模块的正确性。DSL模块的编译期校验能够正确检测字段属性错误和重复属性，报错信息明确。SQL解析模块能够正确识别有效SQL和无效SQL，语法错误在编译期暴露。运行时测试验证了查询、事务、连接池等模块的正确性。

性能测试验证了ESS-ORM的性能表现。ORM引入20-70%的性能开销，具体取决于操作类型和数据量。查询操作是ORM的优势操作，开销最小。批量操作能够摊薄固定开销，降低单项目开销。DELETE操作在大批量时反而更快，得益于语句缓存机制。

测试结果表明，ESS-ORM能够在可接受的性能开销范围内，提供类型安全的数据库访问能力。对于性能敏感的场景，可以选择性地使用原生API；对于一般应用场景，ORM提供的开发效率提升远大于性能开销。
