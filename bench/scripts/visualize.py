#!/usr/bin/env python3
"""
ESS ORM Benchmark 可视化工具
用法: python3 visualize.py bench_results.json
"""

import json
import sys
from collections import defaultdict

# 检查是否安装了 matplotlib
try:
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False
    print("提示: 安装 matplotlib 可以生成图表")
    print("  pip3 install matplotlib")


def parse_results(filepath: str) -> dict:
    """解析 benchmark JSON 结果"""
    with open(filepath) as f:
        data = json.load(f)
    
    results = defaultdict(dict)
    
    for bench in data['benchmarks']:
        name = bench['name']
        time_ns = bench['real_time']
        
        # 解析名称
        if 'Native' in name:
            test_name = name.replace('BM_Native_', '').replace('Native_', '')
            test_name = test_name.replace('SelectFixture/', '')
            results[test_name]['native'] = time_ns
        elif 'ORM' in name:
            test_name = name.replace('BM_ORM_', '').replace('ORM_', '')
            test_name = test_name.replace('SelectFixture/', '')
            results[test_name]['orm'] = time_ns
    
    return dict(results)


def print_table(results: dict):
    """打印文本表格"""
    print("\n" + "=" * 80)
    print("                      ESS ORM Benchmark Results")
    print("=" * 80)
    print(f"{'Test':<30} {'Native':>12} {'ORM':>12} {'Overhead':>12} {'Status'}")
    print("-" * 80)
    
    total_native = 0
    total_orm = 0
    count = 0
    
    for name, times in sorted(results.items()):
        if 'native' in times and 'orm' in times:
            native = times['native']
            orm = times['orm']
            overhead = ((orm - native) / native) * 100
            
            # 格式化时间
            native_str = format_time(native)
            orm_str = format_time(orm)
            
            # 状态指示
            if overhead <= 0:
                status = "✅ 更快"
            elif overhead < 15:
                status = "✅ 可接受"
            elif overhead < 30:
                status = "⚠️  注意"
            else:
                status = "❌ 较慢"
            
            print(f"{name:<30} {native_str:>12} {orm_str:>12} {overhead:>+10.1f}%  {status}")
            
            total_native += native
            total_orm += orm
            count += 1
        
        elif 'orm' in times:
            # 只有 ORM 结果（如 ReadTransaction_Empty, NoTx 版本）
            orm_str = format_time(times['orm'])
            print(f"{name:<30} {'N/A':>12} {orm_str:>12} {'N/A':>12}  ℹ️  仅ORM")
    
    print("-" * 80)
    if count > 0:
        avg_overhead = ((total_orm - total_native) / total_native) * 100
        color = "✅" if avg_overhead < 15 else "⚠️" if avg_overhead < 30 else "❌"
        print(f"{'平均开销':<30} {format_time(total_native):>12} {format_time(total_orm):>12} {avg_overhead:>+10.1f}%  {color}")
    print("=" * 80)


def format_time(ns: float) -> str:
    """格式化时间"""
    if ns >= 1_000_000:
        return f"{ns/1_000_000:.2f} ms"
    elif ns >= 1_000:
        return f"{ns/1_000:.2f} µs"
    else:
        return f"{ns:.0f} ns"


