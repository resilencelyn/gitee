package logx

import (
	"context"
	"testing"

	_ "gitee.com/wheat-os/wheatCache/conf"
	"gitee.com/wheat-os/wheatCache/pkg/event"
)

func TestStd(t *testing.T) {
	Info("%d%s", 11, "Info")
	Debug("%d%s", 11, "Debug")
	Warn("%d%s", 11, "Warn")
	Error("%d%s", 11, "Error")

	Infoln(1321, "dwad", 0x9812933)
	Debugln(1321, "dwad", 0x9812933)
	Warnln(1321, "dwad", 0x9812933)
	Errorln(1321, "dwad", 0x9812933)

	//Panic("%d%s", 11, "Panic")

	logger := With(context.Background(),
		event.NewProduce(event.NewDriver(100)))

	logger.Info("%d%s", 11, "Info")
	logger.Debug("%d%s", 11, "Debug")
	logger.Warn("%d%s", 11, "Warn")
	logger.Error("%d%s", 11, "Error")
	//logger.Panic("%d%s", 11, "Panic")

	logger.Infoln(11, "Info")
	logger.Debugln(11, "Debug")
	logger.Warnln(11, "Warn")
	logger.Errorln(11, "Error")

}
