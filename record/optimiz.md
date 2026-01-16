## 原本

```cpp
static constexpr std::string make_create_table_ddl() {
  std::vector<std::string> column_defs{};
  (
      [&]() {
        std::vector<std::string> parts{};
        // 列名
        parts.push_back(std::string(std::string_view(Fields::column_name)));
        // 类型名
        using sql_type =
            meta::cpp_type_to_sql_semantic_t<typename Fields::member_type>;
        parts.push_back(std::string(std::string_view(
            meta::sql_semantic_to_type_str<sql_type>::type_str)));
        // 属性列表
        std::apply(
            [&](auto... attrs) {
              (parts.push_back(attribute::to_sql_fragment(attrs)), ...);
            },
            typename Fields::attributes{});

        // 过滤掉空字符串并拼接
        std::string def;
        for (const auto &p : parts) {
          if (!p.empty())
            def += " ";
          def += p;
        }
        column_defs.push_back(def);
      }(),
      ...);
  return fmt::format("CREATE TABLE {} ({});", std::string_view(table_name),
                     fmt::join(column_defs, ",\n"));
}
```

```cpp
-- The CXX compiler identification is GNU 15.2.1
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- === C++ Compiler Info ===
-- CXX Compiler Path: /usr/bin/c++
-- CXX Compiler ID: GNU
-- CXX Compiler Version: 15.2.1
-- Configuring done (0.2s)
-- Generating done (0.0s)
-- Build files have been written to: /home/Ess/Code/orm_playground/build
[ 33%] Building CXX object CMakeFiles/orm.dir/cmake_pch.hxx.gch

Time variable                                  wall           GGC
 phase setup                        :   0.00 (  0%)  1958k (  1%)
 phase parsing                      :   1.46 (100%)   148M ( 99%)
 |name lookup                       :   0.06 (  4%)  4674k (  3%)
 |overload resolution               :   0.04 (  2%)  5472k (  4%)
 PCH main state save                :   0.31 ( 21%)   257k (  0%)
 PCH preprocessor state save        :   0.04 (  3%)     0  (  0%)
 PCH pointer reallocation           :   0.23 ( 16%)     0  (  0%)
 PCH pointer sort                   :   0.33 ( 22%)     0  (  0%)
 preprocessing                      :   0.09 (  6%)    12M (  8%)
 parser (global)                    :   0.09 (  6%)    51M ( 34%)
 parser struct body                 :   0.10 (  7%)    23M ( 15%)
 parser function body               :   0.03 (  2%)  8904k (  6%)
 parser inl. func. body             :   0.05 (  3%)  6451k (  4%)
 parser inl. meth. body             :   0.10 (  7%)    25M ( 17%)
 template instantiation             :   0.08 (  5%)    20M ( 13%)
 constant expression evaluation     :   0.01 (  1%)   329k (  0%)
 TOTAL                              :   1.46          150M
[ 66%] Building CXX object CMakeFiles/orm.dir/src/schema.cpp.o

Time variable                                  wall           GGC
 phase setup                        :   0.00 (  0%)  1958k (  0%)
 phase parsing                      :   1.50 ( 12%)   300M ( 34%)
 phase lang. deferred               :   0.15 (  1%)    21M (  2%)
 phase opt and generate             :  11.19 ( 87%)   559M ( 63%)
 |name lookup                       :   0.04 (  0%)  7408k (  1%)
 |overload resolution               :   0.46 (  4%)    96M ( 11%)
 garbage collection                 :   0.28 (  2%)     0  (  0%)
 dump files                         :   0.01 (  0%)     0  (  0%)
 callgraph construction             :   1.35 ( 11%)   119M ( 14%)
 callgraph optimization             :   0.03 (  0%)    40k (  0%)
 callgraph functions expansion      :   9.02 ( 70%)   374M ( 42%)
 callgraph ipa passes               :   0.62 (  5%)    49M (  6%)
 ipa function summary               :   0.01 (  0%)   623k (  0%)
 ipa cp                             :   0.02 (  0%)  1346k (  0%)
 ipa inlining heuristics            :   0.03 (  0%)  3580k (  0%)
 ipa function splitting             :   0.01 (  0%)   119k (  0%)
 ipa pure const                     :   0.01 (  0%)  2688  (  0%)
 ipa icf                            :   0.01 (  0%)  7616  (  0%)
 ipa SRA                            :   0.01 (  0%)   175k (  0%)
 ipa modref                         :   0.01 (  0%)    66k (  0%)
 cfg construction                   :   0.01 (  0%)   716k (  0%)
 cfg cleanup                        :   0.24 (  2%)  5329k (  1%)
 trivially dead code                :   0.07 (  1%)     0  (  0%)
 df scan insns                      :   0.09 (  1%)    13k (  0%)
 df reaching defs                   :   0.21 (  2%)     0  (  0%)
 df live regs                       :   0.53 (  4%)     0  (  0%)
 df live&initialized regs           :   0.21 (  2%)     0  (  0%)
 df must-initialized regs           :   0.01 (  0%)     0  (  0%)
 df use-def / def-use chains        :   0.09 (  1%)     0  (  0%)
 df reg dead/unused notes           :   0.20 (  2%)  6566k (  1%)
 register information               :   0.05 (  0%)     0  (  0%)
 alias analysis                     :   0.12 (  1%)    22M (  3%)
 alias stmt walking                 :   0.05 (  0%)   621k (  0%)
 register scan                      :   0.02 (  0%)    34k (  0%)
 rebuild jump labels                :   0.04 (  0%)     0  (  0%)
 parser (global)                    :   0.03 (  0%)   513k (  0%)
 template instantiation             :   1.19 (  9%)   285M ( 32%)
 constant expression evaluation     :   0.28 (  2%)    34M (  4%)
 early inlining heuristics          :   0.01 (  0%)  2941k (  0%)
 inline parameters                  :   0.02 (  0%)  2715k (  0%)
 integration                        :   0.17 (  1%)    44M (  5%)
 tree gimplify                      :   0.03 (  0%)  8442k (  1%)
 tree eh                            :   0.02 (  0%)  4815k (  1%)
 tree CFG construction              :   0.01 (  0%)  4518k (  0%)
 tree CFG cleanup                   :   0.16 (  1%)    41k (  0%)
 tree tail merge                    :   0.01 (  0%)   890k (  0%)
 tree VRP                           :   0.11 (  1%)  8257k (  1%)
 tree Early VRP                     :   0.03 (  0%)  2012k (  0%)
 tree copy propagation              :   0.03 (  0%)  1152k (  0%)
 tree PTA                           :   0.28 (  2%)  3971k (  0%)
 tree SSA rewrite                   :   0.01 (  0%)  3087k (  0%)
 tree SSA incremental               :   0.08 (  1%)  4656k (  1%)
 tree operand scan                  :   0.04 (  0%)    12M (  1%)
 dominator optimization             :   0.17 (  1%)    12M (  1%)
 backwards jump threading           :   0.12 (  1%)    11M (  1%)
 tree SRA                           :   0.01 (  0%)   423k (  0%)
 tree CCP                           :   0.08 (  1%)   498k (  0%)
 tree reassociation                 :   0.01 (  0%)  7032  (  0%)
 tree PRE                           :   0.09 (  1%)  4789k (  1%)
 tree FRE                           :   0.11 (  1%)  2904k (  0%)
 tree code sinking                  :   0.01 (  0%)  1386k (  0%)
 tree linearize phis                :   0.01 (  0%)   302k (  0%)
 tree forward propagate             :   0.04 (  0%)   384k (  0%)
 tree conservative DCE              :   0.03 (  0%)    59k (  0%)
 tree aggressive DCE                :   0.02 (  0%)  2289k (  0%)
 tree DSE                           :   0.02 (  0%)    22k (  0%)
 tree loop invariant motion         :   0.02 (  0%)  1760  (  0%)
 tree canonical iv                  :   0.01 (  0%)   377k (  0%)
 complete unrolling                 :   0.02 (  0%)  1055k (  0%)
 tree vectorization                 :   0.01 (  0%)   180k (  0%)
 tree slp vectorization             :   0.03 (  0%)  5971k (  1%)
 tree iv optimization               :   0.05 (  0%)  4946k (  1%)
 tree copy headers                  :   0.02 (  0%)   859k (  0%)
 tree strlen optimization           :   0.01 (  0%)   316k (  0%)
 tree modref                        :   0.02 (  0%)   327k (  0%)
 dominance frontiers                :   0.01 (  0%)     0  (  0%)
 dominance computation              :   0.15 (  1%)     0  (  0%)
 out of ssa                         :   0.02 (  0%)    15k (  0%)
 expand vars                        :   0.09 (  1%)    15M (  2%)
 expand                             :   0.16 (  1%)    44M (  5%)
 post expand cleanups               :   0.02 (  0%)  3546k (  0%)
 forward prop                       :   0.18 (  1%)   902k (  0%)
 CSE                                :   0.63 (  5%)  3432k (  0%)
 dead code elimination              :   0.03 (  0%)     0  (  0%)
 dead store elim1                   :   0.10 (  1%)  5510k (  1%)
 dead store elim2                   :   0.08 (  1%)  6825k (  1%)
 loop init                          :   0.07 (  1%)  8935k (  1%)
 loop invariant motion              :   0.01 (  0%)   122k (  0%)
 loop unrolling                     :   0.01 (  0%)   257k (  0%)
 CPROP                              :   0.20 (  2%)  7350k (  1%)
 PRE                                :   0.11 (  1%)  2797k (  0%)
 CSE 2                              :   0.38 (  3%)  1935k (  0%)
 branch prediction                  :   0.01 (  0%)   605k (  0%)
 combiner                           :   0.32 (  2%)  8374k (  1%)
 late combiner                      :   0.16 (  1%)   740k (  0%)
 if-conversion                      :   0.02 (  0%)   175k (  0%)
 integrated RA                      :   0.55 (  4%)    55M (  6%)
 LRA non-specific                   :   0.25 (  2%)  8592k (  1%)
 LRA virtuals elimination           :   0.03 (  0%)  1265k (  0%)
 LRA reload inheritance             :   0.05 (  0%)   327k (  0%)
 LRA create live ranges             :   0.20 (  2%)   693k (  0%)
 LRA hard reg assignment            :   0.04 (  0%)     0  (  0%)
 LRA rematerialization              :   0.04 (  0%)     0  (  0%)
 reload CSE regs                    :   0.28 (  2%)  7209k (  1%)
 ree                                :   0.02 (  0%)  3864  (  0%)
 thread pro- & epilogue             :   0.06 (  0%)  1035k (  0%)
 if-conversion 2                    :   0.01 (  0%)  2392  (  0%)
 combine stack adjustments          :   0.01 (  0%)     0  (  0%)
 peephole 2                         :   0.04 (  0%)   321k (  0%)
 hard reg cprop                     :   0.05 (  0%)  1344  (  0%)
 scheduling 2                       :   0.55 (  4%)  2477k (  0%)
 machine dep reorg                  :   0.04 (  0%)   224  (  0%)
 reorder blocks                     :   0.03 (  0%)  2682k (  0%)
 shorten branches                   :   0.04 (  0%)     0  (  0%)
 final                              :   0.16 (  1%)    17M (  2%)
 variable output                    :   0.01 (  0%)   138k (  0%)
 access analysis                    :   0.02 (  0%)  1880  (  0%)
 ext dce                            :   0.04 (  0%)     0  (  0%)
 fold mem offsets                   :   0.03 (  0%)   699k (  0%)
 rest of compilation                :   0.22 (  2%)    36M (  4%)
 remove unused locals               :   0.05 (  0%)   136  (  0%)
 address taken                      :   0.03 (  0%)    16k (  0%)
 TOTAL                              :  12.83          883M
[100%] Linking CXX executable orm
[100%] Built target orm
```

