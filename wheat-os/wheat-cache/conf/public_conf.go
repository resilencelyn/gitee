package conf

import (
	"log"
	"sync"

	"github.com/spf13/viper"
)

const (
	linuxPath = "/etc/wheat-cache/"
)

var confLock sync.Once

func init() {
	confLock.Do(func() {
		setDefaultConfValue()
		err := LoadConf("")
		switch err.(type) {
		case nil:
		case viper.ConfigFileNotFoundError:
			formatPath := []string{linuxPath}
			log.Fatalf("the profile could not be read, read path:%v", formatPath)
		default:
			log.Fatalf("the resolution of the profile failed, err: %v", err)

		}
	},
	)
}

func setDefaultConfValue() {
	// 设置一些默认值
	viper.SetDefault("version", "base-01")
	defaultStorage()
}

func LoadConf(path string) error {
	if path != "" {
		viper.AddConfigPath(path)
	}
	viper.SetConfigName("wheat-cache")

	// 添加默认读取地址
	// linux
	viper.AddConfigPath(linuxPath)

	viper.SetConfigType("yaml")

	err := viper.ReadInConfig()
	if err != nil {
		return err
	}
	return nil
}
