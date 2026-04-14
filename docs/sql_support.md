# SQL Support

> 本文档用于记录 `ess::orm::sql` 当前编译期 SQL 词法/语法分析器的支持范围。  
> 当前实现仍处于实验阶段，重点集中在 `SELECT` 查询的静态校验能力。

---

## 说明

- 当前支持分为两层：
  - `Lexer`：词法识别能力
  - `Parser`：语法解析能力
- 某些关键字虽然已在 `TokenType` / `Keywords` 中定义，但若 parser 尚未实现，对外仍视为 **未支持**
- 当前主支持范围为：
  - 单表 `SELECT`
  - 基础 `WHERE`
  - 子查询
  - `GROUP BY`
  - `HAVING`
  - `ORDER BY`

---

## 1. Token / Keyword 定义

### 已定义的关键字 Token

- [x] `SELECT`
- [x] `INSERT`
- [x] `UPDATE`
- [x] `DELETE`
- [x] `FROM`
- [x] `INTO`
- [x] `VALUES`
- [x] `WHERE`
- [x] `SET`
- [x] `AND`
- [x] `OR`
- [x] `NOT`
- [x] `IN`
- [x] `LIKE`
- [x] `BETWEEN`
- [x] `IS`
- [x] `NULL`
- [x] `ORDER`
- [x] `BY`
- [x] `ASC`
- [x] `DESC`
- [x] `LIMIT`
- [x] `OFFSET`
- [x] `JOIN`
- [x] `LEFT`
- [x] `RIGHT`
- [x] `INNER`
- [x] `OUTER`
- [x] `CROSS`
- [x] `ON`
- [x] `AS`
- [x] `DISTINCT`
- [x] `ALL`
- [x] `COUNT`
- [x] `SUM`
- [x] `AVG`
- [x] `MAX`
- [x] `MIN`
- [x] `GROUP`
- [x] `HAVING`
- [x] `EXISTS`
- [x] `CREATE`
- [x] `TABLE`
- [x] `DROP`
- [x] `ALTER`
- [x] `INDEX`
- [x] `PRIMARY`
- [x] `KEY`
- [x] `FOREIGN`
- [x] `REFERENCES`
- [x] `DEFAULT`
- [x] `UNIQUE`
- [x] `CHECK`

### 已定义的其它 Token

- [x] `Identifier`
- [x] `Number`
- [x] `String`
- [x] `PlaceHolder`

### 已定义的运算符 / 分隔符 Token

- [x] `Comma` `,`
- [x] `Dot` `.`
- [x] `Star` `*`
- [x] `Lparen` `(`
- [x] `Rparen` `)`
- [x] `Eq` `=`
- [x] `Ne` `!=` / `<>`
- [x] `Lt` `<`
- [x] `Gt` `>`
- [x] `Le` `<=`
- [x] `Ge` `>=`
- [x] `Plus` `+`
- [x] `Minus` `-`
- [x] `Slash` `/`

---

## 2. Lexer 支持情况

### 已完成

- [x] 空白字符跳过
- [x] 单行注释跳过（`-- comment`）
- [x] 关键字大小写不敏感识别
- [x] 标识符识别
- [x] 数字字面量识别
  - [x] 整数
  - [x] 小数
- [x] 字符串字面量识别
  - [x] 单引号字符串
  - [x] `''` 形式转义
- [x] 占位符 `?` 识别
- [x] 常见比较运算符识别
- [x] 基础分隔符识别
- [x] `End` token 生成
- [x] 基础错误状态返回
  - [x] 未知符号
  - [x] 未闭合字符串
  - [x] 非法数字形式（如尾随 `.`）

### 当前限制 / 未完成

- [ ] 双引号标识符处理仍需完善
- [ ] 反引号标识符支持
- [ ] 方括号标识符支持
- [ ] 命名参数支持
  - [ ] `:name`
  - [ ] `@name`
  - [ ] `$name`
- [ ] 十六进制 / 科学计数法数字
- [ ] 多行注释 `/* ... */`
- [ ] 更完整的 SQL 方言兼容
- [ ] `Keywords` 表与 `TokenType` 定义完全对齐校验
- [ ] 关键字查找优化（二分 / 哈希）

---

## 3. 当前 Parser 总体支持范围

### 已完成

- [x] `SELECT` 查询解析入口
- [x] 顶层 `SELECT` 语句解析
- [x] 子查询中的 `SELECT` 解析
- [x] 子查询允许在右括号前结束
- [x] 顶层查询要求完整消费输入
- [x] 基础语法错误返回

