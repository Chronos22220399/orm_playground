#!/bin/bash
# SQLite3 vs ORM 基准测试运行脚本

set -e

echo "=== SQLite3 vs ORM 性能基准测试 ==="
echo ""

# 检查是否在build目录中运行
if [ ! -f "CMakeCache.txt" ]; then
    echo "错误: 请在build目录中运行此脚本"
    echo "用法: cd build && ../bench/run_benchmark.sh"
    exit 1
fi

# 构建基准测试
echo "1. 构建基准测试程序..."
cmake --build . --target sqlite_vs_orm_benchmark

# 运行基准测试
echo ""
echo "2. 运行基准测试..."
echo "   这将测试:"
echo "   - 插入单个/10个/100个/1000个记录"
echo "   - 查询单个/10个/100个/1000个记录"
echo "   - 更新和删除操作"
echo "   - 事务性能"
echo ""

./bench/sqlite_vs_orm_benchmark

# 检查结果文件
RESULTS_FILE="bench/results.json"
if [ -f "$RESULTS_FILE" ]; then
    echo ""
    echo "3. 基准测试完成!"
    echo "   结果已保存到: $RESULTS_FILE"
    
    # 检查Python环境
    echo ""
    echo "4. 检查Python可视化环境..."
    if command -v python3 &> /dev/null; then
        echo "   发现Python3，检查依赖..."
        
        # 检查主要依赖
        for pkg in matplotlib seaborn pandas numpy; do
            if python3 -c "import $pkg" 2>/dev/null; then
                echo "   ✓ $pkg 已安装"
            else
                echo "   ✗ $pkg 未安装"
                echo "   请运行: pip install -r ../bench/requirements.txt"
                exit 1
            fi
        done
        
        echo ""
        echo "5. 生成可视化图表..."
        cd bench
        python3 visualize_results.py
        
        echo ""
        echo "=== 完成 ==="
        echo "图表已保存到: bench/visualization/"
        echo "报告已保存到: bench/visualization/benchmark_report.txt"
    else
        echo "   未找到Python3，跳过可视化"
        echo ""
        echo "要安装Python依赖并生成图表，请运行:"
        echo "  pip install -r ../bench/requirements.txt"
        echo "  cd bench && python3 visualize_results.py"
    fi
else
    echo "错误: 结果文件未生成: $RESULTS_FILE"
    exit 1
fi