# SQL 语法支持

> 编译期 SQL 静态分析器支持的功能列表

---

## 1. 查询语句 (SELECT)

### 1.1 SELECT 子句

- [x] `SELECT *` - 选择所有列
- [x] `SELECT DISTINCT` - 去重选择
- [x] `SELECT col1, col2, ...` - 选择指定列
- [x] 列别名 `AS alias` / `column alias`
- [x] 表前缀 `g.id`, `goods.title`

### 1.2 FROM 子句

- [x] 单表 `FROM table_name`
- [x] 多表 `FROM t1, t2`
- [x] 表别名 `FROM goods g`
- [x] 子查询 `FROM (SELECT ...) alias`

### 1.3 JOIN 语法

- [x] `JOIN ... ON ...` - 内连接
- [x] `LEFT JOIN ... ON ...` - 左外连接
- [x] `RIGHT JOIN ... ON ...` - 右外连接
- [x] `INNER JOIN ... ON ...` - 内连接
- [x] `OUTER JOIN ... ON ...` - 外连接
- [x] `CROSS JOIN` - 交叉连接

### 1.4 WHERE 子句

- [x] 比较运算符 `=`, `!=`, `<>`, `<`, `>`, `<=`, `>=`
- [x] 逻辑运算符 `AND`, `OR`, `NOT`
- [x] 圆括号分组 `(condition1 OR condition2) AND condition3`
- [x] `IN (value1, value2, ...)`
- [x] `NOT IN (...)`
- [x] `LIKE 'pattern'`
- [x] `BETWEEN a AND b`
- [x] `IS NULL`, `IS NOT NULL`
- [x] 子查询条件 `IN (SELECT ...)`, `= (SELECT ...)`

### 1.5 聚合函数

- [x] `COUNT(*)`, `COUNT(col)`, `COUNT(DISTINCT col)`
- [x] `SUM(col)` - 求和
- [x] `AVG(col)` - 平均值
- [x] `MAX(col)` - 最大值
- [x] `MIN(col)` - 最小值

### 1.6 GROUP BY / HAVING

- [x] `GROUP BY col`
- [x] `GROUP BY col1, col2`
- [x] `GROUP BY` 表达式
- [x] `HAVING` 聚合条件

### 1.7 ORDER BY / LIMIT

- [x] `ORDER BY col [ASC|DESC]`
- [x] `ORDER BY` 表达式
- [x] `LIMIT n`
- [x] `OFFSET m`
- [x] `LIMIT n OFFSET m`

### 1.8 表达式支持

- [x] 一元运算符 `+id`, `-id`
- [x] 二元运算符 `+`, `-`, `*`, `/`, `%`
- [x] 字面量 `123`, `'text'`, `TRUE`, `FALSE`, `NULL`
- [x] 占位符 `?`
- [x] 函数调用 `UPPER(title)`, `ABS(price)`
- [x] 括号表达式 `(id + 1)`

### 1.9 复合查询

- [x] `UNION` - 并集（去重）
- [x] `UNION ALL` - 并集（不去重）
- [x] `INTERSECT` - 交集
- [x] `EXCEPT` - 差集

### 1.10 WITH 子句 (CTE)

- [x] `WITH cte AS (...) SELECT ...`
- [x] CTE 列名 `WITH cte(col1, col2) AS (...)`
- [x] 多个 CTE `WITH cte1 AS (...), cte2 AS (...)`
- [x] CTE + 复合查询 `WITH ... SELECT ... UNION ...`

---

## 2. 插入语句 (INSERT)

### 2.1 基础语法

- [x] `INSERT INTO table_name VALUES (...)`
- [x] `INSERT INTO table_name (col1, col2) VALUES (...)`

### 2.2 多行插入

- [x] `INSERT ... VALUES (...), (...), ...`

### 2.3 INSERT...SELECT

- [x] `INSERT INTO table_name SELECT ...`

### 2.4 参数化查询

- [x] `INSERT ... VALUES (?, ?, ?)`

### 2.5 表达式值支持

- [x] 表达式值 `INSERT ... VALUES (price * 1.1, id + 1)`
- [x] 函数调用值 `INSERT ... VALUES (UPPER('test'), NOW())`
- [x] `DEFAULT` 关键字 `INSERT ... VALUES (DEFAULT, ...)`
- [x] 子查询作为值 `INSERT ... VALUES ((SELECT ...), ...)`

---

## 3. 更新语句 (UPDATE)

### 3.1 基础语法

- [x] `UPDATE table_name SET col = value`
- [x] `UPDATE table_name SET col1 = value1, col2 = value2`

### 3.2 WHERE 条件

- [x] `UPDATE ... WHERE condition`

### 3.3 参数化查询

- [x] `UPDATE ... SET col = ? WHERE id = ?`

### 3.4 表达式值支持

