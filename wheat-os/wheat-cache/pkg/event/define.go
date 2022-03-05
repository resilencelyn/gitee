package event

import (
	"context"
)

const (
	defaultEventState = int32(iota) //默认情况下的状态
	waitEventState                  // 等待状态
	workEventState                  //工作状态
	closeEventState                 //事件关闭状态
)

type EventWorkFunc func() (interface{}, error)

type DriverInterface interface {
	Get() *event
	Put(*event)
	GetLength() int
	NewEvent(string) *event
	Recovery(*event)
}

type ProduceInterface interface {
	Call(context.Context, *event)
	NewEvent(string) *event
	Recovery(*event)
}

type ConsumerInterface interface {
	Receive(ctx context.Context) *event
	Recovery(*event)
}
