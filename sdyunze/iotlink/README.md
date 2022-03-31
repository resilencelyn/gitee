
<h1 align="center" style="margin: 30px 0 30px; font-weight: bold;">IoTLink v0.7.0</h1>
<h4 align="center">基于SpringBoot开发的轻量级的物联网综合业务支撑平台</h4>


<h1 align="center" style="margin: 30px 0 30px; font-weight: bold;">IoTLink</h1>

> 提供开箱即用的物联卡云端SaaS部署和本地私有部署解决方案，为您的物联卡提供数据采集的服务器端PaaS平台支撑。

 使用 IoTLink 您可以：

- 业务告警、生命周期分布、业务使用量情况 等一览平台数据信息。
- 管理物联卡 用量记录、套餐封装、在线信息获取、智能诊断、分组备注、企业代理管理、预存、充值、返利 等。
- 微信端 B端企业管理 toB 信息看板数据信息全局掌控 预存、返利、提现、诊断、充值、订单等便携数据处理。
- 微信端 C端用户自查询 卡基本信息、用量、续费、预存、支付密码、常见问题、智能诊断、实名绑定 等 用户自处理操作。


特色功能

1. 业务分离，独立操作 系统功能与业务执行分离 优化用户体验加强业务独立。
2. 上游通道灵活配置，一次对接终生实用，支持二次开发、拓展、拒绝对接费用。 
3. 通道轮询进度查看 可查看通道下 用量、生命周期、激活时间 等各个类别轮询进度一手掌握。
4. 关键数据私钥加密保障，全程加密传输 防止爬虫获取数据。
5. 首页 数据一览一手掌控 业务告警、生命周期分布、业务量使用情况、平台数据信息 等。
6. 商品、仓储、客户、合同、入款、发货 ERP企业常用功能拓展即将上线，敬请期待。



# 一、 关于 IoTLink

## 1.1 项目介绍

IoTLink基于 SpringBoot、Vue、Mybatis、RabbitMq、Mysql、Redis 等开发,支持物联网卡、物联网模组、卡+模组融合管理。提供状态、资费、客户、进销存、合同、订单、续费、充值、诊断、账单等功能。平台可同时接入中国移动、中国电信、中国联通、第三方物联网卡进行统一管理。逐步完善平台，助您快速接入物联网，让万物互联更简单。

通信管理:主要用于所有物联卡的综合信息查询，同时提供不涉及资费产品订购的终端和物联卡的功能配置管理。

资费管理:资费决定了用户使用通信服务功能，如数据、短信和语言服务的权限和计费方式。

生命周期:重新定义了物联卡在使用过程中各个特征时期，目前分为七大生命周期，包括：可测试、库存、待激活、已激活、已停机、预销户、已销户(参考移动EC)。

其他资料正在补充中...

