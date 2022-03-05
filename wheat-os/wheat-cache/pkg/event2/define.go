package event2

import (
	"context"
)

const (
	initEventState     = int32(iota) // 初始化状态
	waitEventState                   // 等待状态
	workEventState                   // 工作状态
	closeEventState                  // 事件关闭状态
	recoveryEventState               // 事件回收状态
)

const (
	awaitThread = 3
)

const (
	WorkFuncEventKey = "workFunc"
)

// 线程安全
type EventWorkFunc func() (interface{}, error)

// 挂起事件, 线程不安全
type EventAwaitFunc func() (interface{}, error)

// 实际操作
type awaitFunc func() (*event, interface{}, error)

type DriverInterface interface {
	Get() *event
	Put(*event)
	GetLength() int
	NewEvent(string) *event

	await(awaitFunc)
	recovery(e *event)
}

type ProduceInterface interface {
	Call(context.Context, *event)
	NewEvent(string) *event
}

type ConsumerInterface interface {
	Receive(ctx context.Context) *event
}