## 优化1 -- 减少函数体体积

```cpp
static std::string make_create_table_ddl() {
 auto process_field = []<typename F>() {
   // 直接返回该列的片段，减少函数体大小
   using sql_type =
       meta::cpp_type_to_sql_semantic_t<typename F::member_type>;
   std::string col = fmt::format(
       "{} {}", std::string_view(F::column_name),
       std::string_view(meta::sql_semantic_to_type_str<sql_type>::type_str));

   // 处理属性
   std::string attrs_str;
   std::apply(
       [&](auto... attrs) {
         ((attrs_str += " ", attrs_str += attribute::to_sql_fragment(attrs)),
          ...);
       },
       typename F::attributes{});

   return col + attrs_str;
 };

 // 关键优化点：直接用折叠表达式收集结果
 std::vector<std::string> column_defs;
 column_defs.reserve(sizeof...(Fields)); // 提前扩容
 (column_defs.push_back(process_field.template operator()<Fields>()), ...);

 return fmt::format("CREATE TABLE {} ({});", std::string_view(table_name),
                    fmt::join(column_defs, ",\n"));
}
```

```cpp
-- The CXX compiler identification is GNU 15.2.1
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- === C++ Compiler Info ===
-- CXX Compiler Path: /usr/bin/c++
-- CXX Compiler ID: GNU
-- CXX Compiler Version: 15.2.1
-- Configuring done (0.1s)
-- Generating done (0.0s)
-- Build files have been written to: /home/Ess/Code/orm_playground/build
[ 33%] Building CXX object CMakeFiles/orm.dir/cmake_pch.hxx.gch

Time variable                                  wall           GGC
 phase setup                        :   0.00 (  0%)  1958k (  1%)
 phase parsing                      :   1.36 (100%)   148M ( 99%)
 |name lookup                       :   0.05 (  4%)  4676k (  3%)
 |overload resolution               :   0.03 (  3%)  5535k (  4%)
 PCH main state save                :   0.29 ( 21%)   257k (  0%)
 PCH preprocessor state save        :   0.04 (  3%)     0  (  0%)
 PCH pointer reallocation           :   0.22 ( 16%)     0  (  0%)
 PCH pointer sort                   :   0.30 ( 22%)     0  (  0%)
 preprocessing                      :   0.09 (  6%)    12M (  8%)
 parser (global)                    :   0.08 (  6%)    51M ( 34%)
 parser struct body                 :   0.09 (  7%)    23M ( 15%)
 parser function body               :   0.03 (  2%)  8904k (  6%)
 parser inl. func. body             :   0.05 (  3%)  6451k (  4%)
 parser inl. meth. body             :   0.09 (  7%)    25M ( 17%)
 template instantiation             :   0.07 (  5%)    20M ( 13%)
 constant expression evaluation     :   0.01 (  1%)   329k (  0%)
 TOTAL                              :   1.37          150M
[ 66%] Building CXX object CMakeFiles/orm.dir/src/schema.cpp.o

Time variable                                  wall           GGC
 phase setup                        :   0.00 (  0%)  1958k (  0%)
 phase parsing                      :   1.15 ( 11%)   243M ( 31%)
 phase lang. deferred               :   0.39 (  4%)    68M (  9%)
 phase opt and generate             :   9.39 ( 86%)   460M ( 59%)
 |name lookup                       :   0.04 (  0%)  7173k (  1%)
 |overload resolution               :   0.55 (  5%)   117M ( 15%)
 garbage collection                 :   0.26 (  2%)     0  (  0%)
 dump files                         :   0.01 (  0%)     0  (  0%)
 callgraph construction             :   2.61 ( 24%)   215M ( 28%)
 callgraph optimization             :   0.03 (  0%)    26k (  0%)
 callgraph functions expansion      :   6.21 ( 57%)   201M ( 26%)
 callgraph ipa passes               :   0.40 (  4%)    30M (  4%)
 ipa function summary               :   0.01 (  0%)   510k (  0%)
 ipa dead code removal              :   0.02 (  0%)     0  (  0%)
 ipa cp                             :   0.02 (  0%)   756k (  0%)
 ipa inlining heuristics            :   0.02 (  0%)  1732k (  0%)
 ipa function splitting             :   0.01 (  0%)   119k (  0%)
 ipa pure const                     :   0.01 (  0%)  2688  (  0%)
 ipa icf                            :   0.01 (  0%)     0  (  0%)
 cfg cleanup                        :   0.14 (  1%)  3290k (  0%)
 trivially dead code                :   0.04 (  0%)     0  (  0%)
 df scan insns                      :   0.06 (  1%)  8880  (  0%)
 df reaching defs                   :   0.15 (  1%)     0  (  0%)
 df live regs                       :   0.39 (  4%)     0  (  0%)
 df live&initialized regs           :   0.15 (  1%)     0  (  0%)
 df must-initialized regs           :   0.01 (  0%)     0  (  0%)
 df use-def / def-use chains        :   0.06 (  1%)     0  (  0%)
 df reg dead/unused notes           :   0.15 (  1%)  4523k (  1%)
 register information               :   0.04 (  0%)     0  (  0%)
 alias analysis                     :   0.08 (  1%)    14M (  2%)
 alias stmt walking                 :   0.03 (  0%)   186k (  0%)
 register scan                      :   0.01 (  0%)    17k (  0%)
 rebuild jump labels                :   0.02 (  0%)     0  (  0%)
 parser (global)                    :   0.03 (  0%)   513k (  0%)
 template instantiation             :   1.10 ( 10%)   273M ( 35%)
 constant expression evaluation     :   0.28 (  3%)    36M (  5%)
 early inlining heuristics          :   0.01 (  0%)  1298k (  0%)
 inline parameters                  :   0.02 (  0%)  2598k (  0%)
 integration                        :   0.11 (  1%)    20M (  3%)
 tree gimplify                      :   0.02 (  0%)  6429k (  1%)
 tree eh                            :   0.01 (  0%)  3354k (  0%)
 tree CFG construction              :   0.00 (  0%)  3562k (  0%)
 tree CFG cleanup                   :   0.06 (  1%)    41k (  0%)
 tree tail merge                    :   0.01 (  0%)    82k (  0%)
 tree VRP                           :   0.05 (  0%)   929k (  0%)
 tree Early VRP                     :   0.02 (  0%)  2015k (  0%)
 tree copy propagation              :   0.01 (  0%)   198k (  0%)
 tree PTA                           :   0.21 (  2%)  2785k (  0%)
 tree SSA rewrite                   :   0.01 (  0%)  2122k (  0%)
 tree SSA incremental               :   0.03 (  0%)  3599k (  0%)
 tree operand scan                  :   0.02 (  0%)    10M (  1%)
 dominator optimization             :   0.07 (  1%)  2987k (  0%)
 backwards jump threading           :   0.06 (  1%)   555k (  0%)
 tree SRA                           :   0.01 (  0%)   107k (  0%)
 tree CCP                           :   0.04 (  0%)   234k (  0%)
 tree PRE                           :   0.04 (  0%)  2317k (  0%)
 tree FRE                           :   0.06 (  1%)  2601k (  0%)
 tree code sinking                  :   0.01 (  0%)   439k (  0%)
 tree forward propagate             :   0.02 (  0%)   174k (  0%)
 tree conservative DCE              :   0.01 (  0%)  5360  (  0%)
 tree aggressive DCE                :   0.01 (  0%)  2189k (  0%)
 tree DSE                           :   0.02 (  0%)    23k (  0%)
 tree slp vectorization             :   0.03 (  0%)  4898k (  1%)
 tree strlen optimization           :   0.01 (  0%)   222k (  0%)
 tree modref                        :   0.01 (  0%)   322k (  0%)
 dominance computation              :   0.08 (  1%)     0  (  0%)
 control dependences                :   0.01 (  0%)     0  (  0%)
 out of ssa                         :   0.01 (  0%)    24k (  0%)
 expand vars                        :   0.07 (  1%)  9502k (  1%)
 expand                             :   0.11 (  1%)    31M (  4%)
 post expand cleanups               :   0.01 (  0%)  2047k (  0%)
 lower subreg                       :   0.01 (  0%)    77k (  0%)
 forward prop                       :   0.12 (  1%)   738k (  0%)
 CSE                                :   0.56 (  5%)  2685k (  0%)
 dead code elimination              :   0.02 (  0%)     0  (  0%)
 dead store elim1                   :   0.06 (  1%)  3953k (  0%)
 dead store elim2                   :   0.05 (  0%)  4316k (  1%)
 loop init                          :   0.03 (  0%)  2777k (  0%)
 CPROP                              :   0.13 (  1%)  5143k (  1%)
 PRE                                :   0.12 (  1%)  1859k (  0%)
 CSE 2                              :   0.37 (  3%)  1787k (  0%)
 branch prediction                  :   0.01 (  0%)   275k (  0%)
 combiner                           :   0.22 (  2%)  6242k (  1%)
 late combiner                      :   0.11 (  1%)   789k (  0%)
 if-conversion                      :   0.01 (  0%)   230k (  0%)
 integrated RA                      :   0.42 (  4%)    20M (  3%)
 LRA non-specific                   :   0.15 (  1%)  3111k (  0%)
 LRA virtuals elimination           :   0.02 (  0%)   647k (  0%)
 LRA reload inheritance             :   0.04 (  0%)   947k (  0%)
 LRA create live ranges             :   0.18 (  2%)   319k (  0%)
 LRA hard reg assignment            :   0.03 (  0%)     0  (  0%)
 LRA rematerialization              :   0.03 (  0%)     0  (  0%)
 reload CSE regs                    :   0.22 (  2%)  4817k (  1%)
 ree                                :   0.01 (  0%)  3864  (  0%)
 thread pro- & epilogue             :   0.04 (  0%)   682k (  0%)
 if-conversion 2                    :   0.01 (  0%)  7456  (  0%)
 combine stack adjustments          :   0.01 (  0%)     0  (  0%)
 peephole 2                         :   0.03 (  0%)   562k (  0%)
 hard reg cprop                     :   0.04 (  0%)  1680  (  0%)
 scheduling 2                       :   0.39 (  4%)  1641k (  0%)
 machine dep reorg                  :   0.02 (  0%)   224  (  0%)
 reorder blocks                     :   0.02 (  0%)  1668k (  0%)
 shorten branches                   :   0.03 (  0%)     0  (  0%)
 final                              :   0.09 (  1%)  8572k (  1%)
 variable output                    :   0.01 (  0%)   134k (  0%)
 access analysis                    :   0.01 (  0%)  1904  (  0%)
 ext dce                            :   0.04 (  0%)     0  (  0%)
 fold mem offsets                   :   0.02 (  0%)   503k (  0%)
 rest of compilation                :   0.14 (  1%)    24M (  3%)
 remove unused locals               :   0.03 (  0%)   280  (  0%)
 address taken                      :   0.02 (  0%)    10k (  0%)
 TOTAL                              :  10.94          774M
[100%] Linking CXX executable orm
```