### 未完成

- [ ] `INSERT`
- [ ] `UPDATE`
- [ ] `DELETE`
- [ ] 方言特化 parser

---

## 4. SELECT 主体支持

### 已完成

- [x] `SELECT *`
- [x] `SELECT DISTINCT`
- [x] `SELECT ALL`
- [x] `SELECT col1, col2, ...`
- [x] `FROM table_name`
- [x] 单表查询
- [x] `WHERE`
- [x] `GROUP BY`
- [x] `HAVING`
- [x] `ORDER BY`

### 已完成

- [x] 列别名
- [x] 表别名
  - [x] `FROM goods g`
  - [x] WHERE 子句中表别名 (`g.id > 0`)
- [x] `FROM` 子查询
  - [x] `FROM (SELECT ...) alias`
- [x] 多表 `FROM`
  - [x] `FROM t1, t2`
  - [x] `FROM t1, t2 WHERE t1.id = t2.id`
- [x] `JOIN`
  - [x] `JOIN ... ON ...`
  - [x] `LEFT JOIN ... ON ...`
  - [x] `RIGHT JOIN ... ON ...`
  - [x] `INNER JOIN ... ON ...`
  - [x] `OUTER JOIN ... ON ...`
  - [x] `CROSS JOIN`
- [x] `LIMIT`
- [x] `OFFSET`

---

## 5. SELECT 列表支持

### 已完成

- [x] `*`
- [x] 普通标识符列列表
- [x] 聚合表达式列
  - [x] `COUNT(*)`
  - [x] `COUNT(id)`
  - [x] `SUM(id)`
  - [x] `AVG(id)`
  - [x] `MAX(id)`
  - [x] `MIN(id)`
- [x] `has_aggregate` 语义标记
- [x] `DISTINCT` 列表支持
- [x] 列名位置导出
  - [x] `column_start`
  - [x] `column_names[pos/len]`
  - [x] 列别名解析 (`AS`)
  - [x] `table.column` 解析
  - [x] `has_table_prefix` 标记
  - [x] 最多记录固定数量的列名（当前为 32）

### 未完成

- [ ] 表达式列
- [ ] `schema.table.column`

---

## 6. FROM 子句支持

### 已完成

- [x] `FROM identifier`
- [x] 表别名
  - [x] `FROM goods g`
  - [x] WHERE 子句中表别名 (`g.id > 0`)
- [x] 多表查询
  - [x] `FROM t1, t2`
  - [x] `FROM t1, t2 WHERE t1.id = t2.id`
- [x] `JOIN ... ON ...`
- [x] `LEFT JOIN ... ON ...`
- [x] `RIGHT JOIN ... ON ...`
- [x] `INNER JOIN ... ON ...`
- [x] `OUTER JOIN ... ON ...`
- [x] `CROSS JOIN`
- [x] `FROM (SELECT ...) alias`

---

## 7. WHERE 条件表达式支持

### 已完成

- [x] 普通比较表达式
  - [x] `=`
  - [x] `!=`
  - [x] `<>`
  - [x] `<`
  - [x] `>`
  - [x] `<=`
  - [x] `>=`
- [x] 左值为普通标识符
- [x] 右值支持
  - [x] 数字
  - [x] 字符串
  - [x] 占位符 `?`
- [x] `AND`
- [x] `OR`
- [x] 圆括号优先级分组
- [x] `IN (...)`
- [x] `NOT IN`
- [x] `IN (SELECT ...)`
- [x] `LIKE 'pattern'`
- [x] `BETWEEN a AND b`
- [x] `IS NULL`
- [x] `IS NOT NULL`
- [x] 比较右值为子查询
  - [x] `id = (SELECT ...)`
- [x] `IN` 字面量列表支持占位符
  - [x] `id IN (?, ?, ?)`
- [x] `NOT LIKE`

### 未完成

- [x] 通用 `NOT expr`
- [x] `NOT BETWEEN`
- [x] `EXISTS (SELECT ...)`
- [x] `NOT EXISTS (SELECT ...)`
- [x] 布尔字面量
- [ ] 更多表达式类型
- [x] 算术表达式
  - [x] `price + 1 > ?`
  - [x] `1 + 1 = id`
- [x] 一元正负号
- [x] `col = NULL` 语义拦截
- [x] 更严格的空值语义校验
  - [x] 拦截 `1 IS NULL`
  - [x] 拦截 `NULL IS NULL`

---

## 8. 聚合函数支持

### 已完成

