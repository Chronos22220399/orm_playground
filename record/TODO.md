## 🚀 Ess ORM 后续开发任务 (TODO)

### 编译期自定义类型与 Sql 结构映射（DSL）

- [x] DSL 定义层
  > 利用模版与元组实现基于类型的 DSL 功能，通过 concept 、偏特化、编译期计算与 static_assert 进行约束
- [x] DSL 语法检测层
  > 利用编译期计算实现 DSL 的语法检测以及部分语义实现，用以保证强类型契约与 Sql 对接
- [ ] 对 `NotNull` 与 `AutoIncrement` 的语法检测完善
- [ ] 对特殊类型（如Date、Time）的支持
- [ ] 对 `PrimaryKey`、`ForeignKey` 的列级别支持

### 配置系统（Configuration）

- [ ] 默认配置实现
- [ ] 配置萃取
- [ ] 配置接口

### 运行期连接管理与函数式接口 (Management)

- [x] Sqlite3 连接池
  > 利用 sqlite3 的原生 api 实现高性能连接池
- [x] sql 操作结果与自定义类型、通用类型映射
  > 通过 DSL 提供的接口实现了 mapper 进行反射结果，自定义 Row 作为通用接受类型（视图类型）
- [ ] 隐式 api 实现
- [ ] 函数式接口实现
- [ ] 序列化与反序列化实现

### Sql 字符串静态校验

- [ ] Sqlite3 的编译期递归下降语法分析
- [ ] 基于 DSL 注册信息的检测
- [ ] 可高度定义的报错信息接口

### 启动期 DDL 自动同步 (Migrations)

- [ ] 实现 `ensure_schema<T>`（当前仅支持 `make_create_table_ddl` 用于生成 ddl 字符串），实现“表不存在则自动建表”。
- [ ] 自动演进 (Auto-Alter)：对比编译期与数据库差异，自动执行 `ALTER TABLE` 补全缺失字段。（Sqlite3不支持）

### 2. 运行期性能优化 (Performance)

- [ ] **元数据冷热分离**：将编译期反射得到的元数据在启动阶段注入静态 Hash Map，减少 SQL 拼接时的解析开销。
- [ ] **内存分配优化**：在 SQL 拼接过程中引入 `pmr` 或预分配 `SmallString`，降低高并发下的内存碎片的产生。

### 3. 系统健壮性 (Robustness)

- [ ] **日志分级系统**：实现可配置的 SQL 日志（Debug 时开启，Release 时按需开启）。
- [ ] **连接池监控**：增加运行期连接池水位线、查询耗时等监控指标的输出。

---

                    之前                              之后
          ┌──────────────────────┐          ┌──────────────────────┐
          │   Transaction<M,DB>  │          │    Transaction<M,DB> │
          │  ┌─────────────────┐ │          │    (thin RAII guard) │
          │  │ nesting logic   │ │          │    只调 conn 的方法  │
          │  │ SAVEPOINT sql   │ │          └──────────┬───────────┘
          │  │ sqlite3_changes │ │                     │
          │  └─────────────────┘ │          ┌──────────▼───────────┐
          └──────────┬───────────┘          │    Connection        │
                     │                      │  ┌─────────────────┐ │
          ┌──────────▼───────────┐          │  │ begin(mode)     │ │
          │    Connection        │          │  │ commit()        │ │
          │  begin_transaction() │          │  │ rollback()      │ │
          │  commit()            │          │  │ nesting_level() │ │
          │  rollback()          │          │  │ last_insert_id()│ │
          │  (未被使用)          │          │  │ affected_rows() │ │
          └──────────────────────┘          │  └─────────────────┘ │
                                            └──────────┬───────────┘
                                                       │ virtual
                                            ┌──────────▼───────────┐
                                            │ Sqlite3Connection    │
                                            │  m_nesting_level     │
                                            │  SAVEPOINT 逻辑      │
                                            │  sqlite3_changes()   │
                                            └──────────────────────┘