## 优化2 -- 减少内联

```cpp
template <typename Field>
[[gnu::noinline]] static std::string make_column_def() {
 using member_type = typename Field::member_type;
 using sql_type = meta::cpp_type_to_sql_semantic_t<member_type>;

 std::string def = std::string(std::string_view(Field::column_name));
 def += " ";
 def += std::string(
     std::string_view(meta::sql_semantic_to_type_str<sql_type>::type_str));

 // 处理属性
 std::apply(
     [&](auto... attrs) {
       ((def += " ", def += attribute::to_sql_fragment(attrs)), ...);
     },
     typename Field::attributes{});

 return def;
}

static std::string make_create_table_ddl() {
 std::vector<std::string> column_defs;
 column_defs.reserve(sizeof...(Fields));

 // 使用简单的折叠表达式调用独立函数
 (column_defs.push_back(make_column_def<Fields>()), ...);

 return fmt::format("CREATE TABLE {} ({});", std::string_view(table_name),
                    fmt::join(column_defs, ",\n"));
}
```

```cpp
-- The CXX compiler identification is GNU 15.2.1
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- === C++ Compiler Info ===
-- CXX Compiler Path: /usr/bin/c++
-- CXX Compiler ID: GNU
-- CXX Compiler Version: 15.2.1
-- Configuring done (0.1s)
-- Generating done (0.0s)
-- Build files have been written to: /home/Ess/Code/orm_playground/build
[ 33%] Building CXX object CMakeFiles/orm.dir/cmake_pch.hxx.gch

Time variable                                  wall           GGC
 phase setup                        :   0.00 (  0%)  1958k (  1%)
 phase parsing                      :   1.36 (100%)   148M ( 99%)
 |name lookup                       :   0.05 (  4%)  4674k (  3%)
 |overload resolution               :   0.03 (  2%)  5486k (  4%)
 PCH main state save                :   0.29 ( 21%)   257k (  0%)
 PCH preprocessor state save        :   0.04 (  3%)     0  (  0%)
 PCH pointer reallocation           :   0.22 ( 16%)     0  (  0%)
 PCH pointer sort                   :   0.29 ( 21%)     0  (  0%)
 preprocessing                      :   0.09 (  7%)    12M (  8%)
 parser (global)                    :   0.08 (  6%)    51M ( 34%)
 parser struct body                 :   0.09 (  7%)    23M ( 15%)
 parser function body               :   0.03 (  2%)  8904k (  6%)
 parser inl. func. body             :   0.05 (  3%)  6451k (  4%)
 parser inl. meth. body             :   0.09 (  7%)    25M ( 17%)
 template instantiation             :   0.07 (  5%)    20M ( 13%)
 constant expression evaluation     :   0.01 (  1%)   329k (  0%)
 TOTAL                              :   1.37          150M
[ 66%] Building CXX object CMakeFiles/orm.dir/src/schema.cpp.o

Time variable                                  wall           GGC
 phase setup                        :   0.00 (  0%)  1958k (  0%)
 phase parsing                      :   1.17 ( 14%)   243M ( 38%)
 phase lang. deferred               :   0.39 (  5%)    67M ( 11%)
 phase opt and generate             :   6.81 ( 81%)   326M ( 51%)
 |name lookup                       :   0.04 (  0%)  7149k (  1%)
 |overload resolution               :   0.43 (  5%)    94M ( 15%)
 garbage collection                 :   0.25 (  3%)     0  (  0%)
 dump files                         :   0.01 (  0%)     0  (  0%)
 callgraph construction             :   1.35 ( 16%)   118M ( 19%)
 callgraph optimization             :   0.02 (  0%)    34k (  0%)
 callgraph functions expansion      :   5.03 ( 60%)   162M ( 25%)
 callgraph ipa passes               :   0.39 (  5%)    32M (  5%)
 ipa function summary               :   0.01 (  0%)   503k (  0%)
 ipa cp                             :   0.02 (  0%)  1029k (  0%)
 ipa inlining heuristics            :   0.02 (  0%)  2169k (  0%)
 ipa pure const                     :   0.01 (  0%)  2688  (  0%)
 ipa icf                            :   0.01 (  0%)  7616  (  0%)
 cfg cleanup                        :   0.11 (  1%)  2498k (  0%)
 trivially dead code                :   0.04 (  0%)     0  (  0%)
 df scan insns                      :   0.05 (  1%)    13k (  0%)
 df reaching defs                   :   0.10 (  1%)     0  (  0%)
 df live regs                       :   0.26 (  3%)     0  (  0%)
 df live&initialized regs           :   0.10 (  1%)     0  (  0%)
 df must-initialized regs           :   0.01 (  0%)     0  (  0%)
 df use-def / def-use chains        :   0.05 (  1%)     0  (  0%)
 df reg dead/unused notes           :   0.10 (  1%)  3504k (  1%)
 register information               :   0.03 (  0%)     0  (  0%)
 alias analysis                     :   0.07 (  1%)    11M (  2%)
 alias stmt walking                 :   0.02 (  0%)    81k (  0%)
 register scan                      :   0.01 (  0%)  8768  (  0%)
 rebuild jump labels                :   0.02 (  0%)     0  (  0%)
 parser (global)                    :   0.03 (  0%)   513k (  0%)
 template instantiation             :   1.12 ( 13%)   274M ( 43%)
 constant expression evaluation     :   0.27 (  3%)    33M (  5%)
 early inlining heuristics          :   0.01 (  0%)  1766k (  0%)
 inline parameters                  :   0.02 (  0%)  2567k (  0%)
 integration                        :   0.10 (  1%)    17M (  3%)
 tree gimplify                      :   0.02 (  0%)  6616k (  1%)
 tree eh                            :   0.01 (  0%)  3579k (  1%)
 tree CFG construction              :   0.01 (  0%)  3744k (  1%)
 tree CFG cleanup                   :   0.08 (  1%)   114k (  0%)
 tree tail merge                    :   0.01 (  0%)   233k (  0%)
 tree VRP                           :   0.08 (  1%)   959k (  0%)
 tree Early VRP                     :   0.02 (  0%)  2000k (  0%)
 tree copy propagation              :   0.01 (  0%)    53k (  0%)
 tree PTA                           :   0.17 (  2%)  2360k (  0%)
 tree SSA rewrite                   :   0.01 (  0%)  2257k (  0%)
 tree SSA incremental               :   0.03 (  0%)  2540k (  0%)
 tree operand scan                  :   0.01 (  0%)  8908k (  1%)
 dominator optimization             :   0.11 (  1%)  2074k (  0%)
 backwards jump threading           :   0.11 (  1%)   319k (  0%)
 tree CCP                           :   0.05 (  1%)   169k (  0%)
 tree reassociation                 :   0.01 (  0%)  6936  (  0%)
 tree PRE                           :   0.05 (  1%)  2754k (  0%)
 tree FRE                           :   0.06 (  1%)  2004k (  0%)
 tree code sinking                  :   0.01 (  0%)   658k (  0%)
 tree forward propagate             :   0.03 (  0%)   182k (  0%)
 tree conservative DCE              :   0.02 (  0%)  1784  (  0%)
 tree aggressive DCE                :   0.01 (  0%)  2313k (  0%)
 tree DSE                           :   0.01 (  0%)    40k (  0%)
 tree slp vectorization             :   0.04 (  0%)  4863k (  1%)
 tree strlen optimization           :   0.01 (  0%)   365k (  0%)
 tree modref                        :   0.01 (  0%)   324k (  0%)
 dominance computation              :   0.09 (  1%)     0  (  0%)
 control dependences                :   0.01 (  0%)     0  (  0%)
 out of ssa                         :   0.01 (  0%)    24k (  0%)
 expand vars                        :   0.04 (  1%)  7018k (  1%)
 expand                             :   0.10 (  1%)    23M (  4%)
 post expand cleanups               :   0.01 (  0%)  3087k (  0%)
 forward prop                       :   0.10 (  1%)   526k (  0%)
 CSE                                :   0.31 (  4%)  1388k (  0%)
 dead code elimination              :   0.02 (  0%)     0  (  0%)
 dead store elim1                   :   0.05 (  1%)  2631k (  0%)
 dead store elim2                   :   0.05 (  1%)  3111k (  0%)
 loop init                          :   0.03 (  0%)  3077k (  0%)
 CPROP                              :   0.11 (  1%)  5016k (  1%)
 PRE                                :   0.05 (  1%)  1144k (  0%)
 CSE 2                              :   0.18 (  2%)   777k (  0%)
 branch prediction                  :   0.01 (  0%)   316k (  0%)
 combiner                           :   0.19 (  2%)  5780k (  1%)
 late combiner                      :   0.10 (  1%)   512k (  0%)
 if-conversion                      :   0.01 (  0%)   176k (  0%)
 integrated RA                      :   0.31 (  4%)    15M (  2%)
 LRA non-specific                   :   0.15 (  2%)  3855k (  1%)
 LRA virtuals elimination           :   0.02 (  0%)   639k (  0%)
 LRA reload inheritance             :   0.04 (  0%)   311k (  0%)
 LRA create live ranges             :   0.12 (  1%)   395k (  0%)
 LRA hard reg assignment            :   0.02 (  0%)     0  (  0%)
 LRA rematerialization              :   0.02 (  0%)     0  (  0%)
 reload CSE regs                    :   0.14 (  2%)  3384k (  1%)
 ree                                :   0.01 (  0%)  3312  (  0%)
 thread pro- & epilogue             :   0.03 (  0%)  1036k (  0%)
 if-conversion 2                    :   0.01 (  0%)  3288  (  0%)
 combine stack adjustments          :   0.01 (  0%)     0  (  0%)
 peephole 2                         :   0.03 (  0%)   336k (  0%)
 hard reg cprop                     :   0.03 (  0%)  1104  (  0%)
 scheduling 2                       :   0.28 (  3%)  1405k (  0%)
 machine dep reorg                  :   0.02 (  0%)   224  (  0%)
 reorder blocks                     :   0.02 (  0%)  1343k (  0%)
 shorten branches                   :   0.02 (  0%)     0  (  0%)
 final                              :   0.16 (  2%)    13M (  2%)
 variable output                    :   0.01 (  0%)   138k (  0%)
 access analysis                    :   0.02 (  0%)  2112  (  0%)
 ext dce                            :   0.02 (  0%)     0  (  0%)
 fold mem offsets                   :   0.02 (  0%)   342k (  0%)
 rest of compilation                :   0.12 (  1%)    15M (  2%)
 remove unused locals               :   0.03 (  0%)   136  (  0%)
 address taken                      :   0.01 (  0%)  8936  (  0%)
 TOTAL                              :   8.38          638M
[100%] Linking CXX executable orm
[100%] Built target orm
```
