#!/usr/bin/env python3
"""
图表可视化脚本 - 生成直观的性能对比图表
"""

import json
import os
import sys
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, List, Tuple, Any


def load_results(json_file: str) -> List[Dict[str, Any]]:
    """加载基准测试结果"""
    with open(json_file, "r") as f:
        data = json.load(f)
    return data.get("benchmarks", [])


def extract_benchmark_data(
    results: List[Dict[str, Any]],
) -> Dict[str, Dict[str, List[Tuple[int, float]]]]:
    """从结果中提取基准测试数据"""
    data = {
        "insert": {"sqlite": [], "orm": []},
        "query": {"sqlite": [], "orm": []},
        "update": {"sqlite": [], "orm": []},
        "delete": {"sqlite": [], "orm": []},
    }

    for result in results:
        name = result.get("name", "")
        cpu_time = result.get("cpu_time", 0)

        # 跳过聚合结果
        if "BigO" in name or "RMS" in name:
            continue

        # 提取批量大小
        if "/" in name:
            parts = name.split("/")
            if len(parts) >= 2:
                try:
                    batch_size = int(parts[1])
                except ValueError:
                    continue
            else:
                continue
        else:
            continue

        # 转换为毫秒
        time_ms = cpu_time / 1_000_000

        # 分类数据
        if "InsertBatch" in name:
            if "SQLite3" in name:
                data["insert"]["sqlite"].append((batch_size, time_ms))
            elif "ORM" in name:
                data["insert"]["orm"].append((batch_size, time_ms))
        elif "QueryBatch" in name:
            if "SQLite3" in name:
                data["query"]["sqlite"].append((batch_size, time_ms))
            elif "ORM" in name:
                data["query"]["orm"].append((batch_size, time_ms))
        elif "UpdateBatch" in name:
            if "SQLite3" in name:
                data["update"]["sqlite"].append((batch_size, time_ms))
            elif "ORM" in name:
                data["update"]["orm"].append((batch_size, time_ms))
        elif "DeleteBatch" in name:
            if "SQLite3" in name:
                data["delete"]["sqlite"].append((batch_size, time_ms))
            elif "ORM" in name:
                data["delete"]["orm"].append((batch_size, time_ms))

    # 按批量大小排序
    for op in data:
        for impl in data[op]:
            data[op][impl].sort(key=lambda x: x[0])

    return data