- [x] 聚合函数关键字识别
  - [x] `COUNT`
  - [x] `SUM`
  - [x] `AVG`
  - [x] `MAX`
  - [x] `MIN`
- [x] 聚合表达式解析
  - [x] `COUNT(*)`
  - [x] `COUNT(id)`
  - [x] `SUM(id)`
  - [x] `AVG(id)`
  - [x] `MAX(id)`
  - [x] `MIN(id)`
- [x] 聚合表达式出现在条件表达式中
- [x] 聚合表达式支持比较运算
- [x] 聚合表达式右值支持
  - [x] 数字
  - [x] 占位符
- [x] 聚合表达式出现在 ORDER BY
- [x] `COUNT(DISTINCT col)` 支持

---

## 9. GROUP BY 支持

### 已完成

- [x] `GROUP BY col`
- [x] `GROUP BY col1, col2, ...`

### 未完成

- [ ] `GROUP BY table.column`
- [ ] `GROUP BY` 表达式
- [ ] `GROUP BY` 序号
  - [ ] `GROUP BY 1`
- [ ] `ROLLUP` / `CUBE`
- [ ] 更严格分组语义校验

---

## 10. HAVING 支持

### 已完成

- [x] `HAVING` 子句
- [x] `HAVING` 复用条件表达式解析
- [x] `HAVING` 需出现在 `GROUP BY` 之后（当前 parser 语法限制）
- [x] `HAVING COUNT(id) > ?` 等聚合条件

### 未完成

- [ ] 无 `GROUP BY` 时的单组 `HAVING` 支持
- [ ] `HAVING` 语义校验
  - [ ] 非聚合字段是否合法
  - [ ] 非聚合字段是否必须出现在 `GROUP BY`
- [ ] `HAVING EXISTS (...)`

---

## 11. ORDER BY 支持

### 已完成

- [x] `ORDER BY col`
- [x] `ORDER BY col ASC`
- [x] `ORDER BY col DESC`
- [x] `ORDER BY col1, col2`
- [x] `ORDER BY col1 ASC, col2 DESC`

### 当前限制

- [ ] `ORDER BY table.column`
- [ ] `ORDER BY` 聚合表达式
- [ ] `ORDER BY` 别名
- [ ] `ORDER BY 1`
- [ ] 更严格排序项语义校验

---

## 12. 子查询支持

### 已完成

- [x] `IN (SELECT ...)`
- [x] 比较表达式右值子查询
  - [x] `id = (SELECT ...)`
- [x] 括号中的独立子查询表达式
- [x] 子查询以 `)` 作为允许结束条件
- [x] 外层语法结构负责消费子查询闭合右括号
- [x] 多层括号场景下提前结束问题已修复

### 未完成

- [ ] `EXISTS (SELECT ...)`
- [ ] `FROM (SELECT ...) alias`
- [ ] 相关子查询
- [ ] 子查询列数 / 形状语义检查
- [ ] 子查询与外层字段关联校验

---

## 13. 已支持的典型 SQL 示例

### 可通过当前 parser 的示例

- [x] `SELECT * FROM goods`
- [x] `SELECT id, title FROM goods`
- [x] `SELECT * FROM goods WHERE id = ?`
- [x] `SELECT * FROM goods WHERE id > 10`
- [x] `SELECT * FROM goods WHERE id BETWEEN 1 AND 10`
- [x] `SELECT * FROM goods WHERE title LIKE 'abc%'`
- [x] `SELECT * FROM goods WHERE id IS NULL`
- [x] `SELECT * FROM goods WHERE id IS NOT NULL`
- [x] `SELECT * FROM goods WHERE id IN (1, 2, 3)`
- [x] `SELECT * FROM goods WHERE id IN (SELECT id FROM goods)`
- [x] `SELECT * FROM goods WHERE id = (SELECT id FROM goods)`
- [x] `SELECT * FROM goods WHERE (id > ? AND title LIKE 'a%') OR stock = 0`
- [x] `SELECT * FROM goods GROUP BY id`
- [x] `SELECT * FROM goods GROUP BY id, title`
- [x] `SELECT * FROM goods GROUP BY id HAVING COUNT(id) > ?`
- [x] `SELECT * FROM goods ORDER BY id`
- [x] `SELECT * FROM goods ORDER BY id DESC`
- [x] `SELECT * FROM goods ORDER BY id ASC, title DESC`
- [x] `SELECT * FROM goods WHERE id IN (SELECT id FROM goods WHERE (COUNT(id) > ? AND id IS NOT NULL)) ORDER BY id`
- [x] `SELECT COUNT(*) FROM goods`
- [x] `SELECT id, COUNT(*) FROM goods GROUP BY id`
- [x] `SELECT * FROM goods WHERE 1 + 1 = id AND id = 1 + 1`
- [x] `SELECT * FROM goods WHERE -id > 0`

