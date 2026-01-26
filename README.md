# Ess-Orm

## sqlite3 下的 nest transaction 速查表

| 外层  | 内层  | 同线程       | 跨线程      |
| ----- | ----- | ------------ | ----------- |
| Write | Write | ✅ SAVEPOINT | ❌ 死锁     |
| Write | Read  | ❌ 禁止降级  | ⚠️ 可能工作 |
| Read  | Read  | ✅ SAVEPOINT | ✅ 并发读   |
| Read  | Write | ✅ 允许升级  | ✅ 正常     |
| 无    | Write | ✅ 正常      | ✅ 串行     |
| 无    | Read  | ✅ 正常      | ✅ 并发     |

---

## SQLite 锁机制简述

| 命令              | 锁类型       | 并发性     |
| ----------------- | ------------ | ---------- |
| `BEGIN DEFERRED`  | 延迟获取     | 允许多个读 |
| `BEGIN IMMEDIATE` | 立即获取写锁 | 阻塞其他写 |
| `SAVEPOINT`       | 继承外层锁   | 无额外锁   |

---

### 说明

#### 死锁原因

在库的 sqlite3 方言下，Write 事务启动时对应 `BEGIN IMMEDIATE`，这会获取保留锁，而 Read 事务启动时对应 `BEGIN DEFERRED`，这会获取共享锁，对于同一个数据库，在获取保留锁后在嵌套事务(nest transaction)下获取共享锁会导致死锁发生

#### 强调

即使在跨线程的情况下事务的嵌套可能仍能正常工作，但是请不要这么做！如下：

```CPP
transaction<Read>([](auto &txs) {
  std::vector<std::thread> threads;
  threads.reserve(10);
  for (int i = 0; i < 3; ++i) {
    threads.emplace_back([]() {
      transaction<Write>([](auto &tx) {
        std::vector<Goods> res =
            tx.template query<Goods, "SELECT * FROM goods ">();
        for (auto &g : res) {
          std::cout << g.title << std::endl;
        }
      });
    });
  }
  for (auto &t : threads)
    t.join();
});
```

即使这段代码可以运行，但是实际上内层嵌套的三个跨线程的`transaction`和外层的并不属于同一个上下文，此时语义和实际并不一致，而要保证库能够检测该问题难以做到，因此，请不要在事务内部使用跨线程的语义

---