def create_comparison_charts(
    data: Dict[str, Dict[str, List[Tuple[int, float]]]], title_prefix: str = "性能对比"
):
    """创建性能对比图表（柱状图+折线图组合）"""

    # 设置中文字体支持
    plt.rcParams["font.sans-serif"] = ["Arial Unicode MS", "DejaVu Sans", "SimHei"]
    plt.rcParams["axes.unicode_minus"] = False

    # 创建子图
    fig, axes = plt.subplots(2, 2, figsize=(18, 14))
    fig.suptitle(
        f"{title_prefix} - SQLite3 vs ORM", fontsize=20, fontweight="bold", y=0.98
    )

    operations = [
        ("insert", "Batch Insert", axes[0, 0]),
        ("query", "Batch Query", axes[0, 1]),
        ("update", "Batch Update", axes[1, 0]),
        ("delete", "Batch Delete", axes[1, 1]),
    ]

    for op_key, op_name, ax in operations:
        if not data[op_key]["sqlite"] or not data[op_key]["orm"]:
            continue

        # Extract data
        sqlite_sizes = [x[0] for x in data[op_key]["sqlite"]]
        sqlite_times = [x[1] for x in data[op_key]["sqlite"]]
        orm_sizes = [x[0] for x in data[op_key]["orm"]]
        orm_times = [x[1] for x in data[op_key]["orm"]]

        # Ensure batch sizes match
        common_sizes = sorted(set(sqlite_sizes) & set(orm_sizes))
        if not common_sizes:
            continue

        # Reorganize data
        sqlite_times_filtered = []
        orm_times_filtered = []
        for size in common_sizes:
            sqlite_idx = sqlite_sizes.index(size)
            orm_idx = orm_sizes.index(size)
            sqlite_times_filtered.append(sqlite_times[sqlite_idx])
            orm_times_filtered.append(orm_times[orm_idx])

        # Set bar chart positions
        x = np.arange(len(common_sizes))
        width = 0.35

        # Draw bar charts
        bars1 = ax.bar(
            x - width / 2,
            sqlite_times_filtered,
            width,
            label="SQLite3",
            color="#3498db",
            alpha=0.8,
        )
        bars2 = ax.bar(
            x + width / 2,
            orm_times_filtered,
            width,
            label="ORM",
            color="#e74c3c",
            alpha=0.8,
        )

        # Calculate overhead percentage and draw line chart
        overheads = []
        for sqlite_time, orm_time in zip(sqlite_times_filtered, orm_times_filtered):
            if sqlite_time > 0:
                overhead = ((orm_time - sqlite_time) / sqlite_time) * 100
                overheads.append(overhead)
            else:
                overheads.append(0)

        # Create second Y axis for overhead percentage
        ax2 = ax.twinx()
        ax2.plot(
            x,
            overheads,
            "o-",
            color="#2ecc71",
            linewidth=3,
            markersize=8,
            label="ORM Overhead %",
            markerfacecolor="white",
            markeredgewidth=2,
        )

        # Set chart properties
        ax.set_title(f"{op_name} Performance", fontsize=16, fontweight="bold", pad=20)
        ax.set_xlabel("Batch Size", fontsize=14)
        ax.set_ylabel("Time (ms)", fontsize=14, color="#2c3e50")
        ax2.set_ylabel("ORM Overhead (%)", fontsize=14, color="#2ecc71")

        # Set X axis ticks
        ax.set_xticks(x)
        ax.set_xticklabels([str(size) for size in common_sizes], fontsize=12)

        # Set grid
        ax.grid(True, alpha=0.3, axis="y")

        # Set colors
        ax.tick_params(axis="y", labelcolor="#2c3e50")
        ax2.tick_params(axis="y", labelcolor="#2ecc71")

        # Merge legends
        lines1, labels1 = ax.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax.legend(lines1 + lines2, labels1 + labels2, loc="upper left", fontsize=11)

        # Add bar value labels
        for bar in bars1:
            height = bar.get_height()
            ax.text(
                bar.get_x() + bar.get_width() / 2.0,
                height + 0.01,
                f"{height:.1f}",
                ha="center",
                va="bottom",
                fontsize=10,
            )

        for bar in bars2:
            height = bar.get_height()
            ax.text(
                bar.get_x() + bar.get_width() / 2.0,
                height + 0.01,
                f"{height:.1f}",
                ha="center",
                va="bottom",
                fontsize=10,
            )

        # Add overhead percentage labels
        for i, overhead in enumerate(overheads):
            ax2.text(
                x[i],
                overhead + 1,
                f"{overhead:.1f}%",
                ha="center",
                va="bottom",
                fontsize=10,
                fontweight="bold",
                color="#2ecc71",
            )

        # Set Y axis range
        max_time = max(max(sqlite_times_filtered), max(orm_times_filtered))
        ax.set_ylim(0, max_time * 1.2)

        max_overhead = max(overheads) if overheads else 50
        ax2.set_ylim(min(overheads) - 10 if overheads else -10, max_overhead * 1.3)

    plt.subplots_adjust(
        left=0.08, right=0.92, top=0.92, bottom=0.08, hspace=0.3, wspace=0.3
    )
    return fig


def create_overhead_chart(data: Dict[str, Dict[str, List[Tuple[int, float]]]]):
    """Create overhead percentage chart"""

    fig, ax = plt.subplots(figsize=(12, 8))

    operations = ["insert", "query", "update", "delete"]
    op_names = ["插入", "查询", "更新", "删除"]

    # 计算平均开销
    avg_overheads = []
    for op_key, op_name in zip(operations, op_names):
        if not data[op_key]["sqlite"] or not data[op_key]["orm"]:
            avg_overheads.append(0)
            continue

        # 计算每个批量大小的开销
        overheads = []
        for sqlite_data, orm_data in zip(data[op_key]["sqlite"], data[op_key]["orm"]):
            sqlite_size, sqlite_time = sqlite_data
            orm_size, orm_time = orm_data

            if sqlite_size == orm_size and sqlite_time > 0:
                overhead = ((orm_time - sqlite_time) / sqlite_time) * 100
                overheads.append(overhead)

        if overheads:
            avg_overheads.append(np.mean(overheads))
        else:
            avg_overheads.append(0)

    # 创建柱状图
    bars = ax.bar(
        op_names, avg_overheads, color=["#2E86AB", "#A23B72", "#F18F01", "#C73E1D"]
    )

    # 设置图表属性
    ax.set_title("ORM Average Performance Overhead", fontsize=16, fontweight="bold")
    ax.set_xlabel("Operation Type", fontsize=14)
    ax.set_ylabel("Overhead (%)", fontsize=14)
    ax.grid(True, alpha=0.3, axis="y")

    # 添加数值标签
    for bar, overhead in zip(bars, avg_overheads):
        height = bar.get_height()
        ax.text(
            bar.get_x() + bar.get_width() / 2.0,
            height + 1,
            f"{overhead:.1f}%",
            ha="center",
            va="bottom",
            fontsize=12,
            fontweight="bold",
        )

    # 添加参考线
    ax.axhline(y=0, color="black", linewidth=0.8, alpha=0.5)
    ax.axhline(
        y=20, color="green", linestyle="--", linewidth=1, alpha=0.5, label="良好 (≤20%)"
    )
    ax.axhline(
        y=40,
        color="orange",
        linestyle="--",
        linewidth=1,
        alpha=0.5,
        label="可接受 (≤40%)",
    )
    ax.axhline(
        y=60, color="red", linestyle="--", linewidth=1, alpha=0.5, label="较差 (>40%)"
    )

    ax.legend(fontsize=11)
    plt.subplots_adjust(
        left=0.08, right=0.92, top=0.92, bottom=0.08, hspace=0.3, wspace=0.3
    )
    return fig


