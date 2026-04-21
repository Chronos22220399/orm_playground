1. 帮我检查一下 include/ess/orm/core 下的一些运行时功能是否存在问题，然后在docs目录下拟定一份测试
   计划，我查看后你再动手写
2. 我需要你为我在bench下写一些基准测试，测试我的库和sqlite3原生
   api的效率对比，使用的数据库就在bench/data下创建，主要测试在include
   /ess/orm/core下的一些运行时功能，比如带表和不带表的query，lambda风格
   的transaction及内部query的测试，基准测试使用google benchmark，通过参数将结果输出为json文件，然后
   用python进行可视化处理
