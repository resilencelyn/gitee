[![JDK 17](https://img.shields.io/badge/JDK-17-green.svg "JDK 17")]()

**过去、现在、将来都不会有商业版本，所有功能全部开源！**

**只做真的完全式开源，拒绝虚假开源，售卖商业版，不搞短暂维护！**

**承诺项目的维护周期是十年起步， 2022-03-01起，至少十年维护期！**

**提供高质量的使用文档！**

.

开源地址: [基于 sofa-bolt 的网络游戏框架-源码地址](https://gitee.com/iohao/iogame)    

在线文档: [基于 sofa-bolt 的网络游戏框架-在线文档](https://www.yuque.com/iohao/game/wwvg7z)

> 推荐大家看在线文档，排版好一些，README 上看有点乱！



## 愿景

**让网络游戏服务器的编程变得轻松简单！**



ioGame 是一个由 **java** 语言编写的**网络游戏服务器框架**。适用于回合制游戏、策略游戏、即时战斗游戏，等游戏服务器的开发。具有高性能、稳定、易用易扩展、超好编程体验等特点。可做为 H5（HTML5）、手游、端游的游戏服务器。

在这个网络游戏框架中能让你遗忘Netty，你几乎没有机会能直接的接触到 Netty 的**复杂**，但却能享受 Netty 带来的**高性能**。对开发者要求低，**为开发者节约开发时间**，为企业减少人工成本。

ioGame 可以很方便的与 spring 集成。支持多服多进程的方式部署，也支持多服单进程的方式部署。



> ioGame 是国内首个基于蚂蚁金服 [sofa-bolt](https://www.sofastack.tech/projects/sofa-bolt/overview/) 的网络游戏框架。
> 
> 通过 ioGame，可以快速的搭建一个稳定的、高性能的、**分步式**的网络游戏服务器，与游戏前端进行通信！
> 
> 源码内置了一个坦克射击游戏的示例，可直接运行。
> 
> 坦克射击游戏是基于FXGL引擎（纯java的游戏引擎）开发的。
> 
> 通过示例，可以快速的掌握网络游戏编程！



[游戏示例在线文档](https://www.yuque.com/iohao/game/gqossg)



**ioGame 的关注点：**

- 线程模型，基于[disruptor](https://www.yuque.com/iohao/game/gmfy1k)环形数组来消费业务
- [注重开发体验](https://www.yuque.com/iohao/game/wotnhl)
- [插件机制，扩展性高](https://www.yuque.com/iohao/game/gmxz33)
- [异常机制](https://www.yuque.com/iohao/game/avlo99)
- [游戏文档生成](https://www.yuque.com/iohao/game/irth38)
- [支持 JSR303+](https://www.yuque.com/iohao/game/ghng6g)
- [jprotobuf通信协议的友好支持](https://www.yuque.com/iohao/game/mbr9in)
- 业务代码定位--神级特性 （可以让你知道哪些业务方法被掉用了，并能快速的跳转到对应的业务代码中）



**内置多种可选模块，可按需选择，以方便应用开发：**

- [领域事件](https://www.yuque.com/iohao/game/gmfy1k) （[disruptor](https://www.yuque.com/iohao/game/gmfy1k) 实现类似Spring事件驱动模型 ApplicationEvent）
- [任务延时器](https://www.yuque.com/iohao/game/niflk0) （将来某个时间可对任务进行执行、暂停、取消等操作，并不是类似 Quartz 的任务调度）
- [多环境切换](https://www.yuque.com/iohao/game/ekx6ve) （不同运行环境下的配置支持）
- [light-jprotobuf ](https://www.yuque.com/iohao/game/vpe2t6) （补足 jprotobuf 不能让多个对象在单个 .proto 源文件中生成的需求，并简化jprotobuf对源文件的注释）



**置的其他功能：**

- [心跳相关](https://www.yuque.com/iohao/game/uueq3i)
- [用户上线、离线相关的钩子方法](https://www.yuque.com/iohao/game/hv5qqh)
- [UserSessions](https://www.yuque.com/iohao/game/wg6lk7) （对所有用户UserSession的管理，统计在线用户等）
- [UserSession](https://www.yuque.com/iohao/game/wg6lk7) (与 channel 是 1:1 的关系，可取到对应的 userId、channel 等信息。)
- [登录相关](https://www.yuque.com/iohao/game/tywkqv)



**游戏服务器通信相关：**

- [多个逻辑服之间可以相互掉用](https://www.yuque.com/iohao/game/anguu6)
- 广播消息给全服用户
- 广播消息给多个指定用户
- 单播消息给指定用户



**服务器相关：**

- 动态添加与扩展游戏逻辑服
- 网关服 - 负载均衡逻辑服
- 对外服 - 保持用户长连接



**集成相关：**

- [spring 集成 ](https://www.yuque.com/iohao/game/evkgnz)（业务框架可以方便的与 spring 进行集成，5 行代码）



**已完成的示例：**

示例代码在源码 **example/** 目录下

- [多服单进程示例](https://www.yuque.com/iohao/game/zm6qg2) - DemoApplication.java
- [spring集成示例](https://www.yuque.com/iohao/game/evkgnz) - DemoSpringBootApplication
- [多个逻辑服之间相互掉用示例](https://www.yuque.com/iohao/game/anguu6) - DemoInteractionApplication



**后续计划：**

- 抽象通用的游戏逻辑 （进一步减少开发实践过程中的工作量）
- 步骤表
- 帧同步
- 状态同步





## java 网络游戏服务器简介

游戏框架：ioGame

ioGame 是国内首个基于蚂蚁金服 [sofa-bolt](https://www.sofastack.tech/projects/sofa-bolt/overview/) 的网络游戏框架，游戏框架由 [**网络通信框架**] 和 [**业务框架**] 组成。

- **网络通信框架**：负责服务器之间的网络通信

- **业务框架**：负责业务逻辑的处理方式和编写方式

这是一个**注重开发体验、稳定、高性能**的服务器、高可扩展、简单上手的游戏框架。 游戏框架**借助于蚂蚁金服** sofa-bolt 通信框架来**提供稳定、高性能**。 即使之前没有游戏编程的经验，也能参与到游戏编程中。 游戏框架致力于**屏蔽通信细节**，为了让 Java 程序员能将更多的精力放在基于游戏框架的**业务逻辑实现上**！ 业务框架注重开发体验，提供异常机制、游戏业务文档生成、支持 JSR303+、业务线程编排、屏蔽 NIO 等细节。 让游戏开发者做尽可能少的事。如果你之前具备一些游戏开发或者 web MVC 的知识会更容易的上手游戏服务器的开发。



## 网络通信框架 - SOFABolt

[SOFABolt](https://www.sofastack.tech/projects/sofa-bolt/overview/) 是蚂蚁金融服务集团开发的一套基于 Netty 实现的网络通信框架。

- 为了让 Java 程序员能将更多的精力放在基于网络通信的业务逻辑实现上，而不是过多的纠结于网络底层 NIO 的实现以及处理难以调试的网络问题，Netty 应运而生。
- 为了让中间件开发者能将更多的精力放在产品功能特性实现上，而不是重复地一遍遍制造通信框架的轮子，SOFABolt 应运而生。

Bolt 名字取自迪士尼动画-闪电狗，是一个**基于 Netty 最佳实践**的轻量、易用、高性能、易扩展的通信框架。



## 业务框架

如果说SOFABolt为了让 Java 程序员能将更多的精力放在基于网络通信的业务逻辑实现上。**而业务框架正是解决业务逻辑如何方便的实现这一问题上**。业务框架是游戏框架的一部份，职责是简化程序员的业务逻辑实现。业务框架使程序员能够快速的开始编写游戏业务。在开发阶段可快速定位业务代码，请求响应监控，方便日志定位（插件实现）。

### 特点：

- 跨NIO框架（屏蔽 NIO）, 可以做到业务代码不做任何的改动，就能轻松实现NIO框架的切换.(netty mina 这些都属于NIO框架)

- 线程模型，基于[disruptor](https://www.yuque.com/iohao/game/gmfy1k)环形数组来消费业务

- [注重开发体验](https://www.yuque.com/iohao/game/wotnhl) 

- [插件机制，扩展性高](https://www.yuque.com/iohao/game/gmxz33)

- [异常机制](https://www.yuque.com/iohao/game/avlo99)

- [游戏文档生成](https://www.yuque.com/iohao/game/irth38)

- [支持 JSR303+](https://www.yuque.com/iohao/game/ghng6g)

- [jprotobuf通信协议的友好支持](https://www.yuque.com/iohao/game/mbr9in)



## 快速入门

### 业务交互

![img](https://gitee.com/iohao/xmindpic/raw/master/game/interaction.jpeg)

抽象的说，游戏前端与游戏服务器的的交互由上图组成。游戏前端与游戏服务器可以自由的双向交互，交互的业务数据由 .proto 作为载体。



### 协议文件

协议文件是对业务数据的描述载体，用于游戏前端与游戏服务的数据交互。Protocol Buffers (ProtocolBuffer/ protobuf )是Google公司开发的一种数据描述语言。也简称 [PB](https://www.oschina.net/p/protocol+buffers)。当然协议文件描述还可以是 json、xml或者任意自定义的，因为最后传输时会转换为二进制。但游戏开发中 PB 是目前的最佳。



### 游戏前端

游戏前端可以是 [Unity](https://unity.cn/)、 [UE(虚幻)](https://www.unrealengine.com/zh-CN/)、 [Cocos](https://www.cocos.com/)或者其他的游戏引擎。游戏前端与用户的接触最为直接。



### 游戏服务器

游戏服务器处理实际的用户等其他的业务数据。



### 快速入门代码示例

这里主要介绍游戏服务器，毕竟是游戏服务器的框架。下面这个示例介绍了服务器编程可以变得如此简单。



#### Proto 协议文件定义

首先我们自定义一个协议文件，这个协议文件作为我们的业务载体描述。这个协议是纯java代码编写的，使用的是 jprotobuf, jprotobuf 是对 [google protobuf](https://www.oschina.net/p/protocol+buffers) 的简化使用，性能同等。

```java
/** 请求 */
@ProtobufClass
@FieldDefaults(level = AccessLevel.PUBLIC)
public class HelloReq {
    String name;
}
```



#### Action

游戏服务器的编程，游戏服务器接收业务数据后，对业务数据进行处理；

```java
@ActionController(1)
public class DemoAction {
    @ActionMethod(0)
    public HelloReq here(HelloReq helloReq) {
        HelloReq newHelloReq = new HelloReq();
        newHelloReq.name = helloReq.name + ", I'm here ";
        return newHelloReq;
    }
}
```

一个方法在业务框架中表示一个 Action（既一个业务动作）。

方法声名的参数是用于接收前端传入的业务数据，在方法 return 时，数据就可以被游戏前端接收到。程序员可以不需要关心业务框架的内部细节。

从上面的示例可以看出，**这和普通的 java 类并无区别**。如果只负责编写游戏业务，那么对于业务框架的**学习可以到此为止了**。

游戏编程就是如此简单！

问：我可以开始游戏服务器的编程了吗？

是的，你已经可以开始游戏服务器的编程了。



#### 访问示例（控制台）

当我们访问 here 方法时（通常由游戏前端来请求），控制台将会打印

```basic
┏━━━━━ Debug. [(DemoAction.java:4).here] ━━━ [cmd:1 - subCmd:0 - cmdMerge:65536]
┣ userId: 888
┣ 参数: helloReq : HelloReq(name=塔姆)
┣ 响应: HelloReq(name=塔姆, I'm here )
┣ 时间: 0 ms (业务方法总耗时)
┗━━━━━ Debug [DemoAction.java] ━━━
```

> Debug. [(DemoAction.java:4).here]：
>         表示执行业务的是 DemoAction 类下的 here 方法，4 表示业务方法所在的代码行数。在工具中点击控制台的 DemoAction.java:4 这条信息，就可以跳转到对应的代码中（快速导航到对应的代码）。
>
> userId :  
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;当前发起请求的 用户 id。
> 
> 参数 :  
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;通常是游戏前端传入的值。
> 
> 响应 :  
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;通常是业务方法返回的值 ，业务框架会把这个返回值推送到游戏前端。
> 
> 时间 :  
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;执行业务方法总耗时，我们可根据业务方法总耗时的时长来优化业务。
> 
> 路由信息 :  
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[路由](https://www.yuque.com/iohao/game/soxp4u)是唯一的访问地址。

有了以上信息，游戏开发者可以很快的定位问题。

如果没有可视化的信息，开发中会浪费很多时间在前后端的沟通上。

问题包括：

- 是否传参问题 （游戏前端说传了）

- 是否响应问题（游戏后端说返回了）

- 业务执行时长问题 （游戏前端说没收到响应， 游戏后端说早就响应了）

其中代码导航可以让开发者快速的跳转到业务类对应代码中，在多人合作的项目中，可以快速的知道业务经过了哪些方法的执行，使得我们可以快速的进行阅读或修改；



## 适合人群？

1. 长期从事 web 内部系统开发人员， 想了解游戏的
2. 刚从事游戏开发的
3. 未从事过游戏开发但却对其感兴趣的
4. 对设计模式在实践中的应用和 sofa-bolt 有兴趣的学习者

推荐实际编程经验一年以上的人员



## 快速从零编写服务器完整示例

如果觉得 ioGame 适合你，可以看一下 [快速从零编写服务器完整示例](https://www.yuque.com/iohao/game/zm6qg2) 。在这个示例中，你可以**用很少的代码**实现一个完整的、可运行的、高性能的、稳定的服务器。



## 坦克游戏示例

ioGame 内提供了一个基于 [FXGL](https://www.oschina.net/p/fxgl) 游戏引擎的游戏示例 （[坦克射击](https://www.yuque.com/iohao/game/gqossg)启动文档），FXGL 是纯 java 开发的一个游戏引擎，可以在项目中直接运行。

运行 TankApp.java 文件就可以启动游戏了。

原计划用 [U3D](https://unity.cn/) 来做游戏示例的，但想到大伙还得安装 [u3d](https://unity.cn/) 的环境，就用  [FXGL](https://www.oschina.net/p/fxgl) 来做游戏示例了。



## 参考

什么是 [Action](https://www.yuque.com/iohao/game/sqcevl)。

[快速从零编写服务器完整示例](https://www.yuque.com/iohao/game/zm6qg2)

[坦克示例](https://www.yuque.com/iohao/game/gqossg)（坦克游戏前端）。



## 交流群

### QQ

![img](https://gitee.com/iohao/xmindpic/raw/master/game/QQ1.png)