def create_scaling_chart(data: Dict[str, Dict[str, List[Tuple[int, float]]]]):
    """创建扩展性图表（柱状图对比）"""

    fig, axes = plt.subplots(2, 2, figsize=(18, 14))
    fig.suptitle(
        "每项操作时间扩展性分析 - SQLite3 vs ORM",
        fontsize=20,
        fontweight="bold",
        y=0.98,
    )

    operations = [
        ("insert", "Batch Insert", axes[0, 0]),
        ("query", "Batch Query", axes[0, 1]),
        ("update", "Batch Update", axes[1, 0]),
        ("delete", "Batch Delete", axes[1, 1]),
    ]

    for op_key, op_name, ax in operations:
        if not data[op_key]["sqlite"] or not data[op_key]["orm"]:
            continue

        # 提取数据并计算每项时间
        sqlite_sizes = []
        sqlite_per_item = []
        for size, time in data[op_key]["sqlite"]:
            if size > 0 and time > 0:
                sqlite_sizes.append(size)
                sqlite_per_item.append((time / size) * 1000)  # 转换为微秒

        orm_sizes = []
        orm_per_item = []
        for size, time in data[op_key]["orm"]:
            if size > 0 and time > 0:
                orm_sizes.append(size)
                orm_per_item.append((time / size) * 1000)  # 转换为微秒

        # 确保批量大小一致
        common_sizes = sorted(set(sqlite_sizes) & set(orm_sizes))
        if not common_sizes:
            continue

        # 重新组织数据
        sqlite_per_item_filtered = []
        orm_per_item_filtered = []
        for size in common_sizes:
            sqlite_idx = sqlite_sizes.index(size)
            orm_idx = orm_sizes.index(size)
            sqlite_per_item_filtered.append(sqlite_per_item[sqlite_idx])
            orm_per_item_filtered.append(orm_per_item[orm_idx])

        # 设置柱状图位置
        x = np.arange(len(common_sizes))
        width = 0.35

        # 绘制柱状图
        bars1 = ax.bar(
            x - width / 2,
            sqlite_per_item_filtered,
            width,
            label="SQLite3",
            color="#3498db",
            alpha=0.8,
        )
        bars2 = ax.bar(
            x + width / 2,
            orm_per_item_filtered,
            width,
            label="ORM",
            color="#e74c3c",
            alpha=0.8,
        )

        # 计算效率提升百分比（负值为开销）
        efficiency_changes = []
        for sqlite_time, orm_time in zip(
            sqlite_per_item_filtered, orm_per_item_filtered
        ):
            if sqlite_time > 0:
                change = ((orm_time - sqlite_time) / sqlite_time) * 100
                efficiency_changes.append(change)
            else:
                efficiency_changes.append(0)

        # 创建第二个Y轴用于效率变化
        ax2 = ax.twinx()
        ax2.plot(
            x,
            efficiency_changes,
            "o-",
            color="#9b59b6",
            linewidth=3,
            markersize=8,
            label="效率变化",
            markerfacecolor="white",
            markeredgewidth=2,
        )

        # 设置图表属性
        ax.set_title(
            f"{op_name} - 每项操作时间", fontsize=14, fontweight="bold", pad=15
        )
        ax.set_xlabel("Batch Size", fontsize=12)
        ax.set_ylabel("每项操作时间 (μs)", fontsize=12, color="#2c3e50")
        ax2.set_ylabel("效率变化 (%)", fontsize=12, color="#9b59b6")

        # 设置X轴刻度
        ax.set_xticks(x)
        ax.set_xticklabels([str(size) for size in common_sizes], fontsize=11)

        # 设置网格
        ax.grid(True, alpha=0.3, axis="y")

        # 设置颜色
        ax.tick_params(axis="y", labelcolor="#2c3e50")
        ax2.tick_params(axis="y", labelcolor="#9b59b6")

        # 合并图例
        lines1, labels1 = ax.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax.legend(lines1 + lines2, labels1 + labels2, loc="upper right", fontsize=10)

        # 添加柱状图数值标签
        for bar in bars1:
            height = bar.get_height()
            ax.text(
                bar.get_x() + bar.get_width() / 2.0,
                height + 0.5,
                f"{height:.0f}",
                ha="center",
                va="bottom",
                fontsize=9,
            )

        for bar in bars2:
            height = bar.get_height()
            ax.text(
                bar.get_x() + bar.get_width() / 2.0,
                height + 0.5,
                f"{height:.0f}",
                ha="center",
                va="bottom",
                fontsize=9,
            )

        # 添加效率变化标签
        for i, change in enumerate(efficiency_changes):
            color = "#e74c3c" if change > 0 else "#27ae60"  # 红色表示开销，绿色表示提升
            prefix = "+" if change > 0 else ""
            ax2.text(
                x[i],
                change + (1 if change >= 0 else -3),
                f"{prefix}{change:.1f}%",
                ha="center",
                va="bottom" if change >= 0 else "top",
                fontsize=9,
                fontweight="bold",
                color=color,
            )

        # 添加零参考线
        ax2.axhline(y=0, color="#7f8c8d", linestyle="--", linewidth=1, alpha=0.5)

        # 设置Y轴范围
        max_time = max(max(sqlite_per_item_filtered), max(orm_per_item_filtered))
        ax.set_ylim(0, max_time * 1.3)

        max_change = max(abs(min(efficiency_changes)), abs(max(efficiency_changes)))
        ax2.set_ylim(-max_change * 1.5, max_change * 1.5)

    plt.subplots_adjust(
        left=0.08, right=0.92, top=0.92, bottom=0.08, hspace=0.3, wspace=0.3
    )
    return fig


