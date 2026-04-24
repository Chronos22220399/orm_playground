# 各语言ORM性能对比

## 0. 测试范围说明（重要备注）

各ORM基准测试覆盖内容对比。注意：**测试范围越广，测量出的ORM开销越大**，因为包含了更多框架功能。

| 语言/ORM | 测试包含 | 额外框架开销 | 开销范围 |
|----------|----------|-------------|---------|
| **本项目C++** | 连接池+语句缓存+结果映射 | 相比裸API | **20-70%** |
| **Rust Diesel** | 类型映射+查询构建 | 编译期检查 | 5-20% |
| **Go GORM** | 连接池+struct映射 | 简化缓存 | 0-20% |
| **Java Hibernate** | Session+脏检查+identity map | 一级缓存+变化追踪 | 50-140% |
| **Python SQLAlchemy** | Session+映射+dirty checking | 运行时类型检查 | 20-600% |

### 详细说明

**[C++ 本项目]**
- 连接池获取与归还（线程锁竞争）
- 语句缓存查找与重置（unordered_map）
- 参数绑定（循环）
- SQL执行
- 结果行→struct映射

**[Python SQLAlchemy]**
- Session建立/销毁
- SQL构建（query builder开销大，实测30x）
- 参数绑定
- 执行SQL
- 结果→Python对象映射（解释器开销）
- 一级缓存/dirty checking（每次commit前扫描所有实体）
- 缓存管理

**[Java Hibernate]**
- EntityManager/Session管理
- 一级缓存(PersistenceContext)：保存所有加载的实体
- identity map：保证同一session中实体唯一性
- 脏检查(dirty checking)：commit前比较每个实体vs快照
- 级联操作：自动处理关联实体
- 懒加载：首次访问触发额外SQL
- 对象状态转换

**[Go ORMs]**
- 连接池
- SQL执行
- struct映射（使用reflection）
- 相对简单，无复杂缓存机制

**[Rust Diesel]**
- 类型映射（编译期）
- 查询构建（~50%开销）
- 执行
- struct映射
- 零成本抽象：不引入运行时开销

---

## 1. Python ORMs

> 测试范围: Session + SQL构建 + 映射 + dirty checking + 运行时类型检查
> 
> **开销大的原因**: Python解释器+动态类型+dirty checking扫描全部实体+对象创建开销

### SQLAlchemy vs Raw SQL (来源: NahidScript, BSWEN)

| 操作 | Raw SQL | SQLAlchemy | 开销 |
|------|--------|------------|------|
| 简单SELECT | 1.0x | ~2.0x | ~100% |
| INSERT 单条 | 1.0x | ~1.2x | ~20% |
| INSERT 批量10000 | 1.0x | ~7.0x | ~600% |
| 复杂JOIN | 1.0x | ~3-5x | 200-400% |

### Python ORM排名 (来源: tortoise/orm-benchmarks)

| 排名 | ORM | 开销 | 备注 |
|------|-----|------|------|
| 1 | Tortoise ORM | 低 | async优化好 |
| 2 | Piccolo | 低 | 薄映射层 |
| 3 | Django/peewee | 中 | 同步优化 |
| 4 | SQLAlchemy async | 高 | Session+UOW开销 |
| 5 | ormar | 极高 | Pydantic验证 |

### Python ORM测试排名 (GitHub: tortoise/orm-benchmarks)

> 测试包含: 完整ORM操作

- **最快**: Tortoise ORM (async), Piccolo
- **中等**: Django, peewee, SQLObject
- **最慢**: SQLAlchemy async, SQLModel, ormar (Pydantic验证开销)

---

## 2. Java ORMs

> 测试范围: Session + 一级缓存 + 脏检查 + identity map + lazy loading
> 
> **开销大的原因**: 一级缓存扫描+脏检查所有实体+identity比较+GC压力

### Hibernate vs JDBC (来源: TheLinuxCode, Baeldung)

| 操作 | JDBC | Hibernate | 开销 |
|------|------|-----------|------|
| 简单INSERT | 2-5ms | 5-12ms | 100-140% |
| 简单SELECT | 2-5ms | 5-12ms | 100-140% |
| 批量INSERT | 100% | 200%+ | 100%+ |

**关键开销点**:
- 每次commit前：扫描所有实体比较脏状态
- 一级缓存：所有加载实体保存在内存
- lazy loading：访问关联属性触发额外SQL（N+1问题）
- 对象转换：Java反射+字节码增强

### Java ORM benchmark (来源: pponec/orm-benchmarks, 50万次迭代)

| 框架 | 批量Insert | 内存/次 |
|------|-----------|---------|
| Ujorm3 | 1.30s | 11,875 B |
| JDBI | 1.55s | 13,664 B |
| MyBatis | 1.67s | 12,841 B |
| **Hibernate** | **2.67s** | **16,348 B** | Session+脏检查+identity |
| Exposed | 3.55s | 23,317 B | SQL-based DSL |
| Spring JDBC | 5.69s | 23,534 B | 轻量级映射 |

