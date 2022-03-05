### Cache 分布式方案-Getway



![getway方案](https://gitee.com/timedb/img/raw/master/images/getway方案.svg)



1. single 集群分布式方案中，使用 getway 方向代理客户端的 grpc 请求， 通过 hash 环实现 分布式。
2. 集群模式中， 通过主从来 实现 cache 的备份问题，提高容灾性。

