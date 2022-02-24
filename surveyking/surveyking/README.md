# 卷王

简体中文 | [English](./README.en-us.md)

<img src="./docs/preview.jpg" width = "100%" alt="" align=center />

<details>
  <summary>预览</summary>

  ![preview-surveyking](./docs/preview.gif)

</details>

## 背景

基于阿里大佬 [Janry](https://github.com/janryWang) 开源的 [formily](https://github.com/alibaba/formily) 表单解决方案自研问卷编辑器，使用最新的前后端技术（React+SpringBoot+AntDesignUI），构建出一套完整的调查问卷系统。

卷王是已知开源调查问卷系统中**功能最强大**、**安装最简单**、**综合体验最好**的一个，一些功能特性及体验可以对标甚至超过问卷网、问卷星、腾讯问卷这些国内主流的在线表单系统。

**稳定**、**易用**、**专业**，是卷王的持续开发理念。大中小企业甚至个人都可以使用卷王快速搭建一个属于自己的在线问卷系统。

## 🚀 1 分钟快速体验调查问卷系统(无需安装数据库)

1. 安装 [Java 运行环境](https://www.java.com/zh-CN/download/manual.jsp)（如果本机已有 Java 环境可以忽略）
2. 下载 [卷王安装包](https://gitee.com/surveyking/surveyking/attach_files/864427/download/surveyking-h2-v0.2.0.jar)
3. 双击运行
4. 打开浏览器访问 [http://localhost:1991](http://localhost:1991)，输入账号密码： *admin*/*surveyking*

## 特性

- 🥇 支持多种题型，如填空、选择、下拉、级联、矩阵、分页、签名、题组等
- 🎉 可计算，分为**逻辑计算**、**值计算**、**文本替换计算**、**必填计算**，[点击体验](https://surveyking.cn/s/logic8)
- 🦋 问题、选项支持快速富文本编辑
- 🥊 响应式布局，所有页面在 PC 和手机端都有良好的操作体验
- 🥂 多种问卷设置，如支持**问卷默认值**、**暂存**、**问卷修改**、**设置密码**、背景图、页眉图、**提交后图文展示**等
- 🎇 数据，支持问卷数据新增、编辑、标记、导出、预览和打包下载附件
- 🎨 报表，支持对问题实时统计分析并以图形（条形图、柱形图、扇形图）、表格的形式展示输出
- 🚀 安装部署简单（**最快 1 分钟部署**），支持 nginx 部署和一键启动的方式
- 👬 支持多人协作管理问卷
- 🎁 后端支持多种数据库，可支持所有带有 jdbc 驱动的关系型数据库，未来将支持 mongodb 这种文档数据库
- 🐯 安全、可靠、稳定、高性能的后端 API 服务
- 🙆 支持完善的 RBAC 权限控制
- 😊 支持**工作流**，工作流代码已全部开源
- 🌈 支持国际化（正在完善）
- ...

## 安装

### 通过发行版安装

目前已适配 mysql 数据库

<!-- 点击[下载 surveyking-h2 版本](https://gitee.com/surveyking/surveyking/attach_files/948414/download/surveyking-h2-v0.2.1.jar)到本地 -->

点击[下载 surveyking-mysql 版本](https://gitee.com/surveyking/surveyking/attach_files/976520/download/surveyking-v0.3.0-beta.4.jar)到本地

### 使用源码编译安装

默认构建的是 h2 版本的安装包。

```bash
# 下载源码
git clone https://gitee.com/surveyking/surveyking.git

# 开始构建
gradle clean :api:build -P pro -x test

# 生成的 jar 包位于 ./api/build/libs/surveyking-v0.x.x.jar
```

### 使用 docker 快速启动

启动 SurveyKing 镜像时，你可以指定 SurveyKing 挂载参数，将日志文件和内置数据库保存到你本地。(docker 版本目前还有点问题，待解决）

```bash
docker run -p 1991:1991 surveyking/surveyking
# 挂载数据文件
docker run -p 1991:1991 -v /surveyking:/surveyking surveyking/surveyking
```

## 使用

- **预安装 JRE 环境**，由于本系统是 Java 构建的，需要依赖 Java 运行环境，可以通过 [适用于所有操作系统的 Java 下载
  ](https://www.java.com/zh-CN/download/manual.jsp) 来预装 java 环境。
- **配置数据库**，按照下面的说明来配置不同的数据库，如果前端需要使用 nginx 部署，参考使用 nginx 部署前端。
- **运行**，支持所有平台部署，windows 和 mac 支持双击运行，或者打开命令行窗口执行如下命令

```bash
java -jar surveyking-v0.x.x.jar
```

打开浏览器，访问 [http://localhost:1991](http://localhost:1991) 即可，系统首次启动之后会自动创建 admin 用户，账号/密码（*admin/123456*），登录系统之后可以通过用户管理界面来修改密码。

<!-- ### h2 启动方式

无需任何配置，会自动创建数据库启动脚本，如需改变端口号，参考 mysql 启动方式的定义端口。 -->

### mysql 启动方式

使用参数启动

1. 首先创建 mysql 数据库，然后执行初始化脚本，[下载脚本](https://gitee.com/surveyking/surveyking/raw/master/rdbms/src/main/resources/scripts/init-mysql.sql)。
2. 执行 `java -jar surveyking-v0.x.x.jar --server.port=1991 --spring.datasource.url=jdbc:mysql://localhost:3306/surveyking --spring.datasource.username=root --spring.datasource.password=123456`（只有首次启动系统需要添加后面的参数）

参数说明(按照实际需要自行修改)：

- `--server.port=1991` 系统端口
- `--spring.datasource.url=jdbc:mysql://localhost:3306/surveyking` 数据库连接的 url
- `--spring.datasource.username=root` 数据库账号
- `--spring.datasource.password=123456` 数据库密码

也可以尝试使用命令行的方式初始化数据库（会自动执行数据库初始脚本）

```bash
# 按照提示初始化数据库
java -jar surveyking-v0.x.x.jar i
# 初始化完成之后运行即可
java -jar surveyking-v0.x.x.jar 
```

### 使用 nginx 部署前端

下载 [该目录下面的静态资源文件](https://gitee.com/surveyking/surveyking/tree/master/api/src/main/resources/static)，直接部署到 nginx 即可。

然后配置 proxy 代理到后端 api 服务。

## 在线体验

演示地址： [https://surveyking.cn](https://surveyking.cn)

点击 *试一试*，无需注册登录

## 联系作者

本项目后端代码完全开源，前端代码待稳定之后也会有开源计划。如果觉得对您有帮助，可以点击右上角的 star。

如果遇到任何问题或者建议，欢迎加群讨论。

<div>
 QQ群：338461197
</div>

## 开源协议

SurveyKing is open source software licensed as
[MIT.](https://github.com/javahuang/SurveyKing/blob/master/LICENSE)