特别鸣谢：[RuoYi-Vue](https://gitee.com/y_project/RuoYi-Vue)，[element](https://github.com/ElemeFE/element)，[vue-element-admin](https://github.com/PanJiaChen/vue-element-admin)，[eladmin-web](https://github.com/elunez/eladmin-web)

## 1.2 [关于云则](https://www.5iot.com)

一家专注于物联网行业通信模组、传感器、设备研究、研发、生产及销售于一体的新兴高新技术企业。公司聚集了一批朝气蓬勃、有梦想，有追求的物联网人，他们服务热忱，经验丰富，熟悉行业，用专业的技术和热情的态度做好产品技术支持。

公司以创新为动力，以应用促研发，为政府部门、企事业单位和行业客户提供专业的物联网解决方案。与通信运营商强强联合，赋予更多行业、更便捷、高质量、低成本的联网能力。并与多家知名硬件生产、方案商精诚合作。携手政府、金融、农业、环保、工业、能源、安防等产业伙伴们一起打造更高效率、更安全、更节能环保、更可持续发展的智慧产业，公司致力于成为您身边的物联网专家，助您快速接入物联网，让万物互联更简单。

公司通过“专业、及时、真诚”的服务，获得了广大用户及业界的认可，依靠多年的行业经验，与用户共同打造可持续发展企业，实现为客户持续创造价值的目标。


## 1.3 能力
系统运行框架图：
![输入图片说明](https://www.5iot.com/doc/introduceImgs/systemStructure1.png)


系统共分为七大模块：

![输入图片说明](https://www.5iot.com/doc/introduceImgs/FunctionList.jpg)


## 1.4 愿景

愿所有的物联网公司、物联网从业者、物联网开发者享受开源的魅力。让物联网在不久的将来更具工具化，为各行各业赋能创造。


# 二、关于项目

## 2.1 系统演示

演示地址：[http://demo.5iot.com/](http://demo.5iot.com/)

演示账号/密码：5iot/123456

## 2.2 技术栈

本项目基于 [RuoYi-Vue](https://gitee.com/y_project/RuoYi-Vue?_from=gitee_search) 后台开发框架，感谢 [RuoYi-Vue](https://gitee.com/y_project/RuoYi-Vue?_from=gitee_search) 的开源。


| 技术栈 | 介绍 | 地址 |
|:-:|:-:|:-:|
| Vue | 渐进式 JavaScript 框架  |  https://cn.vuejs.org/  |
|  Vuex	|专为 Vue.js 应用程序开发的状态管理模式	|https://vuex.vuejs.org/zh/  |
|Vue Router|	Vue.js 官方的路由管理器|	https://router.vuejs.org/zh/
|Vue CLI	|基于 Vue.js 进行快速开发的完整系统	|https://cli.vuejs.org/zh/guide/
|Vant|	轻量、可靠的移动端 Vue 组件库	|https://vant-contrib.gitee.io/vant/#/zh-CN/
|Element-UI	|基于 Vue 2.0 的桌面端组件库|	https://element.eleme.io/#/zh-CN
|ES6|	JavaScript 语言的下一代标准|	https://es6.ruanyifeng.com/


 **后端技术栈** 
| 技术栈 | 介绍 | 地址 |
|:-:|:-:|:-:|
|Spring Boot	|快捷创建基于 Spring 的生产级应用程序|	https://spring.io/projects/spring-boot
|MyBatis-Plus|	MyBatis 增强工具	|https://mp.baomidou.com/
|MyBatis|	MyBatis 持久层框架	|https://mybatis.org/mybatis-3/zh/index.html
|JWT|	轻量级身份认证规范|	https://jwt.io/introduction
|RabbitMq|	基于AMQP协议的消息中间件 |	https://www.rabbitmq.com/
|Spring Security	|基于 Spring 的强大且高度可定制的身份验证和访问控制框架	|https://spring.io/projects/spring-security/


## 2.3 项目结构

**后端结构** 

```
com.yunze     
├── common            // 工具类
│       └── annotation                    // 自定义注解
│       └── config                        // 全局配置
│       └── constant                      // 通用常量
│       └── core                          // 核心控制
│       └── enums                         // 通用枚举
│       └── exception                     // 通用异常
│       └── filter                        // 过滤器处理
│       └── mapper                        // 数据持久化
│       └── utils                         // 通用类处理
├── framework         // 框架核心
│       └── aspectj                       // 注解实现
│       └── config                        // 系统配置
│       └── datasource                    // 数据权限
│       └── interceptor                   // 拦截器
│       └── manager                       // 异步处理
│       └── security                      // 权限控制
│       └── web                           // 前端控制
├── yunze-consumption-admin               // 平台业务分离执行监听
│       └── system                       // 监听yunze-admin业务执行
├── yunze-consumption-car-activatedate   // 轮询 激活时间 执行同步
├── yunze-consumption-car-disconnected   // 未订购停机 消费者
├── yunze-consumption-car-flow           // 轮询 用量 执行同步
├── yunze-consumption-car-status         // 轮询 生命周期 执行同步
├── yunze-consumption-car-stop           // 达量停机 消费者
├── yunze-consumption-order              // 订单充值 消费者
├── yunze-consumption-update             // yz_card_info 表修改 消费者
├── yunze-generator   // 代码生成
├── yunze-quartz      // 定时任务
├── yunze-system      // 系统代码
├── yunze-admin       // 后台服务
├── yunze-ui          // 页面前端代码

```

**前端结构** 

```
├── build                      // 构建相关  
├── bin                        // 执行脚本
├── public                     // 公共文件
│   ├── favicon.ico            // favicon图标
│   └── index.html             // html模板
├── src                        // 源代码
│   ├── api                    // 所有请求
│   ├── assets                 // 主题 字体等静态资源
│   ├── components             // 全局公用组件
│   ├── directive              // 全局指令
│   ├── layout                 // 布局
│   ├── router                 // 路由
│   ├── store                  // 全局 store管理
│   ├── utils                  // 全局公用方法
│   ├── views                  // view
│   ├── App.vue                // 入口页面
│   ├── main.js                // 入口 加载组件 初始化等
│   ├── permission.js          // 权限管理
│   └── settings.js            // 系统配置
├── .editorconfig              // 编码格式
├── .env.development           // 开发环境配置
├── .env.production            // 生产环境配置
├── .env.staging               // 测试环境配置
├── .eslintignore              // 忽略语法检查
├── .eslintrc.js               // eslint 配置项
├── .gitignore                 // git 忽略项
├── babel.config.js            // babel.config.js
├── package.json               // package.json
└── vue.config.js              // vue.config.js
```


## 2.4 项目部署

点击下方链接进入官方语雀帮助手册查看项目部署方式：

[如何快速部署 IoTLink](https://www.5iot.com/doc/?target=/md/deploy)

建议服务器最低配置：

| 类型 | 配置 |
|:-:|:-:|
|操作系统|CentOS Stream  8 64位|
|CPU|4核|
|内存|8G|
|带宽|5M|
|硬盘|100G|

# 三、常见问题


**常见问题列表** 

- [如何快速部署 IoTLink](https://www.5iot.com/doc/?target=/md/deploy)
- [目前支持哪些上游接口](https://www.5iot.com/doc/?target=/md/upstreamApi)
- [为什么项目启动时报错](https://www.5iot.com/doc/?target=/md/startError)
- [数据库使用 Group By 查询报错](https://www.5iot.com/doc/?target=/md/sqlError)
- [项目每个分支的作用是什么](https://www.5iot.com/doc/?target=/md/branchDescription)

# 四、最近规划

补充中


# 五、联系我们
    
如果你有任何 IoTLink 产品上的想法、意见或建议，或商务上的合作需求，请扫码添加 IoTLink 项目团队进一步沟通：


![IoTLinkQunLiao20220119](https://www.5iot.com/doc/introduceImgs/IoTLinkQunLiao20220119.png)


# 六、项目截图


![首页](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow1-1.png)

![物联网卡管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow2-1.png)

![卡详情](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow2.1.png)

![公司所属查询](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow2.2.png)

![更新基础信息](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow2.3.png)

![物联网卡设置](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow3.png)

![平台资费](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow4.png)

![资费订购](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow5.png)

![订购历史](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow6.png)

![用量详情](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow7.png)

![上游通道](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow8.png)

![上游通道详情](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow8.1.png)

![通道进度](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow9.png)

![执行任务管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow10.png)

![执行任务下载](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow10.1.png)

![内部管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow11.png)

![企业管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow12.png)

![角色管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow13.png)

![全部订单](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow14.png)

![定时任务](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow15.png)

![菜单管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow16.png)

![岗位管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow17.png)

![字典管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow18.png)

![参数设置](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow19.png)

![日志执行](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow20.png)

![规则管理](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow21.png)

![推送配置](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow22.png)

![推送记录](https://www.5iot.com/doc/introduceImgs/pageShow/pageShow23.png)

![绑定卡号](https://www.5iot.com/doc/introduceImgs/pageShow/wxpageShow1.png)

![主页](https://www.5iot.com/doc/introduceImgs/pageShow/wxpageShow2.png)

![资费订购](https://www.5iot.com/doc/introduceImgs/pageShow/wxpageShow3.png)

![流量查询](https://www.5iot.com/doc/introduceImgs/pageShow/wxpageShow4.png)

![充值记录](https://www.5iot.com/doc/introduceImgs/pageShow/wxpageShow5.png)

![购物下单](https://www.5iot.com/doc/introduceImgs/pageShow/wxpageShow6.png)





---

 **<p align="center">如果您觉得我们的开源项目很有帮助，请点击 :star: Star 支持 IoTLink 开源团队  :heart: </p>** 

---