def main():
    """主函数"""

    # 检查matplotlib是否可用
    try:
        import matplotlib
    except ImportError:
        print("错误: matplotlib未安装。请运行: pip install matplotlib")
        return 1

    # 检查结果文件
    result_files = [
        ("comprehensive_results.json", "Memory Database Benchmark"),
        ("file_results.json", "File Database Benchmark"),
    ]

    for result_file, title in result_files:
        if not os.path.exists(result_file):
            print(f"警告: 结果文件不存在: {result_file}")
            continue

        print(f"正在分析: {title}")
        print(f"加载结果文件: {result_file}")

        try:
            # 加载结果
            results = load_results(result_file)
            print(f"加载了 {len(results)} 个基准测试结果")

            # 提取数据
            data = extract_benchmark_data(results)

            # 创建图表
            fig1 = create_comparison_charts(data, title)
            fig2 = create_overhead_chart(data)
            fig3 = create_scaling_chart(data)

            # 保存图表
            output_dir = "bench/charts"
            os.makedirs(output_dir, exist_ok=True)

            filename_base = os.path.basename(result_file).replace(".json", "")

            fig1.savefig(
                f"{output_dir}/{filename_base}_comparison.png",
                dpi=300,
                bbox_inches="tight",
            )
            fig2.savefig(
                f"{output_dir}/{filename_base}_overhead.png",
                dpi=300,
                bbox_inches="tight",
            )
            fig3.savefig(
                f"{output_dir}/{filename_base}_scaling.png",
                dpi=300,
                bbox_inches="tight",
            )

            print(f"图表已保存到: {output_dir}/")
            print(f"  - {filename_base}_comparison.png: 性能对比图表")
            print(f"  - {filename_base}_overhead.png: 开销百分比图表")
            print(f"  - {filename_base}_scaling.png: 扩展性分析图表")

            # 显示图表（如果可能）
            try:
                plt.show()
            except:
                print("注意: 无法显示交互式图表，图表已保存为PNG文件")

            # 关闭图表以释放内存
            plt.close("all")

        except Exception as e:
            print(f"处理文件 {result_file} 时出错: {e}")
            import traceback

            traceback.print_exc()

    print("\n✅ 图表生成完成！")
    print("使用以下命令查看图表:")
    print("  open bench/charts/  # 在Finder中打开图表目录")
    print("  ls bench/charts/*.png  # 查看生成的图表文件")

    return 0


if __name__ == "__main__":
    sys.exit(main())
