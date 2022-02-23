package logx

import (
	"context"
	"fmt"
	"os"
	"runtime"
	"strings"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/event"
	middleMsg "gitee.com/wheat-os/wheatCache/pkg/middle-msg"
)

func With(ctx context.Context, p event.ProduceInterface) *upLogger {
	return &upLogger{
		ctx:     ctx,
		produce: p,
	}
}

func (l *upLogger) Debug(format string, msg ...interface{}) {
	l.Print("DEBUG", format, msg...)
}

func (l *upLogger) Info(format string, msg ...interface{}) {
	l.Print("INFO", format, msg...)
}

func (l *upLogger) Warn(format string, msg ...interface{}) {
	l.Print("WARN", format, msg...)
}

func (l *upLogger) Error(format string, msg ...interface{}) {
	l.Print("ERROR", format, msg...)
}

func (l *upLogger) Panic(format string, msg ...interface{}) {
	l.Print("ERROR", format, msg...)
	os.Exit(-1)
}

func (l *upLogger) Print(level string, format string, msg ...interface{}) {
	logPrint(4, level, format, msg...)
	sendMsg := &middleMsg.LogContext{
		Level: level,
		Data:  time.Now(),
		Msg:   fmt.Sprintf(format, msg...),
		Route: findPlace(4),
	}
	middleMsg.SendMiddleMsg(l.ctx, l.produce, sendMsg)
}

func (l *upLogger) Debugln(msg ...interface{}) {
	l.Print("DEBUG", "%s", format(msg...))
}

func (l *upLogger) Infoln(msg ...interface{}) {
	l.Print("INFO", "%s", format(msg...))
}

func (l *upLogger) Warnln(msg ...interface{}) {
	l.Print("WARN", "%s", format(msg...))
}

func (l *upLogger) Errorln(msg ...interface{}) {
	l.Print("ERROR", "%s", format(msg...))
}

func (l *upLogger) Panicln(msg ...interface{}) {
	l.Print("ERROR", "%s", format(msg...))
	os.Exit(-1)
}

func Debug(format string, msg ...interface{}) {
	logPrint(3, "DEBUG", format, msg...)
}
func Info(format string, msg ...interface{}) {
	logPrint(3, "INFO", format, msg...)
}
func Warn(format string, msg ...interface{}) {
	logPrint(3, "WARN", format, msg...)
}
func Error(format string, msg ...interface{}) {
	logPrint(3, "ERROR", format, msg...)
}
func Panic(format string, msg ...interface{}) {
	logPrint(3, "PANIC", format, msg...)
	os.Exit(-1)
}

func Debugln(msg ...interface{}) {
	logPrint(3, "DEBUG", "%s", format(msg...))
}
func Infoln(msg ...interface{}) {
	logPrint(3, "INFO", "%s", format(msg...))
}
func Warnln(msg ...interface{}) {
	logPrint(3, "WARN", "%s", format(msg...))
}
func Errorln(msg ...interface{}) {
	logPrint(3, "ERROR", "%s", format(msg...))
}
func Panicln(msg ...interface{}) {
	logPrint(3, "PANIC", "%s", format(msg...))
	os.Exit(-1)
}

func logPrint(floor int, level string, format string, msg ...interface{}) {
	place := findPlace(floor)
	datetime := fmt.Sprintf("%s", time.Now())[0:19]

	switch level {
	case "DEBUG":
		fmt.Printf("\033[1;37;40m")
	case "INFO":
		fmt.Printf("\033[1;32;40m")
	case "WARN":
		fmt.Printf("\033[1;33;40m")
	default:
		fmt.Printf("\033[1;31;40m")

	}

	//fmt.Println(level, datetime, fmt.Sprintf(format, msg...))
	fmt.Printf("%s\t%v\t%s\n", level, datetime, fmt.Sprintf(format, msg...))

	for _, lv := range stath {
		if strings.ToUpper(lv) == strings.ToUpper(level) {
			fmt.Println(place)
			break
		}
	}
}

func findPlace(floor int) string {

	var (
		place string
		i     = floor
	)

	for {
		_, file, line, _ := runtime.Caller(i)
		if line == 0 {
			break
		}
		i++
		place = fmt.Sprintf("%s:%d\n%s", file, line, place)
	}

	return place
}

func format(message ...interface{}) (context string) {
	for _, msg := range message {
		context = fmt.Sprintf("%s\t%v", context, msg)
	}

	return context
}