def plot_comparison(results: dict, output_file: str = "benchmark_chart.png"):
    """生成对比图表"""
    if not HAS_MATPLOTLIB:
        print("跳过图表生成（需要 matplotlib）")
        return
    
    # 筛选有对比数据的测试
    comparison_data = {k: v for k, v in results.items() 
                       if 'native' in v and 'orm' in v}
    
    if not comparison_data:
        print("没有可对比的数据")
        return
    
    names = list(comparison_data.keys())
    native_times = [comparison_data[n]['native'] / 1000 for n in names]  # 转换为 µs
    orm_times = [comparison_data[n]['orm'] / 1000 for n in names]
    
    # 计算开销百分比
    overheads = [(o - n) / n * 100 for n, o in zip(native_times, orm_times)]
    
    # 创建图表
    fig, axes = plt.subplots(2, 1, figsize=(14, 10))
    
    # ============ 图1: 时间对比柱状图 ============
    ax1 = axes[0]
    x = range(len(names))
    width = 0.35
    
    bars1 = ax1.bar([i - width/2 for i in x], native_times, width, 
                     label='Native SQLite', color='#2196F3', alpha=0.8)
    bars2 = ax1.bar([i + width/2 for i in x], orm_times, width, 
                     label='ESS ORM', color='#4CAF50', alpha=0.8)
    
    ax1.set_ylabel('Time (µs)', fontsize=12)
    ax1.set_title('ESS ORM vs Native SQLite Performance', fontsize=14, fontweight='bold')
    ax1.set_xticks(x)
    ax1.set_xticklabels(names, rotation=45, ha='right', fontsize=10)
    ax1.legend(loc='upper left')
    ax1.set_yscale('log')  # 对数刻度，因为时间差异大
    ax1.grid(axis='y', alpha=0.3)
    
    # 添加数值标签
    for bar, val in zip(bars1, native_times):
        ax1.annotate(f'{val:.1f}', xy=(bar.get_x() + bar.get_width()/2, bar.get_height()),
                     xytext=(0, 3), textcoords='offset points', ha='center', fontsize=8)
    for bar, val in zip(bars2, orm_times):
        ax1.annotate(f'{val:.1f}', xy=(bar.get_x() + bar.get_width()/2, bar.get_height()),
                     xytext=(0, 3), textcoords='offset points', ha='center', fontsize=8)
    
    # ============ 图2: 开销百分比 ============
    ax2 = axes[1]
    colors = ['#4CAF50' if o <= 0 else '#8BC34A' if o < 15 else '#FFC107' if o < 30 else '#F44336' 
              for o in overheads]
    
    bars3 = ax2.bar(x, overheads, color=colors, alpha=0.8, edgecolor='black', linewidth=0.5)
    ax2.axhline(y=0, color='black', linestyle='-', linewidth=1)
    ax2.axhline(y=15, color='orange', linestyle='--', linewidth=1, alpha=0.7, label='15% threshold')
    ax2.axhline(y=-15, color='green', linestyle='--', linewidth=1, alpha=0.7)
    
    ax2.set_ylabel('Overhead (%)', fontsize=12)
    ax2.set_title('ORM Overhead Relative to Native SQLite', fontsize=14, fontweight='bold')
    ax2.set_xticks(x)
    ax2.set_xticklabels(names, rotation=45, ha='right', fontsize=10)
    ax2.grid(axis='y', alpha=0.3)
    
    # 添加百分比标签
    for bar, val in zip(bars3, overheads):
        y_pos = bar.get_height()
        ax2.annotate(f'{val:+.1f}%', 
                     xy=(bar.get_x() + bar.get_width()/2, y_pos),
                     xytext=(0, 5 if y_pos >= 0 else -15), 
                     textcoords='offset points', 
                     ha='center', fontsize=9, fontweight='bold')
    
    # 图例
    legend_elements = [
        mpatches.Patch(color='#4CAF50', label='ORM 更快 (≤0%)'),
        mpatches.Patch(color='#8BC34A', label='可接受 (<15%)'),
        mpatches.Patch(color='#FFC107', label='注意 (15-30%)'),
        mpatches.Patch(color='#F44336', label='较慢 (>30%)')
    ]
    ax2.legend(handles=legend_elements, loc='upper right')
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    print(f"\n图表已保存到: {output_file}")
    
    # 显示图表
    plt.show()


def plot_detailed(results: dict, output_file: str = "benchmark_detailed.png"):
    """生成详细分类图表"""
    if not HAS_MATPLOTLIB:
        return
    
    # 分类
    categories = {
        'Insert': ['InsertOne', 'InsertBatch_100', 'InsertBatch_1000'],
        'Select': ['SelectAll', 'SelectOne'],
        'Transaction': ['Transaction_Empty', 'Transaction_10Ops', 'NestedTransaction'],
        'Update': ['Update']
    }
    
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    axes = axes.flatten()
    
    for idx, (category, tests) in enumerate(categories.items()):
        ax = axes[idx]
        
        test_names = []
        native_vals = []
        orm_vals = []
        
        for test in tests:
            if test in results and 'native' in results[test] and 'orm' in results[test]:
                test_names.append(test)
                native_vals.append(results[test]['native'] / 1000)  # µs
                orm_vals.append(results[test]['orm'] / 1000)
        
        if not test_names:
            continue
        
        x = range(len(test_names))
        width = 0.35
        
        ax.bar([i - width/2 for i in x], native_vals, width, label='Native', color='#2196F3', alpha=0.8)
        ax.bar([i + width/2 for i in x], orm_vals, width, label='ORM', color='#4CAF50', alpha=0.8)
        
        ax.set_title(f'{category} Operations', fontsize=12, fontweight='bold')
        ax.set_ylabel('Time (µs)')
        ax.set_xticks(x)
        ax.set_xticklabels(test_names, rotation=30, ha='right')
        ax.legend()
        ax.grid(axis='y', alpha=0.3)
        
        if max(native_vals + orm_vals) > 10 * min(native_vals + orm_vals):
            ax.set_yscale('log')
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    print(f"详细图表已保存到: {output_file}")


