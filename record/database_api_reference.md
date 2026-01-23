# 数据库 C API 参考手册

## 目录

1. [SQLite3 C API](#1-sqlite3-c-api)
2. [MySQL C API](#2-mysql-c-api)
3. [PostgreSQL libpq API](#3-postgresql-libpq-api)
4. [三数据库对比与最佳实践](#4-三数据库对比与最佳实践)

---

## 1. SQLite3 C API

### 1.1 连接管理

#### `sqlite3_open`

```cpp
int sqlite3_open(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);
```

| 参数       | 说明                                                                         |
| ---------- | ---------------------------------------------------------------------------- |
| `filename` | 数据库文件路径，支持特殊值：`:memory:`（内存数据库）、`:temp:`（临时数据库） |
| `ppDb`     | 输出参数，返回数据库句柄指针的指针                                           |

| 返回值          | 说明                                           |
| --------------- | ---------------------------------------------- |
| `SQLITE_OK` (0) | 成功                                           |
| 其他            | 错误代码，可用 `sqlite3_errmsg()` 获取详细信息 |

**使用示例：**

```cpp
sqlite3* db = nullptr;
int rc = sqlite3_open("test.db", &db);
if (rc != SQLITE_OK) {
  fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
  return 1;
}

// 使用 db...

sqlite3_close(db);  // 关闭数据库
```

---

#### `sqlite3_open_v2`

```cpp
int sqlite3_open_v2(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb,        /* OUT: SQLite db handle */
  int flags,              /* Flags */
  const char *zVfs        /* Name of VFS module to use (or NULL) */
);
```

| 常用标志值                                    | 说明                   |
| --------------------------------------------- | ---------------------- |
| `SQLITE_OPEN_READONLY`                        | 只读打开               |
| `SQLITE_OPEN_READWRITE`                       | 读写打开               |
| `SQLITE_OPEN_READWRITE \| SQLITE_OPEN_CREATE` | 读写打开，不存在则创建 |
| `SQLITE_OPEN_NOMUTEX`                         | 多线程模式（无互斥锁） |
| `SQLITE_OPEN_FULLMUTEX`                       | 串行模式（完全互斥）   |
| `SQLITE_OPEN_SHAREDCACHE`                     | 启用共享缓存           |

**使用示例（推荐用于连接池）：**

```cpp
sqlite3* db = nullptr;
int rc = sqlite3_open_v2(
  "test.db",
  &db,
  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
  nullptr
);
```

---

#### `sqlite3_close`

```cpp
int sqlite3_close(sqlite3 *db);
```

| 参数 | 说明               |
| ---- | ------------------ |
| `db` | 要关闭的数据库句柄 |

**注意事项：**

- 关闭前必须完成所有准备好的语句
- 如果有未完成的语句，返回 `SQLITE_BUSY`
- 应该使用 `sqlite3_close_v2()` 来更安全地关闭

---

#### `sqlite3_close_v2`

```cpp
int sqlite3_close_v2(sqlite3 *db);
```

**区别：** 如果有未完成的语句，会安排在后续关闭，而不是立即返回错误。

---

### 1.2 SQL 语句执行

#### `sqlite3_exec` - 简单执行

```cpp
int sqlite3_exec(
  sqlite3 *db,                                  /* Database handle */
  const char *sql,                              /* SQL to be evaluated */
  int (*callback)(void*,int,char**,char**),     /* Callback function */
  void *arg,                                    /* 1st argument to callback */
  char **errmsg                                 /* Error msg written here */
);
```

| 参数       | 说明                                                 |
| ---------- | ---------------------------------------------------- |
| `db`       | 数据库句柄                                           |
| `sql`      | 要执行的 SQL 语句                                    |
| `callback` | 回调函数，用于处理查询结果（可为 NULL）              |
| `arg`      | 传递给回调函数的用户数据                             |
| `errmsg`   | 输出参数，错误信息（需要调用 `sqlite3_free()` 释放） |

**回调函数签名：**

```cpp
int callback(void *arg, int column_count, char **values, char **names);
```

| 参数           | 说明                 |
| -------------- | -------------------- |
| `arg`          | 用户数据             |
| `column_count` | 列数                 |
| `values`       | 值数组（字符串形式） |
| `names`        | 列名数组             |

**使用示例：**

```cpp
char* err_msg = nullptr;

// 不需要结果的操作（CREATE、INSERT、UPDATE、DELETE）
int rc = sqlite3_exec(db, "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)", nullptr, nullptr, &err_msg);
if (rc != SQLITE_OK) {
  fprintf(stderr, "SQL error: %s\n", err_msg);
  sqlite3_free(err_msg);
}

// 需要结果的操作（SELECT）
static int callback(void *data, int argc, char **argv, char **azColName) {
  for (int i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

rc = sqlite3_exec(db, "SELECT * FROM users", callback, nullptr, &err_msg);
```

---

#### `sqlite3_prepare_v2` - 准备语句（推荐）

```cpp
int sqlite3_prepare_v2(
  sqlite3 *db,            /* Database handle */
  const char *zSql,       /* SQL statement, UTF-8 encoded */
  int nByte,              /* Maximum length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
);
```

| 参数     | 说明                                            |
| -------- | ----------------------------------------------- |
| `db`     | 数据库句柄                                      |
| `zSql`   | SQL 语句字符串                                  |
| `nByte`  | SQL 长度，传递 -1 表示自动计算（以 '\0' 结尾）  |
| `ppStmt` | 输出参数，返回语句句柄                          |
| `pzTail` | 输出参数，指向未使用的 SQL 部分（多语句时有用） |

**返回值：**

- `SQLITE_OK` (0) - 成功
- 其他 - 错误代码

**使用示例：**

```cpp
sqlite3_stmt* stmt = nullptr;
const char* sql = "SELECT id, name FROM users WHERE age > ?";
int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
if (rc != SQLITE_OK) {
  fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
  return;
}

// 使用 stmt...

sqlite3_finalize(stmt);  // 清理语句
```

---

#### `sqlite3_step` - 执行/迭代语句

```cpp
int sqlite3_step(sqlite3_stmt *stmt);
```

| 返回值          | 说明                     |
| --------------- | ------------------------ |
| `SQLITE_ROW`    | 查询结果的一行可用       |
| `SQLITE_DONE`   | 执行完成（无更多行）     |
| `SQLITE_BUSY`   | 数据库被锁定（需要重试） |
| `SQLITE_ERROR`  | 运行时错误               |
| `SQLITE_MISUSE` | 语句使用不当             |

**使用示例（迭代结果）：**

```cpp
while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
  int id = sqlite3_column_int(stmt, 0);
  const unsigned char* name = sqlite3_column_text(stmt, 1);
  printf("id=%d, name=%s\n", id, name);
}

if (rc != SQLITE_DONE) {
  fprintf(stderr, "Execution error: %s\n", sqlite3_errmsg(db));
}
```

---

#### `sqlite3_finalize` - 清理语句

```cpp
int sqlite3_finalize(sqlite3_stmt *stmt);
```

**注意事项：**

- 必须为每个 `sqlite3_prepare_v2()` 返回的语句调用
- 即使执行出错，也需要调用

---

#### `sqlite3_reset` - 重置语句（可重用）

```cpp
int sqlite3_reset(sqlite3_stmt *stmt);
```

**用途：**

- 重置语句以重新绑定参数并再次执行
- 保留语句的编译结果，提高性能

**使用示例：**

```cpp
// 准备语句
sqlite3_prepare_v2(db, "INSERT INTO users (name, age) VALUES (?, ?)", -1, &stmt, nullptr);

// 第一次执行
sqlite3_bind_text(stmt, 1, "Alice", -1, SQLITE_TRANSIENT);
sqlite3_bind_int(stmt, 2, 25);
sqlite3_step(stmt);

// 重置
sqlite3_reset(stmt);

// 第二次执行
sqlite3_bind_text(stmt, 1, "Bob", -1, SQLITE_TRANSIENT);
sqlite3_bind_int(stmt, 2, 30);
sqlite3_step(stmt);

sqlite3_finalize(stmt);
```

---

### 1.3 参数绑定

| 函数                                                   | 说明           |
| ------------------------------------------------------ | -------------- |
| `sqlite3_bind_null(stmt, idx)`                         | 绑定 NULL      |
| `sqlite3_bind_int(stmt, idx, value)`                   | 绑定整数       |
| `sqlite3_bind_int64(stmt, idx, value)`                 | 绑定 64 位整数 |
| `sqlite3_bind_double(stmt, idx, value)`                | 绑定浮点数     |
| `sqlite3_bind_text(stmt, idx, value, len, destructor)` | 绑定文本       |
| `sqlite3_bind_blob(stmt, idx, value, len, destructor)` | 绑定二进制数据 |

#### `sqlite3_bind_text` 详细说明

```cpp
int sqlite3_bind_text(
  sqlite3_stmt *stmt,     /* Statement handle */
  int idx,                 /* Index of parameter (1-based) */
  const char *value,       /* Text value */
  int len,                 /* Length of text, or -1 for auto */
  void (*destructor)(void*)/* Destructor function */
);
```

| 参数         | 说明                  |
| ------------ | --------------------- |
| `stmt`       | 语句句柄              |
| `idx`        | 参数索引（从 1 开始） |
| `value`      | 文本值                |
| `len`        | 长度，-1 表示自动计算 |
| `destructor` | 析构函数（见下表）    |

| 析构函数值         | 说明                             |
| ------------------ | -------------------------------- |
| `SQLITE_STATIC`    | 不复制，假设字符串生命周期足够长 |
| `SQLITE_TRANSIENT` | 复制字符串（默认安全选择）       |
| 自定义函数指针     | 自定义析构逻辑                   |

**性能对比：**

```cpp
// 方案 1: SQLITE_TRANSIENT（安全，但有复制开销）
sqlite3_bind_text(stmt, 1, "Alice", -1, SQLITE_TRANSIENT);

// 方案 2: SQLITE_STATIC（高性能，但需保证生命周期）
std::string name = "Alice";
sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
// name 必须在 stmt 使用期间保持有效

// 方案 3: 自定义析构（平衡性能与安全）
void my_destructor(void* ptr) {
  delete[] static_cast<char*>(ptr);
}

char* buf = new char[100];
strcpy(buf, "Alice");
sqlite3_bind_text(stmt, 1, buf, -1, my_destructor);
// buf 会在语句清理时自动释放
```

---

### 1.4 结果访问

#### `sqlite3_column_count`

```cpp
int sqlite3_column_count(sqlite3_stmt *stmt);
```

**返回值：** 结果集中的列数

---

#### `sqlite3_column_name`

```cpp
const char *sqlite3_column_name(sqlite3_stmt *stmt, int iCol);
```

**返回值：** 列名（支持 AS 别名）

**示例：**

```cpp
int cols = sqlite3_column_count(stmt);
for (int i = 0; i < cols; ++i) {
  const char* name = sqlite3_column_name(stmt, i);
  printf("Column %d: %s\n", i, name);
}
```

---

#### `sqlite3_column_type`

```cpp
int sqlite3_column_type(sqlite3_stmt *stmt, int iCol);
```

| 返回值           | 类型       |
| ---------------- | ---------- |
| `SQLITE_INTEGER` | 整数       |
| `SQLITE_FLOAT`   | 浮点数     |
| `SQLITE_TEXT`    | 文本       |
| `SQLITE_BLOB`    | 二进制数据 |
| `SQLITE_NULL`    | NULL       |

---

#### 数据获取函数

| 函数                                | 返回类型               | 说明           |
| ----------------------------------- | ---------------------- | -------------- |
| `sqlite3_column_int(stmt, iCol)`    | `int`                  | 获取整数       |
| `sqlite3_column_int64(stmt, iCol)`  | `sqlite3_int64`        | 获取 64 位整数 |
| `sqlite3_column_double(stmt, iCol)` | `double`               | 获取浮点数     |
| `sqlite3_column_text(stmt, iCol)`   | `const unsigned char*` | 获取文本       |
| `sqlite3_column_blob(stmt, iCol)`   | `const void*`          | 获取二进制数据 |
| `sqlite3_column_bytes(stmt, iCol)`  | `int`                  | 获取数据长度   |

**完整示例：**

```cpp
while (sqlite3_step(stmt) == SQLITE_ROW) {
  int id = sqlite3_column_int(stmt, 0);
  const unsigned char* name = sqlite3_column_text(stmt, 1);
  double price = sqlite3_column_double(stmt, 2);
  int type = sqlite3_column_type(stmt, 2);

  printf("id=%d, name=%s, price=%.2f (type=%d)\n", id, name, price, type);
}
```

---

#### `sqlite3_expanded_sql`

```cpp
char *sqlite3_expanded_sql(sqlite3_stmt *stmt);
```

**返回值：** 展开参数后的完整 SQL 语句（需要调用 `sqlite3_free()` 释放）

**示例：**

```cpp
sqlite3_bind_text(stmt, 1, "Alice", -1, SQLITE_TRANSIENT);
sqlite3_bind_int(stmt, 2, 25);

char* expanded = sqlite3_expanded_sql(stmt);
printf("Executing: %s\n", expanded);
// 输出: INSERT INTO users (name, age) VALUES ('Alice', 25)

sqlite3_free(expanded);
```

---

### 1.5 错误处理

#### `sqlite3_errcode`

```cpp
int sqlite3_errcode(sqlite3 *db);
```

**返回值：** 最近一次操作的错误代码

---

#### `sqlite3_errmsg`

```cpp
const char *sqlite3_errmsg(sqlite3 *db);
```

**返回值：** 最近一次操作的错误描述（UTF-8 编码）

**完整错误处理示例：**

```cpp
int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
if (rc != SQLITE_OK) {
  fprintf(stderr,
    "SQLite error [%d]: %s\n",
    sqlite3_errcode(db),
    sqlite3_errmsg(db)
  );
  return false;
}
```

---

### 1.6 性能优化 PRAGMA

| PRAGMA               | 说明                    | 示例                               |
| -------------------- | ----------------------- | ---------------------------------- |
| `journal_mode=WAL`   | WAL 模式（提高并发）    | `PRAGMA journal_mode=WAL;`         |
| `synchronous=NORMAL` | 平衡性能和安全          | `PRAGMA synchronous=NORMAL;`       |
| `cache_size=-N`      | 设置缓存大小（N 为 KB） | `PRAGMA cache_size=-64000;` (64MB) |
| `busy_timeout=N`     | 等待锁定超时（毫秒）    | `PRAGMA busy_timeout=30000;`       |
| `temp_store=MEMORY`  | 临时表存储在内存        | `PRAGMA temp_store=MEMORY;`        |

---

## 2. MySQL C API

### 2.1 连接管理

#### `mysql_init`

```cpp
MYSQL *mysql_init(MYSQL *mysql);
```

| 参数    | 说明                                         |
| ------- | -------------------------------------------- |
| `mysql` | 传入 NULL 则分配新结构，传入已有结构则初始化 |

| 返回值   | 说明                          |
| -------- | ----------------------------- |
| `MYSQL*` | 初始化的句柄（失败返回 NULL） |

**使用示例：**

```cpp
MYSQL* conn = mysql_init(nullptr);
if (conn == nullptr) {
  fprintf(stderr, "mysql_init() failed\n");
  return;
}
```

---

#### `mysql_real_connect`

```cpp
MYSQL *mysql_real_connect(
  MYSQL *mysql,
  const char *host,
  const char *user,
  const char *passwd,
  const char *db,
  unsigned int port,
  const char *unix_socket,
  unsigned long client_flag
);
```

| 参数          | 说明                                          |
| ------------- | --------------------------------------------- |
| `mysql`       | `mysql_init()` 返回的句柄                     |
| `host`        | 主机名或 IP（`"localhost"` 或 `"127.0.0.1"`） |
| `user`        | 用户名                                        |
| `passwd`      | 密码                                          |
| `db`          | 数据库名（可为 NULL）                         |
| `port`        | 端口（0 表示默认 3306）                       |
| `unix_socket` | Unix socket（Windows 上忽略）                 |
| `client_flag` | 客户端标志（通常传 0）                        |

| 常用 client_flag          | 说明       |
| ------------------------- | ---------- |
| `CLIENT_MULTI_STATEMENTS` | 允许多语句 |
| `CLIENT_COMPRESS`         | 启用压缩   |

**使用示例：**

```cpp
MYSQL* conn = mysql_init(nullptr);

if (mysql_real_connect(
  conn,
  "localhost",
  "username",
  "password",
  "testdb",
  3306,
  nullptr,
  0
) == nullptr) {
  fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
  mysql_close(conn);
  return;
}

// 使用 conn...

mysql_close(conn);
```

---

#### `mysql_close`

```cpp
void mysql_close(MYSQL *sock);
```

---

### 2.2 查询执行

#### `mysql_query` - 执行简单查询

```cpp
int mysql_query(MYSQL *mysql, const char *stmt_str);
```

| 参数       | 说明                     |
| ---------- | ------------------------ |
| `mysql`    | 连接句柄                 |
| `stmt_str` | SQL 语句（以 '\0' 结尾） |

| 返回值 | 说明 |
| ------ | ---- |
| `0`    | 成功 |
| 非 0   | 失败 |

**使用示例：**

```cpp
if (mysql_query(conn, "CREATE TABLE users (id INT PRIMARY KEY, name VARCHAR(100))") != 0) {
  fprintf(stderr, "Query failed: %s\n", mysql_error(conn));
  return;
}

if (mysql_query(conn, "INSERT INTO users (id, name) VALUES (1, 'Alice')") != 0) {
  fprintf(stderr, "Insert failed: %s\n", mysql_error(conn));
  return;
}
```

---

#### `mysql_real_query` - 执行带长度的查询（推荐）

```cpp
int mysql_real_query(
  MYSQL *mysql,
  const char *stmt_str,
  unsigned long length
);
```

| 参数       | 说明     |
| ---------- | -------- |
| `mysql`    | 连接句柄 |
| `stmt_str` | SQL 语句 |
| `length`   | 语句长度 |

**用途：**

- 比 `mysql_query()` 更安全（明确长度）
- 可以处理包含 '\0' 的语句

---

### 2.3 结果处理

#### `mysql_store_result` - 获取全部结果到内存

```cpp
MYSQL_RES *mysql_store_result(MYSQL *mysql);
```

| 返回值       | 说明                          |
| ------------ | ----------------------------- |
| `MYSQL_RES*` | 结果集指针（无结果返回 NULL） |

**特点：**

- 将所有数据读入客户端内存
- 适合小结果集
- 可以随意前后移动

**使用示例：**

```cpp
if (mysql_query(conn, "SELECT * FROM users") != 0) {
  fprintf(stderr, "Query failed\n");
  return;
}

MYSQL_RES* result = mysql_store_result(conn);
if (result == nullptr) {
  fprintf(stderr, "mysql_store_result() failed\n");
  return;
}

// 使用结果...

mysql_free_result(result);
```

---

#### `mysql_use_result` - 逐行获取结果

```cpp
MYSQL_RES *mysql_use_result(MYSQL *mysql);
```

**特点：**

- 逐行从服务器读取
- 适合大结果集
- 在使用结果集期间不能执行其他查询
- 不能移动游标（只能向前）

**使用示例：**

```cpp
if (mysql_query(conn, "SELECT * FROM large_table") != 0) {
  return;
}

MYSQL_RES* result = mysql_use_result(conn);
MYSQL_ROW row;

while ((row = mysql_fetch_row(result)) != nullptr) {
  // 处理每一行
  process_row(row);
}

mysql_free_result(result);
```

---

#### `mysql_fetch_row` - 获取一行

```cpp
MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
```

| 返回值      | 说明                                |
| ----------- | ----------------------------------- |
| `MYSQL_ROW` | 行数据（字符串数组，NULL 表示结束） |

**`MYSQL_ROW` 类型：**

```cpp
typedef char **MYSQL_ROW;  // 字符串数组
```

**使用示例：**

```cpp
MYSQL_RES* result = mysql_store_result(conn);
MYSQL_ROW row;

while ((row = mysql_fetch_row(result)) != nullptr) {
  for (unsigned int i = 0; i < mysql_num_fields(result); ++i) {
    printf("%s ", row[i] ? row[i] : "NULL");
  }
  printf("\n");
}
```

---

#### `mysql_num_fields` - 获取列数

```cpp
unsigned int mysql_num_fields(MYSQL_RES *result);
```

---

#### `mysql_num_rows` - 获取行数

```cpp
my_ulonglong mysql_num_rows(MYSQL_RES *result);
```

**注意：** 只能在 `mysql_store_result()` 后使用，`mysql_use_result()` 后返回 0。

---

#### `mysql_fetch_field` - 获取字段信息

```cpp
MYSQL_FIELD *mysql_fetch_field(MYSQL_RES *result);
```

**`MYSQL_FIELD` 结构：**

```cpp
typedef struct st_mysql_field {
  char *name;           // 列名（支持 AS 别名）
  char *org_name;       // 原始列名
  char *table;          // 表名
  char *org_table;      // 原始表名
  char *db;             // 数据库名
  char *catalog;        // 目录名
  char *def;            // 默认值
  unsigned long length; // 列长度
  unsigned long max_length;
  unsigned int flags;
  unsigned int decimals;
  enum enum_field_types type;  // 数据类型
} MYSQL_FIELD;
```

**常用类型：**

- `MYSQL_TYPE_TINY`、`MYSQL_TYPE_SHORT`、`MYSQL_TYPE_LONG`、`MYSQL_TYPE_LONGLONG` - 整数
- `MYSQL_TYPE_FLOAT`、`MYSQL_TYPE_DOUBLE` - 浮点数
- `MYSQL_TYPE_VARCHAR`、`MYSQL_TYPE_VAR_STRING` - 字符串
- `MYSQL_TYPE_DATE`、`MYSQL_TYPE_DATETIME` - 日期时间

**使用示例：**

```cpp
MYSQL_RES* result = mysql_store_result(conn);
unsigned int num_fields = mysql_num_fields(result);

// 打印列名
for (unsigned int i = 0; i < num_fields; ++i) {
  MYSQL_FIELD* field = mysql_fetch_field(result);
  printf("%s ", field->name);
}
printf("\n");

// 读取数据
MYSQL_ROW row;
while ((row = mysql_fetch_row(result)) != nullptr) {
  for (unsigned int i = 0; i < num_fields; ++i) {
    printf("%s ", row[i] ? row[i] : "NULL");
  }
  printf("\n");
}
```

---

#### `mysql_fetch_fields` - 获取所有字段信息

```cpp
MYSQL_FIELD *mysql_fetch_fields(MYSQL_RES *result);
```

**返回值：** `MYSQL_FIELD` 数组指针

**使用示例：**

```cpp
MYSQL_RES* result = mysql_store_result(conn);
unsigned int num_fields = mysql_num_fields(result);
MYSQL_FIELD* fields = mysql_fetch_fields(result);

for (unsigned int i = 0; i < num_fields; ++i) {
  printf("Column %d: name=%s, type=%d\n", i, fields[i].name, fields[i].type);
}
```

---

#### `mysql_fetch_lengths` - 获取每列长度

```cpp
unsigned long *mysql_fetch_lengths(MYSQL_RES *result);
```

**返回值：** 长度数组指针（与 `mysql_fetch_row()` 返回的行对应）

**使用示例：**

```cpp
MYSQL_ROW row = mysql_fetch_row(result);
unsigned long* lengths = mysql_fetch_lengths(result);

for (unsigned int i = 0; i < mysql_num_fields(result); ++i) {
  if (row[i]) {
    printf("%.*s ", (int)lengths[i], row[i]);
  }
}
```

---

#### `mysql_free_result`

```cpp
void mysql_free_result(MYSQL_RES *result);
```

---

### 2.4 参数化查询（预处理语句）

#### `mysql_stmt_init`

```cpp
MYSQL_STMT *mysql_stmt_init(MYSQL *mysql);
```

---

#### `mysql_stmt_prepare`

```cpp
int mysql_stmt_prepare(
  MYSQL_STMT *stmt,
  const char *stmt_str,
  unsigned long length
);
```

**使用示例：**

```cpp
MYSQL_STMT* stmt = mysql_stmt_init(conn);
const char* sql = "SELECT * FROM users WHERE age > ?";
if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
  fprintf(stderr, "Prepare failed: %s\n", mysql_stmt_error(stmt));
  return;
}
```

---

#### `mysql_stmt_bind_param`

```cpp
int mysql_stmt_bind_param(MYSQL_STMT *stmt, MYSQL_BIND *bind);
```

**`MYSQL_BIND` 结构：**

```cpp
typedef struct st_mysql_bind {
  unsigned long *buffer_length;  // 输出：实际长度
  void *buffer;                 // 输入/输出缓冲区
  my_bool *is_null;             // 是否为 NULL
  my_bool *error;               // 是否出错
  enum enum_field_types buffer_type;  // 数据类型
  unsigned long *length;
  unsigned long *buffer_length;
  my_bool *is_null;
  my_bool *error;
  // ...
} MYSQL_BIND;
```

**完整预处理语句示例：**

```cpp
MYSQL_STMT* stmt = mysql_stmt_init(conn);
mysql_stmt_prepare(stmt, "INSERT INTO users (name, age) VALUES (?, ?)", -1);

// 绑定参数
MYSQL_BIND bind[2];
memset(bind, 0, sizeof(bind));

char name[100] = "Alice";
int age = 25;

// 参数 1: name (string)
bind[0].buffer_type = MYSQL_TYPE_STRING;
bind[0].buffer = name;
bind[0].buffer_length = nullptr;
bind[0].length = (unsigned long*)&(unsigned long){strlen(name)};

// 参数 2: age (integer)
bind[1].buffer_type = MYSQL_TYPE_LONG;
bind[1].buffer = &age;
bind[1].is_null = nullptr;
bind[1].length = nullptr;

mysql_stmt_bind_param(stmt, bind);
mysql_stmt_execute(stmt);

mysql_stmt_close(stmt);
```

---

#### `mysql_stmt_execute`

```cpp
int mysql_stmt_execute(MYSQL_STMT *stmt);
```

---

#### `mysql_stmt_bind_result`

```cpp
int mysql_stmt_bind_result(MYSQL_STMT *stmt, MYSQL_BIND *bind);
```

**用于绑定结果缓冲区，在 `mysql_stmt_fetch()` 时自动填充。**

---

#### `mysql_stmt_fetch`

```cpp
int mysql_stmt_fetch(MYSQL_STMT *stmt);
```

| 返回值                 | 说明         |
| ---------------------- | ------------ |
| `0`                    | 成功         |
| `1`                    | 出错         |
| `MYSQL_NO_DATA`        | 没有更多数据 |
| `MYSQL_DATA_TRUNCATED` | 数据被截断   |

---

#### `mysql_stmt_close`

```cpp
int mysql_stmt_close(MYSQL_STMT *stmt);
```

---

### 2.5 事务控制

```cpp
// 开始事务
mysql_query(conn, "START TRANSACTION");
// 或
mysql_autocommit(conn, 0);

// 提交
mysql_commit(conn);

// 回滚
mysql_rollback(conn);

// 恢复自动提交
mysql_autocommit(conn, 1);
```

---

### 2.6 错误处理

#### `mysql_errno`

```cpp
unsigned int mysql_errno(MYSQL *mysql);
```

---

#### `mysql_error`

```cpp
const char *mysql_error(MYSQL *mysql);
```

---

#### `mysql_stmt_error`

```cpp
const char *mysql_stmt_error(MYSQL_STMT *stmt);
```

---

## 3. PostgreSQL libpq API

### 3.1 连接管理

#### `PQconnectdb`

```cpp
PGconn *PQconnectdb(const char *conninfo);
```

| 参数       | 说明                           |
| ---------- | ------------------------------ |
| `conninfo` | 连接字符串（空格分隔的键值对） |

**连接字符串参数：**

| 参数       | 说明     | 示例              |
| ---------- | -------- | ----------------- |
| `host`     | 主机名   | `host=localhost`  |
| `port`     | 端口     | `port=5432`       |
| `dbname`   | 数据库名 | `dbname=testdb`   |
| `user`     | 用户名   | `user=postgres`   |
| `password` | 密码     | `password=secret` |

**使用示例：**

```cpp
PGconn* conn = PQconnectdb("host=localhost port=5432 dbname=testdb user=postgres password=secret");

if (PQstatus(conn) != CONNECTION_OK) {
  fprintf(stderr, "Connection failed: %s\n", PQerrorMessage(conn));
  PQfinish(conn);
  return;
}

// 使用 conn...

PQfinish(conn);
```

---

#### `PQsetdbLogin`

```cpp
PGconn *PQsetdbLogin(
  const char *pghost,
  const char *pgport,
  const char *pgoptions,
  const char *pgtty,
  const char *dbName,
  const char *login,
  const char *pwd
);
```

---

#### `PQstatus`

```cpp
ConnStatusType PQstatus(const PGconn *conn);
```

| 返回值           | 说明         |
| ---------------- | ------------ |
| `CONNECTION_OK`  | 连接正常     |
| `CONNECTION_BAD` | 连接失败     |
| 其他             | 连接中各状态 |

---

#### `PQfinish`

```cpp
void PQfinish(PGconn *conn);
```

---

### 3.2 查询执行

#### `PQexec` - 简单执行

```cpp
PGresult *PQexec(PGconn *conn, const char *command);
```

| 参数      | 说明     |
| --------- | -------- |
| `conn`    | 连接句柄 |
| `command` | SQL 语句 |

| 返回值      | 说明                                |
| ----------- | ----------------------------------- |
| `PGresult*` | 结果句柄（必须用 `PQclear()` 释放） |

**使用示例：**

```cpp
PGresult* res = PQexec(conn, "CREATE TABLE users (id SERIAL PRIMARY KEY, name VARCHAR(100))");

if (PQresultStatus(res) != PGRES_COMMAND_OK) {
  fprintf(stderr, "CREATE TABLE failed: %s\n", PQerrorMessage(conn));
  PQclear(res);
  PQfinish(conn);
  return;
}

PQclear(res);

// 插入数据
res = PQexec(conn, "INSERT INTO users (name) VALUES ('Alice')");
PQclear(res);
```

---

#### `PQexecParams` - 参数化查询（推荐）

```cpp
PGresult *PQexecParams(
  PGconn *conn,
  const char *command,
  int nParams,
  const Oid *paramTypes,
  const char * const *paramValues,
  const int *paramLengths,
  const int *paramFormats,
  int resultFormat
);
```

| 参数           | 说明                                     |
| -------------- | ---------------------------------------- |
| `conn`         | 连接句柄                                 |
| `command`      | SQL 语句（参数占位符为 `$1`, `$2`, ...） |
| `nParams`      | 参数数量                                 |
| `paramTypes`   | 参数类型 OID 数组（NULL 表示自动推导）   |
| `paramValues`  | 参数值数组（NULL 表示该参数为 NULL）     |
| `paramLengths` | 参数长度数组（二进制数据用）             |
| `paramFormats` | 参数格式：0=文本，1=二进制               |
| `resultFormat` | 结果格式：0=文本，1=二进制               |

**使用示例：**

```cpp
const char* values[2] = {"Alice", "25"};
int lengths[2] = {5, 2};

PGresult* res = PQexecParams(
  conn,
  "INSERT INTO users (name, age) VALUES ($1, $2)",
  2,           // 参数数量
  nullptr,     // 自动推导类型
  values,      // 参数值
  nullptr,     // 自动计算长度
  nullptr,     // 文本格式
  0            // 返回文本结果
);

if (PQresultStatus(res) != PGRES_COMMAND_OK) {
  fprintf(stderr, "INSERT failed: %s\n", PQerrorMessage(conn));
  PQclear(res);
  return;
}

PQclear(res);
```

---

### 3.3 预处理语句

#### `PQprepare`

```cpp
PGresult *PQprepare(
  PGconn *conn,
  const char *stmtName,
  const char *query,
  int nParams,
  const Oid *paramTypes
);
```

| 参数         | 说明                         |
| ------------ | ---------------------------- |
| `stmtName`   | 语句名称（空字符串表示匿名） |
| `query`      | SQL 语句                     |
| `nParams`    | 参数数量                     |
| `paramTypes` | 参数类型 OID                 |

---

#### `PQexecPrepared`

```cpp
PGresult *PQexecPrepared(
  PGconn *conn,
  const char *stmtName,
  int nParams,
  const char * const *paramValues,
  const int *paramLengths,
  const int *paramFormats,
  int resultFormat
);
```

**使用示例：**

```cpp
// 准备语句
PGresult* res = PQprepare(
  conn,
  "insert_user",
  "INSERT INTO users (name, age) VALUES ($1, $2)",
  2,
  nullptr
);
PQclear(res);

// 执行预处理语句
const char* values[2] = {"Bob", "30"};
res = PQexecPrepared(conn, "insert_user", 2, values, nullptr, nullptr, 0);
PQclear(res);
```

---

### 3.4 结果处理

#### `PQresultStatus`

```cpp
ExecStatusType PQresultStatus(const PGresult *res);
```

| 返回值                 | 说明                       |
| ---------------------- | -------------------------- |
| `PGRES_EMPTY_QUERY`    | 空查询                     |
| `PGRES_COMMAND_OK`     | 命令执行成功（无返回结果） |
| `PGRES_TUPLES_OK`      | 查询成功，有返回结果       |
| `PGRES_COPY_OUT`       | 开始数据导出               |
| `PGRES_COPY_IN`        | 开始数据导入               |
| `PGRES_BAD_RESPONSE`   | 服务器响应错误             |
| `PGRES_NONFATAL_ERROR` | 非致命错误                 |
| `PGRES_FATAL_ERROR`    | 致命错误                   |

---

#### `PQntuples` - 获取行数

```cpp
int PQntuples(const PGresult *res);
```

---

#### `PQnfields` - 获取列数

```cpp
int PQnfields(const PGresult *res);
```

---

#### `PQfname` - 获取列名（支持 AS 别名）

```cpp
char *PQfname(const PGresult *res, int column_number);
```

**使用示例：**

```cpp
int cols = PQnfields(res);
for (int i = 0; i < cols; ++i) {
  printf("Column %d: %s\n", i, PQfname(res, i));
}
```

---

#### `PQfnumber` - 根据列名获取索引

```cpp
int PQfnumber(const PGresult *res, const char *column_name);
```

**返回值：** 列索引（-1 表示未找到）

---

#### `PQftype` - 获取列类型 OID

```cpp
Oid PQftype(const PGresult *res, int column_number);
```

**常用类型 OID：**

| 类型                                        | 说明               |
| ------------------------------------------- | ------------------ |
| `BOOLOID`                                   | 布尔               |
| `INT2OID`、`INT4OID`、`INT8OID`             | 整数（2/4/8 字节） |
| `FLOAT4OID`、`FLOAT8OID`                    | 浮点数（4/8 字节） |
| `VARCHAROID`、`TEXTOID`                     | 字符串             |
| `DATEOID`、`TIMESTAMPOID`、`TIMESTAMPTZOID` | 日期时间           |

---

#### `PQgetvalue` - 获取单元格值

```cpp
char *PQgetvalue(const PGresult *res, int row_number, int column_number);
```

**返回值：** 字符串形式的值（根据 resultFormat 可能是二进制）

---

#### `PQgetisnull` - 判断是否为 NULL

```cpp
int PQgetisnull(const PGresult *res, int row_number, int column_number);
```

**返回值：** 1 表示 NULL，0 表示非 NULL

---

#### `PQgetlength` - 获取值长度

```cpp
int PQgetlength(const PGresult *res, int row_number, int column_number);
```

---

#### 完整结果遍历示例

```cpp
PGresult* res = PQexec(conn, "SELECT id, name, age FROM users");

if (PQresultStatus(res) != PGRES_TUPLES_OK) {
  fprintf(stderr, "SELECT failed: %s\n", PQerrorMessage(conn));
  PQclear(res);
  return;
}

int rows = PQntuples(res);
int cols = PQnfields(res);

// 打印列名
for (int i = 0; i < cols; ++i) {
  printf("%-10s", PQfname(res, i));
}
printf("\n");

// 打印数据
for (int i = 0; i < rows; ++i) {
  for (int j = 0; j < cols; ++j) {
    if (PQgetisnull(res, i, j)) {
      printf("%-10s", "NULL");
    } else {
      printf("%-10s", PQgetvalue(res, i, j));
    }
  }
  printf("\n");
}

PQclear(res);
```

---

#### `PQclear` - 释放结果

```cpp
void PQclear(PGresult *res);
```

---

### 3.5 事务控制

```cpp
// 开始事务
PGresult* res = PQexec(conn, "BEGIN");

// 提交
res = PQexec(conn, "COMMIT");

// 回滚
res = PQexec(conn, "ROLLBACK");
```

---

### 3.6 错误处理

#### `PQerrorMessage`

```cpp
char *PQerrorMessage(const PGconn *conn);
```

---

#### `PQresultErrorMessage`

```cpp
char *PQresultErrorMessage(const PGresult *res);
```

---

#### `PQresultErrorField`

```cpp
char *PQresultErrorField(const PGresult *res, int fieldcode);
```

| fieldcode                    | 说明           |
| ---------------------------- | -------------- |
| `PG_DIAG_SEVERITY`           | 严重级别       |
| `PG_DIAG_SQLSTATE`           | SQLSTATE 代码  |
| `PG_DIAG_MESSAGE_PRIMARY`    | 主要错误消息   |
| `PG_DIAG_MESSAGE_DETAIL`     | 详细消息       |
| `PG_DIAG_MESSAGE_HINT`       | 提示           |
| `PG_DIAG_STATEMENT_POSITION` | 语句中错误位置 |

---

## 4. 三数据库对比与最佳实践

### 4.1 功能对比

| 功能              | SQLite3 | MySQL | PostgreSQL |
| ----------------- | ------- | ----- | ---------- |
| **嵌入式**        | ✅      | ❌    | ❌         |
| **客户端-服务器** | ❌      | ✅    | ✅         |
| **AS 别名支持**   | ✅      | ✅    | ✅         |
| **预处理语句**    | ✅      | ✅    | ✅         |
| **事务**          | ✅      | ✅    | ✅         |
| **WAL 模式**      | ✅      | ❌    | ❌         |
| **JSON 支持**     | ⚠️      | ✅    | ✅         |
| **数组类型**      | ❌      | ⚠️    | ✅         |
| **并发写入**      | ⚠️ 单写 | ✅    | ✅         |

### 4.2 性能特性

| 特性             | SQLite3           | MySQL       | PostgreSQL  |
| ---------------- | ----------------- | ----------- | ----------- |
| **轻量级**       | ✅ 最轻量         | 中等        | 较重        |
| **启动速度**     | ✅ 即开即用       | 中等        | 较慢        |
| **单条查询性能** | ✅ 优秀           | 良好        | 良好        |
| **并发读性能**   | ✅ 优秀           | 优秀        | 优秀        |
| **并发写性能**   | ⚠️ WAL 模式下良好 | 优秀        | 优秀        |
| **复杂查询优化** | 基础              | 优秀        | 优秀        |
| **连接开销**     | ⚠️ 文件锁         | ⚠️ TCP 连接 | ⚠️ TCP 连接 |

### 4.3 使用场景建议

| 场景                 | 推荐数据库          | 原因                             |
| -------------------- | ------------------- | -------------------------------- |
| **移动应用**         | SQLite3             | 嵌入式、零配置                   |
| **桌面应用**         | SQLite3             | 本地数据存储                     |
| **小型 Web 应用**    | SQLite3 或 MySQL    | SQLite3 更简单，MySQL 更易扩展   |
| **中型 Web 应用**    | MySQL 或 PostgreSQL | 成熟稳定，社区支持好             |
| **大型分布式系统**   | PostgreSQL          | 高级特性（JSON、数组、窗口函数） |
| **高并发写入**       | MySQL 或 PostgreSQL | SQLite3 并发写入能力有限         |
| **数据分析**         | PostgreSQL          | 丰富的分析函数                   |
| **需要复杂关系查询** | PostgreSQL          | 优秀的查询优化器                 |

### 4.4 参数绑定最佳实践

#### SQLite3

```cpp
// 推荐使用 SQLITE_TRANSIENT（安全）
sqlite3_bind_text(stmt, 1, str.c_str(), -1, SQLITE_TRANSIENT);

// 或在保证生命周期时使用 SQLITE_STATIC（高性能）
sqlite3_bind_text(stmt, 1, str.c_str(), -1, SQLITE_STATIC);
```

#### MySQL

```cpp
// 使用预处理语句（推荐）
MYSQL_STMT* stmt = mysql_stmt_init(conn);
mysql_stmt_prepare(stmt, sql, -1);
MYSQL_BIND bind[2];
// ... 设置 bind
mysql_stmt_bind_param(stmt, bind);
mysql_stmt_execute(stmt);
```

#### PostgreSQL

```cpp
// 使用 PQexecParams（推荐）
const char* values[2] = {str1.c_str(), str2.c_str()};
PQexecParams(conn, sql, 2, nullptr, values, nullptr, nullptr, 0);
```

### 4.5 结果处理最佳实践

#### 统一模式（适配三数据库）

```cpp
// 伪代码示例
class ResultSet {
public:
  int column_count() const;
  std::string column_name(int idx) const;
  bool is_null(int row, int col) const;
  std::string get_string(int row, int col) const;
  int get_int(int row, int col) const;
  double get_double(int row, int col) const;

private:
  // 使用 variant 或类型擦除存储不同数据库的句柄
  std::variant<
    std::pair<sqlite3_stmt*, std::function<void()>>,
    std::pair<MYSQL_RES*, std::function<void()>>,
    std::pair<PGresult*, std::function<void()>>
  > m_handle;
};
```

### 4.6 连接池实现建议

| 数据库         | 连接池注意事项                                                          |
| -------------- | ----------------------------------------------------------------------- |
| **SQLite3**    | 使用 WAL 模式提高并发；考虑 `SQLITE_OPEN_NOMUTEX` 配合应用层同步        |
| **MySQL**      | 监控连接数；使用 `wait_timeout` 防止空闲连接超时                        |
| **PostgreSQL** | 考虑 `max_connections` 限制；使用连接池验证（`SELECT 1`）检查连接有效性 |

### 4.7 错误处理统一模式

```cpp
enum class DBErrorType {
  CONNECTION_FAILED,
  QUERY_FAILED,
  INVALID_PARAMS,
  CONSTRAINT_VIOLATION,
  TIMEOUT
};

struct DBError {
  DBErrorType type;
  std::string message;
  int code;
  int position;  // 语法错误位置
};

// 统一错误转换
DBError convert_sqlite_error(int rc, sqlite3* db);
DBError convert_mysql_error(int rc, MYSQL* conn);
DBError convert_postgres_error(ExecStatusType status, PGresult* res);
```

### 4.8 性能优化总结

| 优化项       | SQLite3                  | MySQL                            | PostgreSQL           |
| ------------ | ------------------------ | -------------------------------- | -------------------- |
| **批量插入** | 使用事务                 | `LOAD DATA INFILE` 或批量 INSERT | `COPY` 命令          |
| **索引优化** | `CREATE INDEX`           | `CREATE INDEX`                   | `CREATE INDEX`       |
| **查询缓存** | Statement 缓存           | 查询缓存（已废弃）               | 准备语句             |
| **连接复用** | WAL 模式 + 连接池        | 连接池                           | 连接池               |
| **延迟写入** | `PRAGMA synchronous=OFF` | `innodb_flush_log_at_trx_commit` | `synchronous_commit` |

---

## 参考资源

### 官方文档

- [SQLite3 C Interface Reference](https://www.sqlite.org/c3ref/intro.html)
- [MySQL C API Reference](https://dev.mysql.com/doc/c-api/en/)
- [PostgreSQL libpq Reference](https://www.postgresql.org/docs/current/libpq.html)

### 性能调优

- [SQLite Performance Guidelines](https://www.sqlite.org/cvstrac/wiki?p=PerformanceGuidelines)
- [MySQL Performance Blog](https://www.percona.com/blog/)
- [PostgreSQL Performance Tuning](https://wiki.postgresql.org/wiki/Performance_Optimization)

---

## 版本说明

本文档基于以下 API 版本编写：

- SQLite3: 3.40+
- MySQL: 8.0+
- PostgreSQL: 14+

随着版本更新，API 可能有细微变化，请以官方文档为准。
