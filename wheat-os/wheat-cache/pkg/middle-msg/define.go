package middlemsg

import (
	"context"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/event"
)

const (
	MiddleMsgKey = "middleMsgKey"
)

func SendMiddleMsg(
	ctx context.Context,
	middleProduce event.ProduceInterface,
	val interface{},
) error {
	if middleProduce == nil {
		return errorx.New("middleProduce not is nil")
	}

	var eventName string

	switch val.(type) {
	case *LogContext:
		eventName = LogContextName
	case *LruCleanContext:
		eventName = LruCleanContextName
	case *LruTTlContext:
		eventName = LruTTlContextName
	case *PulginsInfos:
		eventName = PulginsInfosName
	}

	msgEvent := middleProduce.NewEvent(eventName)
	msgEvent.SetValue(MiddleMsgKey, val)
	middleProduce.Call(ctx, msgEvent)
	return nil
}
