# SQLite3 vs ORM 性能基准测试

本基准测试用于比较原生SQLite3 API与自定义ORM库的性能差异。

## 测试内容

基准测试覆盖以下操作：

### 1. 插入操作

- **插入单个记录**：测试单条记录插入性能
- **批量插入**：测试10、100、1000条记录的批量插入性能
- **事务批量插入**：在事务中批量插入记录

### 2. 查询操作

- **查询单个记录**：随机查询单个用户
- **批量查询**：查询1、10、100、1000条记录

### 3. 更新和删除操作

- **更新记录**：随机更新用户姓名
- **删除记录**：顺序删除记录

## 测试配置

- **数据库**：使用共享内存数据库 `file::memory:?cache=shared`
- **测试数据**：随机生成的用户数据（姓名、年龄、分数、活跃状态）
- **实现对比**：
  - **SQLite3原生API**：直接使用SQLite3 C API
  - **ORM库**：使用自定义的C++ ORM库

## 使用方法

### 1. 构建基准测试

```bash
cd /Users/wuming/code/orm_playground
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make sqlite_vs_orm_benchmark
```

### 2. 运行基准测试

#### 方法一：使用脚本（推荐）

```bash
cd build
../bench/run_benchmark.sh
```

#### 方法二：手动运行

```bash
cd build
./bench/sqlite_vs_orm_benchmark
```

### 3. 可视化结果

基准测试会自动生成JSON格式的结果文件 `bench/results.json`。

安装Python依赖：

```bash
pip install -r bench/requirements.txt
```

生成可视化图表：

```bash
cd bench
python3 visualize_results.py
```

图表将保存到 `bench/visualization/` 目录。

## 输出文件

- `bench/results.json`：原始基准测试结果（JSON格式）
- `bench/visualization/`：生成的图表和报告
  - `single_operations.png`：单个操作性能对比
  - `batch_insert_performance.png`：批量插入性能对比
  - `batch_query_performance.png`：批量查询性能对比
  - `transaction_performance.png`：事务性能对比
  - `performance_radar.png`：性能加速比雷达图
  - `benchmark_report.txt`：文本格式的汇总报告

## 测试指标

### 性能指标

1. **执行时间**：每个操作的执行时间（毫秒）
2. **吞吐量**：每秒处理的操作数
3. **加速比**：ORM相对于SQLite3的性能提升倍数

### 复杂度分析

- **O(1)操作**：单个插入、查询、更新、删除
- **O(n)操作**：批量插入、批量查询
- **事务开销**：事务开启/提交的开销

## 预期结果

### 理论预期

1. **SQLite3原生API**：
   - 优势：直接调用，无额外开销
   - 劣势：需要手动管理资源，代码复杂

2. **ORM库**：
   - 优势：类型安全，代码简洁，自动资源管理
   - 劣势：可能有额外的抽象层开销

### 实际关注点

1. **ORM抽象开销**：ORM相对于原生API的性能损失
2. **批量操作效率**：ORM的批量操作优化效果
3. **事务性能**：ORM事务管理的效率
4. **内存使用**：ORM的对象映射内存开销

## 自定义测试

### 修改测试参数

编辑 `bench/sqlite_vs_orm_benchmark.cpp`：

1. **修改测试数据量**：

   ```cpp
   // 在 prepare_test_data 函数中修改
   prepare_test_data(5000);  // 改为5000条测试数据
   ```

2. **添加新的测试用例**：
   ```cpp
   static void BM_CustomTest(benchmark::State& state) {
       // 自定义测试逻辑
   }
   BENCHMARK(BM_CustomTest);
   ```

### 调整基准测试参数

运行时可指定参数：

```bash
./bench/sqlite_vs_orm_benchmark \
    --benchmark_min_time=0.5 \
    --benchmark_repetitions=10 \
    --benchmark_out=custom_results.json
```

## 故障排除

### 常见问题

1. **编译错误：找不到Google Benchmark**

   ```bash
   # 安装Google Benchmark
   brew install google-benchmark
   ```

2. **Python依赖安装失败**

   ```bash
   # 使用虚拟环境
   python3 -m venv venv
   source venv/bin/activate
   pip install -r requirements.txt
   ```

3. **内存数据库连接问题**
   - 确保使用 `file::memory:?cache=shared` 共享内存
   - 检查连接池配置是否正确

4. **测试数据重复**
   - 基准测试会自动清理和重置测试数据
   - 如果遇到唯一约束错误，检查 `clear_table()` 函数

### 调试建议

1. **启用详细输出**：

   ```bash
   ./bench/sqlite_vs_orm_benchmark --benchmark_display_aggregates_only=true
   ```

2. **分析特定测试**：

   ```bash
   ./bench/sqlite_vs_orm_benchmark --benchmark_filter="Insert"
   ```

3. **性能分析**：
   ```bash
   # 使用perf工具（Linux）
   perf record ./bench/sqlite_vs_orm_benchmark
   perf report
   ```

## 贡献指南

1. 添加新的测试用例时，确保同时测试SQLite3和ORM两种实现
2. 保持测试数据的随机性和代表性
3. 为新的测试添加适当的文档说明
4. 更新可视化脚本以支持新的图表类型

## 许可证

本项目使用MIT许可证。详见项目根目录的LICENSE文件。
