package conf

import "github.com/spf13/viper"

func defaultStorage() {
	// aof
	viper.SetDefault("storage.aof-path", "/etc/wheat-cache/wheat.aof")
	viper.SetDefault("storage.aof-flush-time", 5)
	viper.SetDefault("storage.aof-check-time", 1)
	viper.SetDefault("storage.aof-check-freq", 20)
}
