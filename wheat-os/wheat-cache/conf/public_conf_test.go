package conf

import (
	"testing"

	"github.com/spf13/viper"
	"github.com/stretchr/testify/require"
)

func TestConf(t *testing.T) {

	// 外部导入 conf.yaml 需要导入 conf 包
	// 每次迁移文件时, 使用 sudo make init-conf来将yam文件迁移到指定的文件夹下
	// get 使用, 读取 public_conf 配置文件
	h := viper.Get("storage.host")
	require.Equal(t, h, "127.0.0.1")

	h = viper.Get("env")
	require.Equal(t, h, "dev")

	// set 使用
	viper.Set("host", "1222")
	host := viper.GetString("host")
	require.Equal(t, host, "1222")
}

func TestMiddleConf(t *testing.T) {
	ct := viper.GetStringSlice("plugins-control.logcontext")
	require.Equal(t, ct, []string{"logMiddle"})

	d := viper.GetInt("middleware-driver.driverCount")
	require.Equal(t, d, 1000)
	c := viper.GetInt("middleware-driver.middleConsumerCount")
	require.Equal(t, c, 5)
}
