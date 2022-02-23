### WheatCache 设计方案



#### 1. 架构方案

![pro](https://gitee.com/timedb/picgo-imig/raw/master/images/pro.svg)



#### 2. 分布式设计模型方案

![distributed](https://gitee.com/timedb/picgo-imig/raw/master/images/distributed.svg)



#### 3. 必要实现任务

- LRU 储存机制
- 常用储存结构接口
- 日志储存, 日志恢复机制, 版本重做机制
- Raft 一致性日志
- Proxy或(Storage) 转发机制
- 中间件
- 客户端



#### 4. 可能使用的工具

**go部分**

- ProtocBuf  V3.17.3 (版本统一)
- GoRPC, jsonRPC
- Grpc
- logrus
- gob
- go mock