- [x] `DEFAULT` 关键字 `UPDATE ... SET col = DEFAULT`
- [x] 表达式值 `UPDATE ... SET price = price * 1.1`
- [x] 函数调用值 `UPDATE ... SET title = UPPER(title)`
- [x] 子查询作为值 `UPDATE ... SET price = (SELECT AVG(price) FROM ...)`
- [x] 算术表达式 `UPDATE ... SET price = 10.0 * 2 + 5`
- [x] 一元运算符 `UPDATE ... SET price = -price`

---

## 4. 删除语句 (DELETE)

### 4.1 基础语法

- [x] `DELETE FROM table_name`
- [x] `DELETE FROM table_name WHERE condition`

### 4.2 参数化查询

- [x] `DELETE FROM ... WHERE id = ?`

### 4.3 WHERE 条件支持

- [x] 复杂条件 `DELETE ... WHERE id > 0 AND price < 100`
- [x] LIKE条件 `DELETE ... WHERE title LIKE '%old%'`
- [x] IN条件 `DELETE ... WHERE id IN (1, 2, 3)`
- [x] 子查询条件 `DELETE ... WHERE id IN (SELECT ...)`
- [x] 多条件组合 `DELETE ... WHERE status = ? AND enabled = ?`
- [x] NOT条件 `DELETE ... WHERE NOT (id = 1)`
- [x] EXISTS条件 `DELETE ... WHERE EXISTS (SELECT ...)`

---

## 2. 编译期错误检测

### 2.1 语法错误

- [x] 缺少关键字（如 `SELECT * goods` 缺少 `FROM`）
- [x] 缺少操作数（如 `id =` 缺少右操作数）
- [x] 缺少表名（如 `DELETE FROM` 缺少表名）
- [x] 缺少列名（如 `INSERT INTO goods () VALUES (...)`）

### 2.2 语义错误

- [x] `col = NULL` 错误（应使用 `col IS NULL`）
- [x] 无意义表达式（如 `1 IS NULL`）
- [x] 缺少条件值（如 `WHERE id =` 缺少值）
- [x] 空列表（如 `WHERE id IN ()`）

### 2.3 结构错误

- [x] 括号不匹配（如 `(id + 1` 缺少右括号）
- [x] 函数调用括号不匹配（如 `UPPER(title`）
- [x] 缺少逗号分隔符（如 `SELECT id title`）
- [x] 多余的逗号（如 `SELECT id, , title`）

---

## 3. 使用示例

### 3.1 查询语句

```sql
-- 基础查询
SELECT * FROM goods
SELECT id, title FROM goods WHERE id > 10

-- 聚合与分组
SELECT COUNT(*) FROM goods
SELECT id, COUNT(*) FROM goods GROUP BY id

-- 表连接
SELECT * FROM goods JOIN category ON goods.cid = category.id

-- 复合查询
SELECT * FROM goods UNION SELECT * FROM goods WHERE id < 100

-- CTE
WITH cte AS (SELECT * FROM goods) SELECT * FROM cte
```

### 3.2 插入语句

```sql
-- 基础插入
INSERT INTO goods (title, price) VALUES ('test', 10.0)
INSERT INTO goods VALUES (1, 'test', 10.0, 100)

-- 多行插入
INSERT INTO goods (title, price) VALUES ('a', 1), ('b', 2)

-- INSERT...SELECT
INSERT INTO goods SELECT * FROM goods WHERE id > 0

-- 参数化插入
INSERT INTO goods (title, price) VALUES (?, ?)
```

### 3.3 更新语句

```sql
-- 基础更新
UPDATE goods SET price = 10.0
UPDATE goods SET price = 10.0 WHERE id = 1

-- 多列更新
UPDATE goods SET title = 'new', price = 20.0

-- 表达式更新
UPDATE goods SET price = price * 1.1
UPDATE goods SET title = UPPER(title)
UPDATE goods SET price = (SELECT AVG(price) FROM goods)
```

### 3.4 删除语句

```sql
-- 基础删除
DELETE FROM goods
DELETE FROM goods WHERE id = 1

-- 复杂条件删除
DELETE FROM goods WHERE id > 0 AND price < 100
DELETE FROM goods WHERE title LIKE '%old%'
DELETE FROM goods WHERE id IN (1, 2, 3)
DELETE FROM goods WHERE id IN (SELECT id FROM goods WHERE price > 100)
```

---

## 4. 当前限制

### 4.1 不支持的功能

- [ ] 递归 CTE
- [ ] 嵌套 WITH 子句
- [ ] 窗口函数
- [ ] DDL 语句（CREATE/ALTER/DROP）
- [ ] 存储过程
- [ ] 触发器

### 4.2 语义检查限制

- [ ] 列类型推导
- [ ] 子查询列形状校验
- [ ] 参数类型检查
- [ ] 复合查询 ORDER BY/LIMIT 作用域限制

---
