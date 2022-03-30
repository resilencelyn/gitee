## 介绍

本中间件在 mycat 源码基础上进行了优化和改造，支持数据库列级别的透明加解密，兼容 mysql8。

支持 mysql 原生协议，支持在配置文件中以 jdbc 驱动的方式连接其他类型数据库，但并不推荐，mycat 是 mysql 最佳伴侣，其他数据库可能存在诸多不稳定因素。

本中间件的用法跟 mycat 基本一致，在原来的基础上添加了配置文件 encrypt.xml 用于加密的相关配置，server.xml 里增加了几个加密的属性。

这里着重介绍一下具体用法和加密部分的配置，其他部分的配置可参考 mycat，另外，分库分表不在万不得已不要轻易使用，mycat 的分库分表是一个伪分布式数据库，日后如果要扩充节点调整规则会十分麻烦，其实大部分公司也只用了它的读写分离功能。

关注公众号 **螺旋编程极客** ，可进群一起探讨，共同学习进步，同时可领取海量学习资源。
![螺旋编程极客](image/%E5%85%AC%E4%BC%97%E5%8F%B7.jpg)

## 安装

下载 release 压缩包，直接解压即可。

[v1.6.7.8-release下载](https://gitee.com/zrxjava/mycat-encrypt-server/releases/v1.6.7.8-release)

## 运行
### linux
   ./mycat start 启动

   ./mycat stop 停止

   ./mycat console 前台运行

   ./mycat restart 重启服务

   ./mycat pause 暂停

   ./mycat status 查看启动状态

### win
直接运行startup_nowrap.bat，如果出现闪退，在cmd 命令行运行，查看出错原因。 

## 内存配置
打开 conf/wrapper.conf 文件，可以调整 jvm 的相关参数。

## 连接测试
测试 mycat 与测试 mysql 完全一致，mysql 怎么连接，mycat 就怎么连接。

mycat 的默认服务端口为 8066: mysql -u 用户名 -P 8066 -p 密码，程序或 navicat 等工具连接使用此端口

mycat 的默认管理端口为 9066: mysql -u 用户名 -P 9066 -p 密码，配合 [mycat-encrypt-web](https://gitee.com/zrxjava/mycat-encrypt-web) 用于监控管理，常用指令如 show @@sql，show @@sql.slow，show @@sql.high 等

mycat 完全支持 mysql5.7，目前市面上的主流 mysql 版本仍然是 5.7，包括分布式数据库 TiDB 也是对 mysql5.7 进行适配。

如果要使用 mysql8.0，需要把用户的密码认证插件设置为 mysql_native_password，

对已经创建过的用户执行命令：

```mysql
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'password';
```

修改 my.cnf 配置文件，之后创建的用户将默认使用 mysql_native_password：

```mysql
[mysqld]
default_authentication_plugin=mysql_native_password
```

## 配置说明
--bin  启动目录

--conf 配置目录存放配置文件：

      --server.xml：是Mycat服务器参数调整和用户授权的配置文件。
    
      --schema.xml：是逻辑库定义和表以及分片定义的配置文件。
      
      --encrypt.xml 是加密指定库，表，字段的配置文件（新添加的模块）。
    
      --rule.xml：  是分片规则的配置文件，分片规则的具体一些参数信息单独存放为文件，也在这个目录下，配置文件修改需要重启MyCAT。
    
      --log4j.xml： 日志存放在logs/log中，每天一个文件，日志的配置是在conf/log4j.xml中，根据自己的需要可以调整输出级别为debug                           debug级别下，会输出更多的信息，方便排查问题。
    
      --autopartition-long.txt,partition-hash-int.txt,sequence_conf.properties， sequence_db_conf.properties 分片相关的id分片规则配置文件
    
      --lib	   MyCAT自身的jar包或依赖的jar包的存放目录。
    
      --logs   MyCAT日志的存放目录。日志存放在logs/log中，每天一个文件


## 逻辑库配置
### 编辑schema.xml

schema 是实际逻辑库的配置，可配置多个

dataNode 是逻辑库对应的分片

dataHost 是实际的物理库配置地址，可以配置多主主从等其他配置，多个 dataHost 代表分片对应的物理库地址，下面的 writeHost、readHost 代表该分片是否配置多写，主从，读写分离等高级特性。

 **balance 属性**

负载均衡类型，目前的取值有 3 种：

1. balance="0", 不开启读写分离机制，所有读操作都发送到当前可用的 writeHost 上。
2. balance="1"，全部的 readHost 与 stand by writeHost 参与 select 语句的负载均衡，简单的说，当双
   主双从模式(M1->S1，M2->S2，并且 M1 与 M2 互为主备)，正常情况下，M2,S1,S2 都参与 select 语句的负载
   均衡。
3. balance="2"，所有读操作都随机的在 writeHost、readhost 上分发。
4. balance="3"，所有读请求随机的分发到 wiriterHost 对应的 readhost 执行，writerHost 不负担读压力，

**writeType 属性**

负载均衡类型，目前的取值有 3 种：

1. writeType="0", 所有写操作发送到配置的第一个 writeHost，第一个挂了切到还生存的第二个 writeHost，
   重新启动后以切换后的为准，切换记录在配置文件中:dnindex.properties 。
2. writeType="1"，所有写操作都随机的发送到配置的 writeHost，1.5 以后废弃不推荐。

**switchType 属性**

-1 表示不自动切换

1 默认值，自动切换

2 基于 MySQL 主从同步的状态决定是否切换

心跳语句为 show slave status

3 基于 MySQL galary cluster 的切换机制（适合集群）

心跳语句为 show status like ‘wsrep%’

```xml
    <schema name="test" checkSQLschema="true" sqlMaxLimit="100" dataNode="test">
    </schema>

    <dataNode name="test" dataHost="test" database="test" />

    <dataHost name="test" maxCon="1000" minCon="10" balance="0"
              writeType="0" dbType="mysql" dbDriver="native" switchType="1"  slaveThreshold="100">
        <heartbeat>select user()</heartbeat>
        <!-- can have multi write hosts -->
        <writeHost host="hostM1" url="localhost:3307" user="root"
                   password="root">
        </writeHost>
    </dataHost>
```

### 配置server.xml

添加mycat逻辑库：test（多个用逗号隔开）

use 可配置多个，实现多租户。  

```xml
 <user name="root">
        <property name="password">123456</property>
        <property name="schemas">test</property>
 </user>
```

新增加密相关属性：

```xml
<!--是否启用加密-->
<property name="encrypt">1</property>
<!--是否对返回结果解密-->
<property name="decrypt">1</property>
<!--是否使用内置的加密算法 1-是 0-否 内置的加密算法支持加密后的 like 查询 -->
<!--如果要自定义加密算法，下载源码 mycat-encrypt-customize 自行实现 encode 和 decode 方法，打包替换 lib 下的			encrypt-core-2.0.jar 即可-->
<property name="useInternalEncryptAlgorithm">1</property>
<!--内置加密算法的密钥（自定义16位字符串） 如果自定义加密算法，则不需要配置-->
<property name="secretKey">12345678ABCDEFGH</property>
```

## 加密配置

### 编辑 encrypt.xml

```xml
	<!--数据库编码需要为utf-8或ut8mb64类型 以下所有属性皆为必填项目-->
    <!--加密的此处的 schema 对应 schema.xml 中的schema，schema 保持唯一，不能重复-->
    <server schema="test">
        <!--auto 设置为1会自动对配置的字段进行加密，dbType 目前只支持 mysql-->
        <!--datasource 的 name 属性跟 schema 的 database 属性保持一致-->
        <datasource name="test" ip="localhost" port="3307" username="root" password="root" auto="1" dbType="mysql">
            <!--需要加密的字段名称-->
            <column name="card_code">
                <!--需要加密的表名和主键 程序会根据主键更新加密字段的值-->
                <table name="real_people_copy1" pk="people_id"/>
            </column>
            <column name="name">
                <table name="real_people_copy1" pk="people_id"/>
            </column>
        </datasource>
    </server>
```

### 自定义加密算法

如果要自定义加密算法，下载源码包 [mycat-encrypt-customize](https://gitee.com/zrxjava/mycat-encrypt-customize) 自行实现 encode 和 decode 方法，打包替换 lib 下的 encrypt-core-2.0.jar 即可

### 手动解密

如果要恢复已经加密的数据，可以下载解密工具包，使用方法跟 mycat 一致，配置好 decrypt.xml 即可自动解密，解密完毕后程序会自动退出。

[下载解密工具](https://gitee.com/zrxjava/mycat-decrypt/releases/v1.6.7.6-release)


关注公众号 **螺旋编程极客** ，可进群一起探讨，共同学习进步，同时可领取海量学习资源。
![螺旋编程极客](image/%E5%85%AC%E4%BC%97%E5%8F%B7.jpg)
