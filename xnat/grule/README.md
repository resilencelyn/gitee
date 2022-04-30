# Groovy DSL 动态规则(rule)执行引擎

> ___DSL___(特定领域语言): 开发 和 业务 共识的语言. 方便业务表达需求, 方便开发理解业务

> 一个业务需求(一般程序或一个接口)可以抽象成为: 按一定业务逻辑规则处理数据,然后返回数据
> > + 一个人可以用成百上千个属性组成,由这些属性衍生出新的属性(例如,好人/坏人) 返回一个业务结果(0..多个属性值)
> > + 一般接口: 查询数据库或接口,对数据进行简单逻辑计算,最后返回一些属性

> 框架提供一种通用流程: 从多种数据源获取数据转换成属性, 并按业务规则 _DSL_ 执行  
> 框架可以执行成千上万的这种通用程序逻辑, 可以动态修改流程中的每个过程(数据源,属性,规则),即时生效

> 可用于风控规则, 电商价格规则, 其它业务规则

# 框架概念
* _决策_: 抽象一个业务需求(一般程序或一个接口)为一条决策,一条决策包含多个业务规则
* _规则_: 框架用自创 ___DSL规则___ 语言表达业务逻辑
```
    规则 {
        规则名 = '芝麻分限制'
        拒绝 { 芝麻分 < 620 }
    }
```
* _指标/字段/变量/属性_: 规则中的变量 芝麻分
* _收集器_: 指标值来源于数据源配置: 接口,SQL,脚本3种类型

> 例子 http://39.104.28.131:9090/ test:test

# ___规则___ 定义
* 由 ___规则名___ 和多个 ___拒绝___, ___通过___, ___人工___, ___操作___ 组成(按顺序依次执行)
```
规则 {
    规则名 = '大小比较'
    拒绝 { 年龄 > 40 || 年龄 < 25 }
}
```
```
规则 {
    规则名 = '通过规则'
    通过 { 年龄 == 30 }
}
```
```
规则 {
    规则名 = '赋值规则'
    操作 { 产品代码 = 'xx' }
}
```
```
规则 { // 电商价格规则
    规则名 = '条件赋值规则'
    操作 {
        if (当前时间 > '2021-03-02 00:00:00') {
            单价 = 10
        } 
    }
}
```
```
规则 {
    规则名 = '列表判断规则'

    风险地 = ["成都", "巴中"]

    拒绝 { 工作地 in 风险地 }
}
```
```
规则 {
    规则名 = '包含规则'
    拒绝 { "$姓名".contains("xx") }
}
```

# ___策略___ 定义
* 由 ___策略名___ 和多个  ___操作___, ___条件___, ___规则___, ___评分卡___, ___决策___ 组成(按顺序依次执行)
* ___条件___ 函数返回false, 则跳出, 继续执行下一个策略
## ___规则___ 策略
```
策略 {
    策略名 = '004244'
    
    操作 { jj_代码 = '004244' }
    条件 { jj_名 }
    
    规则 {
       拒绝 { true }
    }
}
```
## ___评分卡___ 策略
___模型___ : 是由多个评分条目组成
* 普通评分条目: 由 属性名, 值匹配范围, 分值 组成
  * 例: ['年龄', 18..24, 40]
  * 例: ['工作成市', ['成都', '巴中'], 99]
* 函数分条目: 一个求值函数 组成
  * 例: [{ -> 100}]
* 变量分条目: 一个数值变量 组成
  * 例: ['芝麻分']
* 动态分条目: 一个数值变量,及其值计算函数 组成
  * 例: ['逾期次数', {次数 -> -((次数?:0) * 100)}]
* 模型分计算: 依次遍历每个评分条目, 得到匹配的分值相加
* 最终评分 = ___基础分___ + 模型计算得分

