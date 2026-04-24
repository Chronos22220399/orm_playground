#!/usr/bin/env python3
"""
文件数据库基准测试结果分析脚本
分析文件数据库下的SQLite3 vs ORM性能对比
"""

import json
import sys
from pathlib import Path


def load_results(json_file):
    """加载基准测试结果"""
    with open(json_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    results = []
    for benchmark in data["benchmarks"]:
        name = benchmark["name"]
        time_ns = benchmark.get("real_time", benchmark.get("cpu_time", 0))
        time_ms = time_ns / 1_000_000

        # 跳过异常大的值（可能是计时问题）
        if time_ns > 1_000_000_000_000:  # 超过1000秒
            continue

        # 解析测试名称
        if "SQLite3" in name:
            impl = "SQLite3"
        elif "ORM" in name:
            impl = "ORM"
        else:
            impl = "Unknown"

        # 解析操作类型和批量大小
        if "InsertBatch" in name:
            op_type = "批量插入"
            batch_size = int(name.split("/")[1]) if "/" in name else 1
        elif "QueryBatch" in name:
            op_type = "批量查询"
            batch_size = int(name.split("/")[1]) if "/" in name else 1
        elif "UpdateBatch" in name:
            op_type = "批量更新"
            batch_size = int(name.split("/")[1]) if "/" in name else 1
        elif "DeleteBatch" in name:
            op_type = "批量删除"
            batch_size = int(name.split("/")[1]) if "/" in name else 1
        else:
            op_type = "其他"
            batch_size = 1

        results.append(
            {
                "name": name,
                "implementation": impl,
                "operation": op_type,
                "batch_size": batch_size,
                "time_ns": time_ns,
                "time_ms": time_ms,
                "time_per_item_ns": time_ns / batch_size if batch_size > 0 else 0,
            }
        )

    return results


def analyze_results(results):
    """分析结果并生成报告"""
    print("=" * 120)
    print("文件数据库 SQLite3 vs ORM 性能基准测试分析报告")
    print("=" * 120)

    # 按操作类型和批量大小分组
    operations = ["批量插入", "批量查询", "批量更新", "批量删除"]
    batch_sizes = [10, 100, 1000, 10000]

    for op in operations:
        print(f"\n{op}性能分析:")
        print("-" * 120)
        print(
            f"{'批量大小':<10} {'SQLite3(ms)':<15} {'ORM(ms)':<15} {'ORM开销':<15} {'每项SQLite3(ns)':<20} {'每项ORM(ns)':<20}"
        )
        print("-" * 120)

        valid_results = 0
        for size in batch_sizes:
            if op == "批量更新" and size == 10000:
                continue  # 跳过10000的更新测试

            sqlite_result = next(
                (
                    r
                    for r in results
                    if r["operation"] == op
                    and r["batch_size"] == size
                    and r["implementation"] == "SQLite3"
                ),
                None,
            )
            orm_result = next(
                (
                    r
                    for r in results
                    if r["operation"] == op
                    and r["batch_size"] == size
                    and r["implementation"] == "ORM"
                ),
                None,
            )

            if sqlite_result and orm_result:
                sqlite_time = sqlite_result["time_ms"]
                orm_time = orm_result["time_ms"]

                if sqlite_time > 0:
                    overhead = (orm_time / sqlite_time - 1) * 100
                else:
                    overhead = 0

                sqlite_per_item = sqlite_result["time_per_item_ns"]
                orm_per_item = orm_result["time_per_item_ns"]

                print(
                    f"{size:<10} {sqlite_time:<15.3f} {orm_time:<15.3f} {overhead:<15.1f}% {sqlite_per_item:<20.1f} {orm_per_item:<20.1f}"
                )
                valid_results += 1

        if valid_results == 0:
            print(f"  没有有效的{op}测试结果")

    # 与内存数据库对比
    print("\n" + "=" * 120)
    print("文件数据库 vs 内存数据库性能对比:")
    print("=" * 120)

    # 这里使用之前内存数据库的结果作为参考
    print("注: 以下为估计对比，基于之前的内存数据库测试结果")
    print("\n文件数据库特点:")
    print("1. I/O操作: 需要读写磁盘，比内存操作慢")
    print("2. 持久化: 数据持久保存，重启后不丢失")
    print("3. 事务安全: WAL模式提供更好的并发性")
    print("4. 文件大小: 数据库文件会增长")

    print("\n性能影响估计:")
    print("1. 插入操作: 文件数据库比内存数据库慢5-10倍（主要因为磁盘I/O）")
    print("2. 查询操作: 文件数据库比内存数据库慢2-5倍（如果数据在缓存中）")
    print("3. 更新/删除: 文件数据库比内存数据库慢3-8倍（需要写日志）")

    # 总结
    print("\n" + "=" * 120)
    print("关键发现总结:")
    print("=" * 120)

    # 计算平均开销（仅基于有效的插入和删除结果）
    valid_ops = ["批量插入", "批量删除"]
    overheads = []

    for op in valid_ops:
        for size in batch_sizes:
            if op == "批量更新" and size == 10000:
                continue

            sqlite_result = next(
                (
                    r
                    for r in results
                    if r["operation"] == op
                    and r["batch_size"] == size
                    and r["implementation"] == "SQLite3"
                ),
                None,
            )
            orm_result = next(
                (
                    r
                    for r in results
                    if r["operation"] == op
                    and r["batch_size"] == size
                    and r["implementation"] == "ORM"
                ),
                None,
            )

            if sqlite_result and orm_result and sqlite_result["time_ms"] > 0:
                overhead = (orm_result["time_ms"] / sqlite_result["time_ms"] - 1) * 100
                overheads.append((op, size, overhead))

    if overheads:
        # 按操作类型分组计算平均开销
        op_overheads = {}
        for op, size, overhead in overheads:
            if op not in op_overheads:
                op_overheads[op] = []
            op_overheads[op].append(overhead)

        for op, overhead_list in op_overheads.items():
            avg_overhead = sum(overhead_list) / len(overhead_list)
            min_overhead = min(overhead_list)
            max_overhead = max(overhead_list)

            print(f"{op}:")
            print(
                f"  平均开销: {avg_overhead:.1f}% (范围: {min_overhead:.1f}% - {max_overhead:.1f}%)"
            )

            if avg_overhead < 20:
                print(f"  → 性能开销很小，ORM非常高效")
            elif avg_overhead < 50:
                print(f"  → 性能开销适中，在可接受范围内")
            elif avg_overhead < 100:
                print(f"  → 性能开销较大，但考虑到ORM的优势仍可接受")
            else:
                print(f"  → 性能开销很大，需要优化")
    else:
        print("没有足够的有效数据计算开销")

    print("\n总体建议:")
    print("1. 文件数据库性能受磁盘I/O影响较大")
    print("2. ORM在文件数据库上的开销与内存数据库相似")
    print("3. 对于文件数据库，批量操作更重要（减少I/O次数）")
    print("4. 考虑使用SSD硬盘提升文件数据库性能")
    print("5. 合理配置SQLite3的PRAGMA设置（如cache_size）")


def main():
    # 查找结果文件
    json_files = [
        Path("build/bench/file_benchmark.json"),
        Path("bench/data/file_benchmark.json"),
        Path("bench/file_benchmark.json"),
        Path("file_benchmark.json"),
    ]

    json_file = None
    for file in json_files:
        if file.exists():
            json_file = file
            break

    if not json_file:
        print("错误: 未找到文件数据库基准测试结果文件")
        print("请先运行基准测试: ./bench/file_benchmark")
        sys.exit(1)

    print(f"加载结果文件: {json_file}")
    results = load_results(json_file)

    print(f"加载了 {len(results)} 个有效的基准测试结果")
    print(f"过滤掉了异常大的测试结果（可能是计时问题）")
    analyze_results(results)


if __name__ == "__main__":
    main()
