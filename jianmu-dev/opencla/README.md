# OpenCLA

### 介绍

> OpenCLA为开源项目提供贡献者协议签署服务

[jianmu.opencla.org](https://jianmu.opencla.org)

### 预览

![预览](preview/cla.png)

### TL;DR

自定义[docker-compose.yml](deploy/docker-compose.yml)中标注为xxxx的重要配置

```shell
wget https://gitee.com/jianmu-dev/opencla/raw/master/deploy/docker-compose.yml

docker-compose.yml up -d
```

### 前端

[README.md](ui/README.md)

### 后端

#### 运行环境

* JDK 11 及以上
* Docker 18.09.7及以上
* Mysql 8.0及以上

#### 如何编译

`mvn package`

#### 如何运行

参考 [application-xxx.yml.sample](web/src/main/resources/application-xxx.yml.sample)中的配置创建你自己的 `application-dev.yml` 配置文件来覆盖需要配置的值，如datasource.url（当前必须使用名为dev的profile）

下面截取application-xxx.yml.sample的几段配置做一个说明

> 注意：配置gitee第三方应用时，除了指定下面的Client ID和Client Secret外，还需指定`应用回调地址`，格式为`http(s)://host:port/agreement`，例如：`https://jianmu.opencla.org/agreement`

```yml
opencla:
  # 协议配置
  agreement:
    # 项目名称
    project-name: 建木
    # 社区名称
    community-name: 建木
    # 社区归属，公司/组织/团体等名称
    community-ownership: xxxx
    # 代码托管平台，如，Gitee/Github等
    code-hosting-platform: Gitee
    # 代码托管平台URL
    code-hosting-platform-url: https://gitee.com/jianmu-dev
  # gitee配置
  gitee:
    # 第三方应用，https://gitee.com/oauth/applications
    # 第三方应用Client ID
    client-id: xxxx
    # 第三方应用Client Secret
    client-secret: xxxx
  # 邮箱配置
  email:
    # 发送者邮箱
    sender: cla@jianmu.dev
    # 邮箱服务器的smtp主机
    host: smtp.ym.163.com
    # 邮箱服务器的smtp端口，默认为465
    port: 994
    # 是否使用ssl与smtp服务器进行通信，默认为true
    use-ssl: true
    # 邮箱服务器的授权码
    authorization-code: xxxx
    # 邮箱签名
    signature: 建木CLA
    # 邮箱主题
    subject: 签署建木CLA
    # 邮件中的团队名称
    team: 建木团队
    # 提供支持的邮箱
    support-email: support@jianmu.dev
```

#### 构建docker镜像

构建docker镜像使用jib，需要在[pom.xml](https://gitee.com/jianmu-dev/opencla/blob/master/web/pom.xml)中对jib配置进行自定义

主要集中在以下几个配置：
- docker镜像名 
- docker镜像tag
- docker hub用户名
- docker hub密码或密钥

```xml
<to>
  <image>${env.JIANMU_IMAGE_NAME}:${env.JIANMU_IMAGE_TAG}</image>
  <auth>
    <username>${env.JIANMU_DOCKER_USERNAME}</username>
    <password>${env.JIANMU_DOCKER_PASSWORD}</password>
  </auth>
</to>
```

执行`mvn install`时，触发bulid和push镜像