### 当前仍不支持或未完整支持的示例

- [ ] `SELECT g.id FROM goods g`
- [ ] `SELECT * FROM goods LIMIT 10`
- [x] `SELECT * FROM goods OFFSET 20`
- [x] `SELECT * FROM goods LIMIT 10 OFFSET 20`
- [x] `SELECT * FROM goods WHERE id NOT IN (1, 2, 3)`
- [x] `SELECT * FROM goods WHERE NOT (id = 1 OR title = 'x')`
- [x] `SELECT * FROM goods WHERE EXISTS (SELECT 1)`
- [x] `SELECT * FROM (SELECT * FROM goods) t`
- [x] `SELECT * FROM goods, category WHERE goods.cid = category.id`
- [x] `SELECT * FROM goods JOIN category ON goods.cid = category.id`
- [x] `SELECT * FROM goods LEFT JOIN category ON goods.cid = category.id`
- [x] `SELECT * FROM goods OUTER JOIN category ON goods.cid = category.id`
- [x] `SELECT COUNT(DISTINCT id) FROM goods`
- [x] `SELECT * FROM goods ORDER BY COUNT(id)`
- [ ] `SELECT * FROM goods JOIN category ON goods.cid = category.id`

---

## 14. 错误处理支持

### 已完成

- [x] 基础 `SQLErrorKind` 定义
- [x] `match()` 统一匹配失败处理
- [x] 常见语法错误检测
  - [x] `SELECT` 起始错误
  - [x] 缺少列
  - [x] 缺少 `FROM`
  - [x] 缺少表名
  - [x] 缺少操作符
  - [x] 缺少右括号
  - [x] `IN` 后缺少左括号
  - [x] `BETWEEN` 缺少 `AND`
  - [x] `IS NOT` 后缺少 `NULL`
  - [x] `GROUP BY` 语法错误
  - [x] `ORDER BY` 语法错误
  - [x] 聚合函数括号/参数错误
  - [x] `HAVING` 脱离 `GROUP BY`

### 未完成

- [ ] `check()` 对全部错误类型的文案补全
- [ ] 错误位置 `err_idx` 完整填充
- [ ] lexer 错误与 parser 错误统一输出
- [ ] 更精确的上下文错误信息
- [ ] 语义错误类型拆分

---

## 15. 语义校验

> 当前实现以 **语法校验** 为主，语义校验仍待补全。

### 已完成

- [x] 基础子句顺序限制
  - [x] `SELECT -> FROM -> WHERE -> GROUP BY -> HAVING -> ORDER BY`
- [x] `HAVING` 不能脱离 `GROUP BY` 单独出现（当前语法限制）

### 未完成

- [ ] 校验列是否真实存在于实体 / schema
- [ ] 校验表是否存在于映射定义
- [ ] 校验 `SELECT` 列与返回类型映射一致
- [ ] 校验 `GROUP BY` 与聚合列搭配是否合法
- [ ] 校验 `HAVING` 中普通字段是否合法
- [ ] 校验 `ORDER BY` 字段是否合法
- [ ] 校验子查询返回列形状是否合法
- [ ] 校验占位符数量
- [ ] 校验占位符类型

---

## 16. 高优先级待完成项

- [ ] `SELECT` 列表支持聚合函数
- [ ] `SELECT DISTINCT`
- [ ] `LIMIT / OFFSET`
- [x] `NOT IN / NOT LIKE / NOT BETWEEN`
- [x] `EXISTS`
- [x] `table.column`
- [x] 表别名 / 列别名
- [ ] `IN` 字面量列表支持占位符
- [ ] `check()` 完整错误文案
- [ ] `err_idx` / 行列号联动到 parser 错误

---

## 17. 中优先级待完成项

- [ ] `FROM (subquery)`
- [ ] `JOIN`
- [ ] 聚合语义校验
- [ ] 更完整表达式系统
- [ ] `ORDER BY` / `GROUP BY` 表达式支持
- [ ] 更完整方言兼容

---

## 18. 低优先级待完成项

- [ ] `INSERT / UPDATE / DELETE` parser
- [ ] DDL parser
- [ ] `UNION / UNION ALL`
- [ ] 更复杂 SQL 方言特性
- [ ] 词法关键字查找性能优化

---
