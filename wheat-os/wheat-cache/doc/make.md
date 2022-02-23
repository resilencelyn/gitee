### 构建工具文档

#### dcgen
1. 根据结构体接口模板生成 proto 文件
2. 迁移 proto 到 pkg/proto 下
3. 更新结构体常量

>PS : 开发一个 storage 的新接口时一般有以下步骤
>1. 修改 storage 接口配置文件
>2. make dcgen
>3. 修改生成的 proto 文件
>4. make dcgen
>5. 添加 storage 的操作接口 


#### build-storage
编译并且生成 /bin/storage

#### build-gateway
编译并且生成 /bin/gateway

#### install
1. 安装项目，需要 sudo

#### storage
根据配置文件启动 storage

#### gateway
根据配置文件启动 gateway

#### init-conf
根据配置文件文档初始化配置文件到 /etc/wheat-cache/wheat-cache.yaml 
