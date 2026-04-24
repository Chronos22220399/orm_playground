# ORM vs SQLite3 性能基准测试

## 快速开始

```bash
# 推荐：一步运行所有+图表+CSV
./run.sh benchmark all

# 或者分步执行
./run.sh benchmark comp     # 全面测试
./run.sh benchmark chart  # 图表
./run.sh benchmark export  # CSV
./run.sh benchmark analyze  # 分析+图表+CSV
```

## 输出文件

| 位置 | 内容 | 用途 |
|------|------|------|
| `bench/charts/*.png` | 性能图表 | 直接复制到Word |
| `bench/data/*.csv` | 表格数据 | Excel/WPS打开后复制到Word |

## 图表说明

| 图表 | 内容 |
|------|------|
| `*_comparison.png` | 柱状图：SQLite3 vs ORM执行时间(绿色折线：ORM开销%) |
| `*_overhead.png` | 柱状图：各操作平均ORM开销% |
| `*_scaling.png` | 柱状图：批量每项时间(紫色折线：效率变化%) |

## 关键结论 (comprehensive测试)

| 操作 | ORM开销 | 说明 |
|------|--------|------|
| 插入 | 44-49% | 大批量每项开销降低 |
| 查询 | 24-46% | ORM优势，开销最小 |
| 更新 | 65-73% | 开销最大 |
| 删除 | -5%~60% | 大批量ORM反而更快 |

- ORM引入20-70%性能开销，取决于操作类型
- 大批量操作降低单项目开销
- 查询是ORM强项（最低开销）
- 性能关键代码用原生SQLite3；否则ORM提供良好的开发和性能平衡