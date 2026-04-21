# ORM vs SQLite3 性能基准测试

本目录包含了一系列性能基准测试，用于比较自定义ORM库与原生SQLite3 API的性能差异。

## 基准测试类型

### 1. 基础基准测试 (`basic`)
- **文件**: `sqlite_vs_orm_benchmark.cpp`
- **描述**: 最初的基准测试版本，包含基本的CRUD操作测试
- **特点**: 测试单个操作和批量操作，包含事务测试
- **问题**: SQLite3没有充分优化，结果可能不准确

### 2. 公平基准测试 (`fair`)
- **文件**: `fair_benchmark.cpp`
- **描述**: 优化后的公平对比测试
- **特点**:
  - SQLite3使用预编译语句缓存
  - 开启SQLite3性能优化 (PRAGMA设置)
  - 使用相同的测试数据
  - 更准确的性能对比

### 3. 全面基准测试 (`comprehensive`)
- **文件**: `comprehensive_benchmark.cpp`
- **描述**: 测试不同数据规模的性能
- **特点**:
  - 测试数据规模: 10, 100, 1000, 10000条记录
  - 测试操作: 插入、查询、更新、删除
  - 分析扩展性和批量效应
  - 输出JSON格式结果

### 4. 文件数据库基准测试 (`file`)
- **文件**: `file_benchmark.cpp`
- **描述**: 使用文件数据库（非内存数据库）的性能测试
- **特点**:
  - 使用真实的文件数据库
  - ORM和SQLite3使用不同的数据库文件
  - 测试磁盘I/O对性能的影响
  - 数据持久化测试

## 快速使用

### 使用 run.sh 脚本

```bash
# 运行所有基准测试
./run.sh benchmark all

# 运行特定类型的基准测试
./run.sh benchmark basic      # 基础测试
./run.sh benchmark fair       # 公平测试
./run.sh benchmark comp       # 全面测试
./run.sh benchmark file       # 文件数据库测试

# 分析已有结果
./run.sh benchmark analyze
```

### 手动运行

```bash
# 构建所有基准测试
cd build
cmake ..
make -j4

# 运行特定基准测试
./bench/sqlite_vs_orm_benchmark      # 基础测试
./bench/fair_benchmark               # 公平测试
./bench/comprehensive_benchmark      # 全面测试
./bench/file_benchmark               # 文件数据库测试

# 带JSON输出的全面测试
./bench/comprehensive_benchmark --benchmark_out=bench/comprehensive_results.json --benchmark_out_format=json

# 分析结果
python3 bench/analyze_comprehensive.py
python3 bench/analyze_file_results.py
```

## 分析工具

### 1. `analyze_comprehensive.py`
- 分析全面基准测试的JSON结果
- 计算ORM相对于SQLite3的性能开销
- 分析不同数据规模的扩展性
- 生成易读的报告

### 2. `analyze_file_results.py`
- 分析文件数据库基准测试结果
- 比较文件数据库与内存数据库的性能差异
- 分析磁盘I/O对性能的影响
- 提供优化建议

### 3. `simple_visualize.py`
- 简单的文本格式可视化
- 不需要外部依赖（仅使用标准库）
- 快速查看关键性能指标

### 4. `visualize_results.py` (需要matplotlib等依赖)
- 完整的图表可视化
- 生成各种性能图表
- 需要安装Python依赖

## 性能结果总结

### 内存数据库性能开销（ORM vs SQLite3）
- **批量插入**: 平均 28.1% 开销
- **批量查询**: 平均 18.5% 开销  
- **批量更新**: 平均 41.6% 开销
- **批量删除**: 平均 34.9% 开销

### 文件数据库 vs 内存数据库
- **插入操作**: 文件数据库比内存数据库慢 5-10倍
- **查询操作**: 文件数据库比内存数据库慢 2-5倍
- **更新/删除**: 文件数据库比内存数据库慢 3-8倍

### 关键发现
1. **ORM在查询操作上表现最佳**：开销最小（14-23%）
2. **批量效应明显**：随着批量增大，每项操作成本下降
3. **文件数据库I/O影响大**：小批量操作受磁盘I/O影响显著
4. **ORM开销稳定**：在不同场景下保持合理的性能开销

## 使用建议

### 推荐使用ORM的场景
- 开发效率优先的项目
- 需要类型安全和编译时检查
- 小到中等规模的数据操作（<1000项）
- 查询密集型应用

### 考虑使用原生SQLite3的场景
- 性能至关重要的核心路径
- 超大规模批量操作（>10000项）
- 极低延迟要求的实时系统

### 混合策略建议
- 使用ORM处理大部分业务逻辑
- 对性能关键路径使用原生SQLite3优化
- 批量操作使用ORM的事务支持
- 根据数据规模选择合适的数据库类型

## 技术细节

### 测试配置
- **编译器**: AppleClang 16.0.0
- **SQLite3版本**: 3.43.2
- **测试平台**: macOS
- **CPU**: 8核心
- **内存**: 未限制

### 数据库设置
- **内存数据库**: `file::memory:?cache=shared`
- **文件数据库**: `bench/data/` 目录下的 `.db` 文件
- **连接池大小**: 1（确保公平对比）
- **事务模式**: WAL（Write-Ahead Logging）

### 性能优化
- SQLite3 PRAGMA设置: `synchronous=OFF`, `journal_mode=MEMORY`
- 预编译语句缓存
- 批量操作使用事务
- 合理的测试数据生成

## 扩展测试

如需进一步测试，可以考虑：
1. 并发性能测试（多线程）
2. 复杂查询性能测试（JOIN、子查询等）
3. 大数据集测试（>100万条记录）
4. 不同硬件环境测试（SSD vs HDD）
5. 网络数据库测试（如PostgreSQL对比）