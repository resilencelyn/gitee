## 适配分布式Server SAN PureFlash的RocksDB 系统

这个版本的RocksDB提供了一个plugin以适配[PureFlash](https://gitee.com/cocalele/PureFlash) 
PureFlash是一个采用Blob over Block架构的高性能存储系统。通过PureFlash系统提供的AOF(Append only File)可以支持rocksdb。

这样的架构可以使rocksdb直接访问ServeSAN系统，无需经过本地文件系统等中间层，以达到更好的性能。

### 使用方法
以rocksdb自带的simple_example为例，
0. 请参照https://gitee.com/cocalele/PureFlash/blob/master/build_and_run.txt 编译PureFlash系统。
1. 编译rocksdb, 假定PureFlash被克隆到~/PureFlash目录，编译目录为~/PureFlash/build_deb
```
	# git clone https://gitee.com/cocalele/PureFlash.git
	# git clone https://gitee.com/cocalele/rocksdb.git
	# cd rocksdb
    # DEBUG_LEVEL=2 ROCKSDB_PLUGINS="pfaof" PF_INC=../PureFlash/common/include PF_LIB=../PureFlash/build_deb/bin make simple_example
```
2. 运行
   如果使测试，您可以以docker的方式快速运行PureFlash, 请参考https://gitee.com/cocalele/PureFlash/blob/master/run-from-docker.txt，
   或者[PureFlash上手指南](https://blog.csdn.net/winux/article/details/114436250)
   然后你可以将编译好的rocksdb simple_example可执行程序复制到PureFlash容器中进行运行测试
   
## 下面是原始rocksdb的README
[![CircleCI Status](https://circleci.com/gh/facebook/rocksdb.svg?style=svg)](https://circleci.com/gh/facebook/rocksdb)
[![TravisCI Status](https://api.travis-ci.com/facebook/rocksdb.svg?branch=main)](https://travis-ci.com/github/facebook/rocksdb)
[![Appveyor Build status](https://ci.appveyor.com/api/projects/status/fbgfu0so3afcno78/branch/main?svg=true)](https://ci.appveyor.com/project/Facebook/rocksdb/branch/main)
[![PPC64le Build Status](http://140-211-168-68-openstack.osuosl.org:8080/buildStatus/icon?job=rocksdb&style=plastic)](http://140-211-168-68-openstack.osuosl.org:8080/job/rocksdb)


RocksDB is developed and maintained by Facebook Database Engineering Team.
It is built on earlier work on [LevelDB](https://github.com/google/leveldb) by Sanjay Ghemawat (sanjay@google.com)
and Jeff Dean (jeff@google.com)

This code is a library that forms the core building block for a fast
key-value server, especially suited for storing data on flash drives.
It has a Log-Structured-Merge-Database (LSM) design with flexible tradeoffs
between Write-Amplification-Factor (WAF), Read-Amplification-Factor (RAF)
and Space-Amplification-Factor (SAF). It has multi-threaded compactions,
making it especially suitable for storing multiple terabytes of data in a
single database.

Start with example usage here: https://github.com/facebook/rocksdb/tree/main/examples

See the [github wiki](https://github.com/facebook/rocksdb/wiki) for more explanation.

The public interface is in `include/`.  Callers should not include or
rely on the details of any other header files in this package.  Those
internal APIs may be changed without warning.

Questions and discussions are welcome on the [RocksDB Developers Public](https://www.facebook.com/groups/rocksdb.dev/) Facebook group and [email list](https://groups.google.com/g/rocksdb) on Google Groups.

## License

RocksDB is dual-licensed under both the GPLv2 (found in the COPYING file in the root directory) and Apache 2.0 License (found in the LICENSE.Apache file in the root directory).  You may select, at your option, one of the above-listed licenses.