---

## 3. Go ORMs

> 测试范围: 连接池 + 基本CRUD + struct映射（最简化）
> 
> **开销低的原因**: 无复杂缓存/dirty checking + 静态编译 + 简单反射

### GORM vs Raw SQL (来源: frederikhors/orm-benchmark)

| 操作 | Raw SQL | GORM | 开销 |
|------|--------|-----|------|
| INSERT | 100% | 100-110% | 0-10% |
| MultiInsert 100行 | 100% | 100-103% | 0-3% |
| SELECT | 100% | 105-120% | 5-20% |
| Update | 100% | 100-108% | 0-8% |

### 测试结果 (4000次迭代)
```
raw_stmt:     2.62s    656011 ns/op   773 B/op
gorm:        2.69s    673000 ns/op  5080 B/op    ← 7%开销
pg:          2.84s    710984 ns/op  1000 B/op    ← 8%开销
```

**结论**: Go ORM开销极低，因为：
- 无dirty checking
- 无一级缓存
- 简化对象映射
- 静态编译，无GC大暂停

**结论**: Go ORM开销很低，这与Go语言特性相关（静态编译、无GC压力）

---

## 4. 与其他语言的对比总结

| 语言 | ORM | 开销范围 | 备注 |
|------|-----|---------|------|
| **C++ (本项目)** | 自定义ORM | **20-70%** | 连接池+语句缓存+映射 |
| **Go** | GORM/Bun | 0-20% | 连接池+简化映射 |
| **Rust** | Diesel | 5-20% | 编译期优化+零抽象 |
| **Java** | Hibernate | 50-140% | Session+脏检查+identity |
| **Python** | SQLAlchemy | 20-600% | 解释器+dirty checking |
| **C#/.NET** | EF Core | 30-80% | Change Tracker |

### 各语言/库详情

| 语言 | ORM | 开销 | 备注 |
|------|-----|------|------|
| Rust | Diesel | 5-20% | ORM模式 |
| Rust | SQLx | ~0% | 仅执行 |
| Go | GORM/Bun | 0-15% | 简化映射 |
| Java | JDBI | 10-30% | 轻量查询 |
| Java | MyBatis | 20-40% | XML映射 |
| Python | peewee | 20-80% | 轻量ORM |
| Python | Django | 30-100% | 完整框架 |
| C# | EF Core | 30-80% | Change Tracker |

---

## 5. 与业界对比结论

### 各语言ORM性能排名

| 排名 | 语言/ORM | 开销 | 核心原因 |
|------|----------|------|----------|
| 1 | Go GORM | 0-20% | 无缓存/dirty checking+静态 |
| 2 | Rust Diesel | 5-20% | 编译期优化+零成本抽象 |
| 3 | **本项目C++** | **20-70%** | 连接池+语句缓存+映射 |
| 4 | .NET EF Core | 30-80% | Change Tracker |
| 5 | Java Hibernate | 50-140% | Session+脏检查+identity |
| 6 | Python SQLAlchemy | 20-600% | 解释器+dirty checking+动态 |

### 本项目C++ ORM性能位置

在所有测试语言中排名 **第3位**（共6个），处于中上水平。

**为什么是这个位置**：

| 开销来源 | 本项目C++ | Go | Java | Python |
|----------|-----------|-----|------|--------|
| 连接池 | ✅ | ✅ | ✅ | ✅ |
| 语句缓存 | ✅ | ✅ | ❌ | ✅ |
| 结果映射 | ✅ | 简化 | 简化 | 复杂 |
| dirty checking | ❌ | ❌ | ✅ | ✅ |
| 一级缓存 | ❌ | ❌ | ✅ | ✅ |
| lazy loading | ❌ | ❌ | ✅ | ✅ |
| 运行时类型 | ❌ | ❌ | ❌ | ✅ |

✅ = 有此开销，❌ = 无此开销

**结论**: 本项目比Go/Rust多了**语句缓存**（需要查找+重置），比Java/Python少了**dirty checking和缓存扫描**，因此在中间位置。

### 分析

1. **C++/Rust**: 编译期优化、静态类型、无GC，框架开销最低
2. **Go**: 静态特性+简单runtime，无复杂缓存
3. **Java**: Session管理+脏检查+identity map，额外开销
4. **Python**: 动态解释+对象映射+dirty checking，开销最高

### 注意事项

1. 本项目SQLite3使用裸API（无连接池），生产环境都加连接池后差距会缩小
2. 各语言测试环境不同，数据仅供参考
---

## 6. 引用来源

- Python: tortoise/orm-benchmarks, Gold Lapel, BSWEN, NahidScript
- Java: pponec/orm-benchmarks, hibernate/hibernate-orm-benchmark, Baeldung
- Go: frederikhors/orm-benchmark, efectn/go-orm-benchmarks
- Rust: dev.to/ diesel vs sqlx, medium, github hariria/rust-orm-bench
