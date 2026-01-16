## 🚀 Ess ORM 后续开发任务 (TODO)

### 运行期连接管理与函数式接口 (Management)

- [ ] 隐式上下文管理器 (Implicit Context)
  > 利用 thread_local 实现 ThreadContext，自动在同步代码块中追踪当前活跃连接。
- [ ] RAII 连接守卫 (Connection Guard)
  > 实现 ConnectionGuard 逻辑，支持“按需借调”：事务内自动复用连接，非事务环境自动从 PoolManager 获取。
- [ ] 无参数事务 API (Elegant Transaction)
  > 实现 transaction([]{...}) 包装器，自动管理 BEGIN/COMMIT/ROLLBACK 逻辑，并实现异常安全的连接清理。
- [ ] 连接池实现 (Connection Pool)
  > 实现基础的 PoolManager，支持 pool_size 配置、连接心跳检测以及基本的负载分配。
- [ ] 嵌套事务支持 (Nested Transactions)
  > (子任务): 引入 SAVEPOINT 机制，解决 transaction 嵌套调用时的 SQL 方言冲突问题。
- [ ] 异步上下文适配思路研究 (Async Adaptation)
  > (子任务): 预研 std::coroutine 下的上下文传递方案，探索从 thread_local 向协程 promise_type 迁移的可能性。

### Sql 字符串静态校验

### 启动期 DDL 自动同步 (Migrations)

- [ ] **表结构自动创建**：完善 `ensure_schema<T>`，实现“表不存在则自动建表”。
- [ ] 表结构内省 (Introspection)：支持从 Postgres 系统表读取现有列信息。
- [ ] 自动演进 (Auto-Alter)：对比编译期与数据库差异，自动执行 `ALTER TABLE` 补全缺失字段。
- [ ] 约束支持：在 DDL 生成中增加对 `INDEX`、`UNIQUE` 和 `FOREIGN KEY` 的支持。

### 2. 运行期性能优化 (Performance)

- [ ] **元数据冷热分离**：将编译期反射得到的元数据在启动阶段注入静态 Hash Map，减少 SQL 拼接时的解析开销。
- [ ] **预编译语句缓存 (Statement Cache)**：缓存常用的 `INSERT` / `UPDATE` 语句对象，避免数据库重复解析。
- [ ] **内存分配优化**：在 SQL 拼接过程中引入 `pmr` 或预分配 `SmallString`，降低高并发下的内存碎片的产生。

### 3. 系统健壮性 (Robustness)

- [ ] **日志分级系统**：实现可配置的 SQL 日志（Debug 时开启，Release 时按需开启）。
- [ ] **连接池监控**：增加运行期连接池水位线、查询耗时等监控指标的输出。

---
