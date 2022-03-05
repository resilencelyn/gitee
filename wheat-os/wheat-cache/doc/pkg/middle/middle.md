###  中间件调用


###  创建事件

event := event.NewEvent("logcontext")


### 创建驱动

middleware := NewMiddleWare()

### 将事件推入驱动

middleware.eventProduce.Call(ctx, event)


### 获取驱动的事件
middleware.eventConsumer.Reciver(ctx)



### 插件接口
type MiddleToolsInterface interface {
	Init()                                 // 初始化
	Exec(interface{}) (interface{}, error) // 处理用户发送事件
	Name() string                          // 获取中间件名称
	Describe() string                      // 描述
}

### 插件的New方法规定为  NewMiddleWare() 
    每个插件都要定义 NewMiddleWare()



### 将插件名 “logMiddle” 注册到配置文件wheat-cache.yaml,其他插件注册
plugins-control:
  logcontext: ["logMiddle"]









