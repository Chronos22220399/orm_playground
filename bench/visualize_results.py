#!/usr/bin/env python3
"""
SQLite3 vs ORM 基准测试结果可视化脚本
读取Google Benchmark生成的JSON结果并生成图表
"""

import json
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
from pathlib import Path
import argparse
import sys

# 设置中文字体和样式
plt.rcParams['font.sans-serif'] = ['Arial Unicode MS', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False
sns.set_style("whitegrid")
sns.set_palette("husl")

def load_benchmark_results(json_file):
    """加载基准测试结果"""
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    results = []
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        time_ns = benchmark.get('real_time', benchmark.get('cpu_time', 0))
        time_ms = time_ns / 1_000_000  # 转换为毫秒
        
        # 解析测试名称
        if 'SQLite3' in name:
            impl = 'SQLite3'
        elif 'ORM' in name:
            impl = 'ORM'
        else:
            impl = 'Unknown'
        
        # 解析操作类型
        if 'InsertSingle' in name:
            op_type = '插入单个'
            batch_size = 1
        elif 'InsertBatch' in name:
            op_type = '批量插入'
            # 从参数中提取批量大小
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        elif 'QuerySingle' in name:
            op_type = '查询单个'
            batch_size = 1
        elif 'QueryMultiple' in name:
            op_type = '查询多个'
            # 从参数中提取查询数量
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        elif 'Update' in name:
            op_type = '更新'
            batch_size = 1
        elif 'Delete' in name:
            op_type = '删除'
            batch_size = 1
        elif 'Transaction' in name:
            op_type = '事务批量插入'
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        else:
            op_type = '其他'
            batch_size = 1
        
        results.append({
            'name': name,
            'implementation': impl,
            'operation': op_type,
            'batch_size': batch_size,
            'time_ms': time_ms,
            'time_ns': time_ns,
            'iterations': benchmark.get('iterations', 1)
        })
    
    return pd.DataFrame(results)

def create_comparison_charts(df, output_dir):
    """创建对比图表"""
    
    # 1. 单个操作性能对比（条形图）
    single_ops = df[df['batch_size'] == 1]
    if not single_ops.empty:
        plt.figure(figsize=(12, 8))
        single_pivot = single_ops.pivot_table(
            index='operation', 
            columns='implementation', 
            values='time_ms',
            aggfunc='mean'
        ).fillna(0)
        
        ax = single_pivot.plot(kind='bar', width=0.8)
        plt.title('单个操作性能对比 (毫秒)', fontsize=16, fontweight='bold')
        plt.xlabel('操作类型', fontsize=12)
        plt.ylabel('耗时 (毫秒)', fontsize=12)
        plt.xticks(rotation=45, ha='right')
        plt.legend(title='实现方式')
        plt.tight_layout()
        plt.savefig(output_dir / 'single_operations.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # 2. 批量插入性能对比（折线图）
    batch_insert = df[df['operation'] == '批量插入']
    if not batch_insert.empty:
        plt.figure(figsize=(12, 8))
        
        for impl in batch_insert['implementation'].unique():
            impl_data = batch_insert[batch_insert['implementation'] == impl]
            impl_data = impl_data.sort_values('batch_size')
            plt.plot(impl_data['batch_size'], impl_data['time_ms'], 
                    marker='o', linewidth=2, markersize=8, label=impl)
        
        plt.title('批量插入性能对比', fontsize=16, fontweight='bold')
        plt.xlabel('批量大小', fontsize=12)
        plt.ylabel('耗时 (毫秒)', fontsize=12)
        plt.xscale('log')
        plt.yscale('log')
        plt.grid(True, which="both", ls="--", alpha=0.5)
        plt.legend(title='实现方式')
        plt.tight_layout()
        plt.savefig(output_dir / 'batch_insert_performance.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # 3. 查询性能对比（折线图）
    batch_query = df[df['operation'] == '查询多个']
    if not batch_query.empty:
        plt.figure(figsize=(12, 8))
        
        for impl in batch_query['implementation'].unique():
            impl_data = batch_query[batch_query['implementation'] == impl]
            impl_data = impl_data.sort_values('batch_size')
            plt.plot(impl_data['batch_size'], impl_data['time_ms'], 
                    marker='s', linewidth=2, markersize=8, label=impl)
        
        plt.title('批量查询性能对比', fontsize=16, fontweight='bold')
        plt.xlabel('查询数量', fontsize=12)
        plt.ylabel('耗时 (毫秒)', fontsize=12)
        plt.xscale('log')
        plt.yscale('log')
        plt.grid(True, which="both", ls="--", alpha=0.5)
        plt.legend(title='实现方式')
        plt.tight_layout()
        plt.savefig(output_dir / 'batch_query_performance.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # 4. 事务性能对比（折线图）
    batch_tx = df[df['operation'] == '事务批量插入']
    if not batch_tx.empty:
        plt.figure(figsize=(12, 8))
        
        for impl in batch_tx['implementation'].unique():
            impl_data = batch_tx[batch_tx['implementation'] == impl]
            impl_data = impl_data.sort_values('batch_size')
            plt.plot(impl_data['batch_size'], impl_data['time_ms'], 
                    marker='^', linewidth=2, markersize=8, label=impl)
        
        plt.title('事务批量插入性能对比', fontsize=16, fontweight='bold')
        plt.xlabel('批量大小', fontsize=12)
        plt.ylabel('耗时 (毫秒)', fontsize=12)
        plt.xscale('log')
        plt.yscale('log')
        plt.grid(True, which="both", ls="--", alpha=0.5)
        plt.legend(title='实现方式')
        plt.tight_layout()
        plt.savefig(output_dir / 'transaction_performance.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # 5. 性能对比雷达图
    if not single_ops.empty:
        # 准备雷达图数据
        operations = single_ops['operation'].unique()
        implementations = single_ops['implementation'].unique()
        
        # 计算每个实现的相对性能（ORM相对于SQLite3）
        radar_data = []
        for op in operations:
            sqlite_time = single_ops[(single_ops['operation'] == op) & 
                                    (single_ops['implementation'] == 'SQLite3')]['time_ms'].mean()
            orm_time = single_ops[(single_ops['operation'] == op) & 
                                 (single_ops['implementation'] == 'ORM')]['time_ms'].mean()
            
            if sqlite_time > 0:
                speedup = sqlite_time / orm_time if orm_time > 0 else 0
                radar_data.append({
                    'operation': op,
                    'speedup': speedup
                })
        
        if radar_data:
            radar_df = pd.DataFrame(radar_data)
            
            # 创建雷达图
            angles = np.linspace(0, 2 * np.pi, len(radar_df), endpoint=False).tolist()
            angles += angles[:1]  # 闭合图形
            
            fig, ax = plt.subplots(figsize=(10, 10), subplot_kw=dict(projection='polar'))
            
            values = radar_df['speedup'].tolist()
            values += values[:1]  # 闭合图形
            
            ax.plot(angles, values, 'o-', linewidth=2)
            ax.fill(angles, values, alpha=0.25)
            
            ax.set_xticks(angles[:-1])
            ax.set_xticklabels(radar_df['operation'].tolist(), fontsize=10)
            ax.set_ylim(0, max(values) * 1.2)
            ax.set_title('ORM相对于SQLite3的性能加速比\n(值>1表示ORM更快)', fontsize=14, fontweight='bold', pad=20)
            ax.grid(True)
            
            plt.tight_layout()
            plt.savefig(output_dir / 'performance_radar.png', dpi=300, bbox_inches='tight')
            plt.close()
    
    # 6. 生成汇总报告
    create_summary_report(df, output_dir)

def create_summary_report(df, output_dir):
    """生成汇总报告"""
    report_lines = []
    report_lines.append("=" * 80)
    report_lines.append("SQLite3 vs ORM 性能基准测试报告")
    report_lines.append("=" * 80)
    report_lines.append("")
    
    # 总体统计
    total_tests = len(df)
    sqlite_tests = len(df[df['implementation'] == 'SQLite3'])
    orm_tests = len(df[df['implementation'] == 'ORM'])
    
    report_lines.append(f"测试总数: {total_tests}")
    report_lines.append(f"SQLite3测试数: {sqlite_tests}")
    report_lines.append(f"ORM测试数: {orm_tests}")
    report_lines.append("")
    
    # 按操作类型统计
    report_lines.append("按操作类型统计:")
    report_lines.append("-" * 40)
    
    for op_type in df['operation'].unique():
        op_data = df[df['operation'] == op_type]
        if len(op_data) >= 2:  # 至少有两个实现可以比较
            sqlite_mean = op_data[op_data['implementation'] == 'SQLite3']['time_ms'].mean()
            orm_mean = op_data[op_data['implementation'] == 'ORM']['time_ms'].mean()
            
            if sqlite_mean > 0 and orm_mean > 0:
                speedup = sqlite_mean / orm_mean
                faster = "ORM更快" if speedup > 1 else "SQLite3更快"
                report_lines.append(f"{op_type}:")
                report_lines.append(f"  SQLite3平均耗时: {sqlite_mean:.3f} ms")
                report_lines.append(f"  ORM平均耗时: {orm_mean:.3f} ms")
                report_lines.append(f"  性能对比: {speedup:.2f}x ({faster})")
                report_lines.append("")
    
    # 批量操作分析
    report_lines.append("批量操作性能分析:")
    report_lines.append("-" * 40)
    
    batch_sizes = sorted(df['batch_size'].unique())
    for batch_size in batch_sizes:
        if batch_size > 1:
            batch_data = df[df['batch_size'] == batch_size]
            if len(batch_data) >= 2:
                sqlite_times = batch_data[batch_data['implementation'] == 'SQLite3']['time_ms']
                orm_times = batch_data[batch_data['implementation'] == 'ORM']['time_ms']
                
                if len(sqlite_times) > 0 and len(orm_times) > 0:
                    sqlite_mean = sqlite_times.mean()
                    orm_mean = orm_times.mean()
                    
                    if sqlite_mean > 0 and orm_mean > 0:
                        speedup = sqlite_mean / orm_mean
                        report_lines.append(f"批量大小 {batch_size}:")
                        report_lines.append(f"  SQLite3: {sqlite_mean:.3f} ms")
                        report_lines.append(f"  ORM: {orm_mean:.3f} ms")
                        report_lines.append(f"  加速比: {speedup:.2f}x")
                        report_lines.append("")
    
    # 写入报告文件
    report_file = output_dir / 'benchmark_report.txt'
    with open(report_file, 'w', encoding='utf-8') as f:
        f.write('\n'.join(report_lines))
    
    print(f"报告已保存到: {report_file}")

def main():
    parser = argparse.ArgumentParser(description='SQLite3 vs ORM 基准测试结果可视化')
    parser.add_argument('--input', '-i', default='results.json', 
                       help='基准测试结果JSON文件路径')
    parser.add_argument('--output', '-o', default='visualization',
                       help='输出目录路径')
    
    args = parser.parse_args()
    
    # 检查输入文件
    input_file = Path(args.input)
    if not input_file.exists():
        print(f"错误: 输入文件不存在: {input_file}")
        sys.exit(1)
    
    # 创建输出目录
    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)
    
    print(f"加载基准测试结果: {input_file}")
    df = load_benchmark_results(input_file)
    
    print(f"加载完成，共 {len(df)} 个测试结果")
    print(f"实现方式: {df['implementation'].unique().tolist()}")
    print(f"操作类型: {df['operation'].unique().tolist()}")
    
    # 创建图表
    print("生成可视化图表...")
    create_comparison_charts(df, output_dir)
    
    print(f"图表已保存到: {output_dir}")
    print("完成!")

if __name__ == '__main__':
    main()