___赋值变量___ : 把评分结果赋值给的变量
```
策略 {
    策略名 = '测试评分卡'
    
    评分卡 {
       评分卡名 = "测试评分卡"
       基础分 = 600
       模型 = [
           ['年龄', 18..24, 40],
           ['年龄', 25..45, 80],
           ['年龄', 46..60, 50],
           ['性别', 'F', 50],
           ['性别', 'M', 60],
           [{ -> 100}], // 函数分
           ['芝麻分'], //变量值分
           ['逾期次数', {次数 -> -((次数?:0) * 100)}], // 动态分
           ['工作成市', ['成都', '巴中'], 99]
       ]
       赋值变量 = '评分结果'
    }
    
    规则 {
      规则名 = "评分判断"
      拒绝 { 评分结果 < 60 }
    }
}
```
## 委托子 ___决策___ 策略
* 嵌套其它决策执行(_注意不要循环嵌套_)
* 会形成一条单独的决策执行记录(id为父决策的id为前缀)
* ___异步___: 
  * 不配置默认为false; 为true时, ___输出___ 配置不生效
  * 值为false: 并且 当子决策执行结果为Reject的时,会结束当前决策执行
* ___传参___: 共享当前决策数据给子决策使用
* ___输出___: 会设置 到当前执行上下文, 可以使用. 没有配置时: 则使用子决策的返回属性作为 ___输出___
```
策略 {
    策略名 = '测试子决策'
    
    决策 {
        决策id = "cd7049e7e76441fa8315c2e1bd883042"
       // 不配置默认为false, 异步为true时, 输出则不生效
       // 异步 = true
       
       // 左(子决策) = 右(父决策)
       传参 {
          p1 = 1
          p2 = "aa"
          p3 = p3 // p3的值从父决策来
       }
       
       // 左(父决策) = 右(子决策)
       输出 {
          p111 = p1 // p1的值从子决策来
          p222 = p3 // p3的值从子决策来
       }
    }
}
```

# 决策定义
* 由 ___决策名___, ___决策描述___ 和多个 ___操作___, ___策略___ 组成(按顺序依次执行) 
* 触发当前决策: http://ip:port/decision/{id}/decide
* 接口返回: 3种结果(Accept, Reject, Review) 和多个返回属性值(由配置指定)
```json
// 返回示例
{
  "decideId": "647f297a2e4540dfa93991b5b6e7b44d",
  "result": "Accept",
  "decisionId": "jj_analyse",
  "status": "0000",
  "desc": null,
  "data": {
    "jj_code": "165525"
   }
}
```
* 依次从上往下执行
```
决策名 = '测试1'
决策描述 = ''

// 返回的调用决策方的结果属性值
// 返回属性 '身份证号码'

// 预操作执行
操作 {}

策略 {
    策略名 = '预处理'
    // 条件 { true }

    规则 {
        规则名 = '年龄限制'
        拒绝 { 年龄 > 40 || 年龄 < 25 }
    }
}
```
## ___函数定义___: 自定义决策函数
```
决策名 = '测试1'

策略 {
    策略名 = '测试策略'
    规则 {
        规则名 = '使用函数'
        操作 {
            钉钉消息("发个消息") 
        }
    }
}

函数定义("钉钉消息") {String msg -> 
    Utils.http()
        .post("https://oapi.dingtalk.com/robot/send?access_token=7e9d8d97e6b5e76a6a07b0c5d7c31e82f0fbdb8ced1ac23168f9fd5c28c57f1a")
        .jsonBody(JSON.toJSONString(
            [msgtype: 'text', text: [content: "Fund: " + msg], at: ['isAtAll': false]]
        )).debug().execute()
}
```

