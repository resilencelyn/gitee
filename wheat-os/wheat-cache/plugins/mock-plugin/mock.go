package mockplugin

import (
	"fmt"
	"net/http"
	_ "net/http/pprof"

	_ "gitee.com/wheat-os/wheatCache/conf"
	"gitee.com/wheat-os/wheatCache/pkg/logx"
	"github.com/spf13/viper"
)

type MockPlugin struct {
}

func (m *MockPlugin) Init() {
	addr := viper.GetString("mock-plugin.pprof-addr")
	if addr != "" {
		go func() {
			logx.Debugln(http.ListenAndServe(addr, nil))
		}()
	}

	logx.Debug("mock-plugins status: start")
}

func (m *MockPlugin) Exec(msg interface{}) (interface{}, error) {
	fmt.Println(msg)
	return nil, nil
}

func (m *MockPlugin) Name() string {
	return "mock-plugins"
}

func (m *MockPlugin) Describe() string {
	return "这是一个测试用的插件"
}

func NewPlugin() *MockPlugin {
	return &MockPlugin{}
}
