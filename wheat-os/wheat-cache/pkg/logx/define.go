package logx

import (
	"context"
	"sync"

	"gitee.com/wheat-os/wheatCache/pkg/event"
	"github.com/spf13/viper"
)

type LogLevelState int8

var (
	once  sync.Once
	stath []string
)

type upLogger struct {
	ctx     context.Context
	produce event.ProduceInterface
}

func init() {
	once.Do(func() {
		stath = viper.GetStringSlice("logPrint.stath")
	})
}

type logInterface interface {
	Debug(format string, msg ...interface{})
	Info(format string, msg ...interface{})
	Warn(format string, msg ...interface{})
	Error(format string, msg ...interface{})
	Panic(format string, msg ...interface{})

	Debugln(msg ...interface{})
	Infoln(msg ...interface{})
	Warnln(msg ...interface{})
	Errorln(msg ...interface{})
	Panicln(msg ...interface{})

	Print(level string, format string, msg ...interface{})
}
