#!/bin/zsh

# ===== 配置 =====
BUILD_DIR="build"
CPPINSIGHTS="/opt/homebrew/Cellar/cppinsights/20.1_1/bin/insights"
# ===== 解析参数 =====
case "$1" in
    # 模板调试模式
    debug|d)
        echo "🔍 模板调试模式"
        cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
              -DCMAKE_CXX_STANDARD=20 \
              -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
              -B ${BUILD_DIR}
        
        # 展开指定文件（默认 examples/schema.cpp）
        FILE="${2:-examples/schema.cpp}"
        ${CPPINSIGHTS} ${FILE} -p ${BUILD_DIR}/compile_commands.json > expanded.cpp
        echo "✅ 已生成 expanded.cpp"
        
        # 可选：用编辑器打开
        # code expanded.cpp
        exit 0
        ;;
    
    # 清理
    clean|c)
        rm -rf ${BUILD_DIR} expanded.cpp
        echo "✅ 已清理"
        exit 0
        ;;
    
    # 仅编译
    build)
        cmake --build ${BUILD_DIR} -j 4
        exit $?
        ;;

    # 运行特定示例
    example|e)
        EXAMPLE="${2:-schema}"
        if [ "$EXAMPLE" = "schema" ]; then
            ./${BUILD_DIR}/examples/schema_example
        elif [ "$EXAMPLE" = "orm" ]; then
            ./${BUILD_DIR}/examples/orm_example
        else
            echo "❌ 未知示例: $EXAMPLE"
            echo "可用示例: schema, orm"
            exit 1
        fi
        exit $?
        ;;

    # 运行基准测试
    benchmark|bench|b)
        BENCH_TYPE="${2:-all}"
        
        # 确保构建目录存在
        if [ ! -d "${BUILD_DIR}" ]; then
            echo "📦 构建目录不存在，正在构建..."
            cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
                  -DCMAKE_CXX_STANDARD=20 \
                  -DCMAKE_CXX_STANDARD_REQUIRED=ON \
                  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                  -B ${BUILD_DIR}
            cmake --build ${BUILD_DIR} -j 4
        fi
        
        case "$BENCH_TYPE" in
            all)
                echo "📊 1/2 全面基准测试 (batch 10/100/1000/10000)..."
                ./${BUILD_DIR}/bench/comprehensive_benchmark --benchmark_out=bench/data/comprehensive_results.json --benchmark_out_format=json
                echo ""
                echo "📊 2/2 文件数据库基准测试..."
                ./${BUILD_DIR}/bench/file_benchmark --benchmark_out=bench/data/file_benchmark.json --benchmark_out_format=json
                echo ""
                echo "✅ 基准测试完成"
                echo ""
                echo "📈 生成图表和数据..."
                python3 bench/scripts/chart_visualization.py
                python3 bench/scripts/export_paper_data.py
                ;;
            comprehensive|comp)
                echo "📊 运行全面基准测试 (batch 10/100/1000/10000)..."
                ./${BUILD_DIR}/bench/comprehensive_benchmark --benchmark_out=bench/data/comprehensive_results.json --benchmark_out_format=json
                python3 bench/scripts/chart_visualization.py
                python3 bench/scripts/export_paper_data.py
                echo ""
                echo "📈 数据已保存到 bench/data/ 和 bench/charts/"
                ;;
            file)
                echo "📊 运行文件数据库基准测试..."
                ./${BUILD_DIR}/bench/file_benchmark --benchmark_out=bench/data/file_benchmark.json --benchmark_out_format=json
                python3 bench/scripts/chart_visualization.py
                ;;
analyze|a)
                echo "📈 分析结果 + 生成图表 + 导出CSV..."
                source venv/bin/activate 2>/dev/null || true
                python3 bench/scripts/export_paper_data.py
                python3 bench/scripts/chart_visualization.py
                echo ""
                echo "✅ 完成！"
                echo "   图表: bench/charts/*.png"
                echo "   数据: bench/data/*.csv"
                ;;
            chart|c)
                echo "📊 生成图表..."
                source venv/bin/activate 2>/dev/null || true
                python3 bench/scripts/chart_visualization.py
                echo ""
                echo "📈 图表已保存到 bench/charts/"
                ;;
            export|export_data)
                echo "📊 导出CSV数据..."
                source venv/bin/activate 2>/dev/null || true
                python3 bench/scripts/export_paper_data.py
                echo ""
                echo "📈 数据已保存到 bench/data/*.csv"
                ;;
            *)
                echo "❌ 未知基准测试类型: $BENCH_TYPE"
                echo "可用类型:"
                echo "  all            运行全面+文件基准测试+图表+CSV (推荐)"
                echo "  comp|comprehensive  内存数据库全面基准测试"
                echo "  file           文件数据库基准测试"
                echo "  chart          仅生成图表"
                echo "  analyze        分析+图表+CSV"
                echo "  export         仅导出CSV"
                exit 1
                ;;
        esac
        exit $?
        ;;

    # 帮助
    help|h)
        echo "用法: $0 [命令] [参数]"
        echo "  (无参数)  完整构建并运行两个示例"
        echo "  debug|d   展开模板到 expanded.cpp"
        echo "  clean|c   清理构建目录"
        echo "  build     仅编译"
        echo "  example|e [schema|orm]  运行特定示例"
    echo "  benchmark|bench|b [类型]  运行基准测试"
    echo "    类型: all (推荐), comp, chart, analyze, export"
        echo "  help|h    显示帮助"
        exit 0
        ;;
esac
# ===== 默认：完整构建流程 =====
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
      -DCMAKE_CXX_STANDARD=20 \
      -DCMAKE_CXX_STANDARD_REQUIRED=ON \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -B ${BUILD_DIR}
cmake --build ${BUILD_DIR} -j 4
if [[ $? -ne 0 ]]; then
    echo "\n❌ 编译失败"
    echo "💡 提示: 运行 '$0 debug' 查看模板展开"
    exit 1
fi
# 运行两个示例
echo "🚀 运行 orm_example..."
./${BUILD_DIR}/examples/orm_example
# echo ""
# echo "🚀 运行 schema_example..."
# ./${BUILD_DIR}/examples/schema_example
