package middle

import (
	"context"
	"fmt"
	"testing"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/event"
	middleMsg "gitee.com/wheat-os/wheatCache/pkg/middle-msg"
)

func Test_middleware_loadPlugins(t *testing.T) {
	m := NewMiddleWare()
	m.loadPlugins()

	fmt.Println(m.plugins)
}

func TestWorker(t *testing.T) {
	ctx := context.Background()
	m := NewMiddleWare()

	product := event.NewProduce(m.GetEventDriver())
	middleMsg.SendMiddleMsg(ctx, product, &middleMsg.LogContext{
		Msg: "debug msg",
	})

	middleMsg.SendMiddleMsg(ctx, product, &middleMsg.PulginsInfos{
		Infos: []*middleMsg.PulginsInfo{
			{
				Desc: "miss",
			},
		},
	})

	middleMsg.SendMiddleMsg(ctx, product, &middleMsg.LruTTlContext{
		Keys: []string{"1", "2", "3"},
	})

	time.Sleep(1 * time.Second)
}
