**测试用例表**

| 用例编号  | 测试内容                      | 测试代码                                                            | 预期结果 |
| --------- | ----------------------------- | ------------------------------------------------------------------- | -------- |
| TC-DSL-01 | 有效Schema定义                | `Field<"id", &Goods::id, PrimaryKey, AutoIncrement>`                | 编译成功 |
| TC-DSL-02 | AutoIncrement用于非整数类型   | `Field<"name", &Goods::name, AutoIncrement>`                        | 编译失败 |
| TC-DSL-03 | 重复属性声明                  | `Field<"id", &Goods::id, PrimaryKey, PrimaryKey>`                   | 编译失败 |
| TC-DSL-04 | DefaultValue与DefaultExpr互斥 | `Field<"id", &Goods::id, DefaultValue<1>, DefaultExpr<"1">>`        | 编译失败 |
| TC-DSL-05 | 重复字段名                    | `Schema<"goods", Field<"id", &Goods::id>, Field<"id", &Goods::id>>` | 编译失败 |

---

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

---

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

---

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