![Image text](https://gitee.com/xnat/tmp/raw/master/img/decisions.png)


# 指标/字段/属性
## 使用 '年龄' 指标
```
拒绝 { 年龄 > 50 }
```

## 指标值获取
<!-- 
@startuml
skinparam ConditionEndStyle hline
:指标名;
if (执行上下文) then (已存在值)
  if (值来源是否为收集器) then (是)
    :收集器;
  else (否)
  :返回值;
  detach
  endif
else (不存在值)
  if (入参) then (存在值)
    :保存指标值到 __执行上下文__;
    :返回值;
    detach
  else (不存在)
    :收集器;
  endif
endif
->//计算//;
:指标值;
:保存指标值到 __执行上下文__;
:返回值;
@enduml
-->
![Image text](http://www.plantuml.com/plantuml/png/SoWkIImgAStDuIhEpimhI2nAp5LmpizBoIp9pCzppKi9BgdCILN8oCdCI-MoUjRJ_cn1-zC9lTPScMaA6iywbxzOsFDaHzUJ7TtFfhLhAfHafEOfQ3pTlkdfsXbFvwnush17aqj10QGKo7msT-cpNHEUpLZ_TCAo9pjsFPkoxUNijgSpLy2q0MM0ge702Yvb3UIdvXIdAcW0zO0ahLxid_9qzZoWQI2fbDIInEGCa1gWUzEu82gVxEZ5jBrrwTF-9fX5S6ceTK_spmKAGVm657tQiK4XFXxDR_7n80lH7O3a13JBCNm2ToM4rBNJrt-nRk7pTTFrzQrX0GisbMZd83l50MWgC0u1)

## 指标列表
![Image text](https://gitee.com/xnat/tmp/raw/master/img/rule_fields.png)

# 收集器(数据源)
> 支持从SQL(数据库),http(接口),script(自定义groovy脚本函数)获取数据
## 收集器执行
<!--
@startuml
skinparam ConditionEndStyle hline
:指标名;
-> 选择;
:收集器;
-> 根据上下文计算;
:数据key;
note left
  用于判断是否需要重复计算
end note
if (执行上下文) then(存在key)
  :收集结果;
else (不存在)
  :执行收集器;
  :保存收集结果到 __执行上下文__;
  note right
    收集结果可能是多个指标的值映射
    也可能是单个指标值
  end note
endif

split
 :多值映射;
  note left
    收集结果是个Map
  end note
  -> get;
  if (指标值) then(是个函数)
      :值函数计算;
  endif
split again
 :单值;
end split

:指标值;
:返回值;
@enduml
-->
![Image text](http://www.plantuml.com/plantuml/png/PLBDpX8n5DttARhaM_W2Cudv4kFIbGymcOG2pJSKmjG5Eup40M6KH404H4W8eWa_ApEeX9UPTcRUmjkMZAXTDQVdd7lklRttkTlWBweUyXyegxiDjugVr5YHSbfZJrdnEMzw15SyoWYoP3-Goq0CGXizUeopLbVslje03xzdizVYurR3SdcIuJwEtiHHJuw3TBzAzXyKQtG4_84qRSHgd62Fb3Z2E1bkunzlHMSjnpivEOZ19fktqitBB0Z5EZHgH5WHAn6Y9LoGtI_fgfyNkCEyGbX1x2PYlWNxEp2zHaf-lfUBkOs8vnDSYAFGa3J3kDn41oo-V0B6hLPqZjXn_gdeE8gjcsZGSWMwWFENwjqXVNLMtQodSVJDZ2sPjaNhbvminR6j5V7fynzYECg9m8Btl6Muq192VjsZKCa2ozmcZm6p_3y5s8BdCxT-wuOnhAXCk9AgOUObhsCq8X6SOLqm9tqiU3Q8MOVIwbGc57RBBcKgMZW2fgstPUBcNqR1LdePYjb2t--10v_kDm00)

## 决策执行过程中生产的数据收集
![Image text](https://gitee.com/xnat/tmp/raw/master/img/collect_records.png)

# 决策执行
## 决策执行记录
![Image text](https://gitee.com/xnat/tmp/raw/master/img/decision_records.png)

## 决策执行详情
![Image text](https://gitee.com/xnat/tmp/raw/master/img/decision_result_detail.png)

# 统计图
## 决策执行统计 
![Image text](https://gitee.com/xnat/tmp/raw/master/img/decision_dashbrod.png)

## 决策规则统计 
![Image text](https://gitee.com/xnat/tmp/raw/master/img/countRule.png)


# 权限
> **静态权限**
> + _权限管理_, _登陆_, _用户-创建_, _决策-创建_, _字段-添加_, _字段-查看_, _字段-更新_, _字段-删除_, _收集器-添加_, _收集器-查看_, _收集器-更新_, _收集器-删除_, _操作历史查看_
> > _权限管理_: 可修改任何用户的任何权限
> 
> > _用户-创建_: 创建者默认拥有被创建的用户的所有动态权限
> 
> > _操作历史查看_: 目前支持 _决策-查看-{某个决策}_, _字段-查看_, _收集器-查看_

> **动态资源(用户)权限**
> + 被创建用户默认拥有 _用户-查看-{自己}_ 此动态权限
> + 每添加一个用户 就会生成与之对应的以下动态权限
> > _用户-查看-{用户1}_: 能搜索到此用户1  
> 
> > _用户-删除-{用户1}_: 能删除用户1
> 
> > _用户-重置密码-{用户1}_: 能为用户1重置密码
> 
> > _用户-继承决策权限-{用户1}_: 能自动拥有用户1新创建的决策的所动态有权限 
> 
> > _用户-修改权限-{用户1}_: 能修改指定用户1权限      
> > > + 如果: 修改用户有 _权限管理_, 则可修改用户1的所有权限
> > > + 否则: 能修改用户1的权限只能是, 当前用户所拥有的权限(除 _用户-创建_)

> **动态资源(决策)权限** 
> + 创建者默认自动拥有生成动态权限
> + 每添加一个决策 就会生成与之对应的以下动态权限
> > _决策-查看-{决策1}_: 拥有查看决策1的权限  
> 
> > _决策-删除-{决策1}_: 拥有删除决策1的权限
> 
> > _决策-更新-{决策1}_: 拥有更新决策1的权限

## 用户分类(虚称)
* 超级用户(系统管理员): 拥有 _权限管理_ 的用户
* 用户管理员: 拥有 _用户-创建_ 的用户
* 普通用户: 非 _权限管理_ 和 _用户-创建_ 的用户

![Image text](https://gitee.com/xnat/tmp/raw/master/img/userlist.png)

# 使用说明
> 系统基于java轻量级框架: [Tiny](https://gitee.com/xnat/tiny)
1. git clone https://gitee.com/xnat/grule.git 或 git clone https://github.com/xnat9/grule.git
2.
  >+ linux: sh start.sh -Xmx512m -Xms512m -Dlog.totalSizeCap=50G
  >+ windows: ./start -Xmx512m -Xms512m

* api文档地址: http://localhost:9090/api-doc/

### 环境要求
jdk8, gradle7+

### 可以更换自已的数据库 (默认用的h2)
> 数据库要求: mysql5.7+(最好是8.0.4+), MariaDB10.2+

```sql
-- 创建数据库
CREATE DATABASE IF NOT EXISTS rule DEFAULT CHARSET utf8 COLLATE utf8_general_ci;
```
```properties
# 替换 conf/app.properties 中的 jpa_rule.url 配置项
jpa_rule.url=jdbc:mysql://localhost:3306/rule?useSSL=false&user=root&password=root&useUnicode=true&characterEncoding=utf-8&allowPublicKeyRetrieval=true
```

### 开发环境运行
> 需要添加Groovy sdk 3.0.8

IntelliJ IDEA 运行 main.groovy 启动


# v2.0.1 ing...
- [ ] feat: 重新执行决策(自动/手动)
- [ ] opt: 实体关联(无外键)
- [ ] opt: 深入优化决策DSL变量作用域
- [ ] feat: 新增 DSL SPEC: 接口
- [ ] feat: redis 收集器
- [ ] upgrade: groovy 4.0.1
- [ ] feat: 字符串 in 或者 contains 操作符


# 参与贡献
xnatural@msn.cn