def print_summary(results: dict):
    """打印总结"""
    comparison = {k: v for k, v in results.items() if 'native' in v and 'orm' in v}
    
    if not comparison:
        return
    
    faster = []
    acceptable = []
    attention = []
    slower = []
    
    for name, times in comparison.items():
        overhead = (times['orm'] - times['native']) / times['native'] * 100
        if overhead <= 0:
            faster.append((name, overhead))
        elif overhead < 15:
            acceptable.append((name, overhead))
        elif overhead < 30:
            attention.append((name, overhead))
        else:
            slower.append((name, overhead))
    
    print("\n" + "=" * 60)
    print("                      性能总结")
    print("=" * 60)
    
    if faster:
        print(f"\n✅ ORM 更快 ({len(faster)} 项):")
        for name, overhead in faster:
            print(f"   • {name}: {overhead:+.1f}%")
    
    if acceptable:
        print(f"\n✅ 可接受开销 <15% ({len(acceptable)} 项):")
        for name, overhead in acceptable:
            print(f"   • {name}: {overhead:+.1f}%")
    
    if attention:
        print(f"\n⚠️  需注意 15-30% ({len(attention)} 项):")
        for name, overhead in attention:
            print(f"   • {name}: {overhead:+.1f}%")
    
    if slower:
        print(f"\n❌ 较慢 >30% ({len(slower)} 项):")
        for name, overhead in slower:
            print(f"   • {name}: {overhead:+.1f}%")
    
    # 平均
    total_overhead = sum((v['orm'] - v['native']) / v['native'] * 100 
                         for v in comparison.values())
    avg = total_overhead / len(comparison)
    
    print("\n" + "-" * 60)
    print(f"平均开销: {avg:+.1f}%")
    
    if avg <= 0:
        print("结论: 🎉 ORM 整体性能优于原生 API！")
    elif avg < 15:
        print("结论: ✅ ORM 开销在可接受范围内")
    elif avg < 30:
        print("结论: ⚠️  ORM 有一定开销，建议优化热点路径")
    else:
        print("结论: ❌ ORM 开销较大，需要优化")
    
    print("=" * 60)


def parse_text_output(text: str) -> dict:
    """从文本输出解析结果（备用方案）"""
    results = defaultdict(dict)
    
    for line in text.strip().split('\n'):
        if 'ns' not in line or 'Benchmark' in line:
            continue
        
        parts = line.split()
        if len(parts) < 3:
            continue
        
        name = parts[0]
        try:
            time_ns = float(parts[1])
        except ValueError:
            continue
        
        if 'Native' in name:
            test_name = name.replace('BM_Native_', '').replace('Native_', '')
            test_name = test_name.replace('SelectFixture/', '')
            results[test_name]['native'] = time_ns
        elif 'ORM' in name:
            test_name = name.replace('BM_ORM_', '').replace('ORM_', '')
            test_name = test_name.replace('SelectFixture/', '')
            results[test_name]['orm'] = time_ns
    
    return dict(results)


def main():
    if len(sys.argv) < 2:
        print("用法:")
        print("  python3 visualize.py bench_results.json")
        print("  python3 visualize.py --text  (然后粘贴文本输出)")
        sys.exit(1)
    
    if sys.argv[1] == '--text':
        print("请粘贴 benchmark 输出，然后按 Ctrl+D (Linux/Mac) 或 Ctrl+Z (Windows):")
        text = sys.stdin.read()
        results = parse_text_output(text)
    else:
        results = parse_results(sys.argv[1])
    
    if not results:
        print("未找到有效的 benchmark 数据")
        sys.exit(1)
    
    # 打印表格
    print_table(results)
    
    # 打印总结
    print_summary(results)
    
    # 生成图表
    if HAS_MATPLOTLIB:
        plot_comparison(results)
        plot_detailed(results)


if __name__ == '__main__':
    main()
