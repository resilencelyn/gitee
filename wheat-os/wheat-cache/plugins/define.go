package plugins

type PluginInterface interface {
	Init()                                 // 初始化
	Exec(interface{}) (interface{}, error) // 处理用户发送事件
	Name() string                          // 获取中间件名称
	Describe() string                      // 描述
}
