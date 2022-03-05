package middle

import (
	"context"

	"gitee.com/wheat-os/wheatCache/pkg/logx"
	middleMsg "gitee.com/wheat-os/wheatCache/pkg/middle-msg"
)

func (m *MiddleWare) startWork() {

	for i := 0; i < m.consumerCount; i++ {
		go func() {
			ctx := context.Background()
			for {
				workEvent := m.eventConsumer.Receive(ctx)
				plugs := m.plugins[workEvent.GetEventName()]
				msg, ok := workEvent.GetValue(middleMsg.MiddleMsgKey)
				m.eventConsumer.Recovery(workEvent)

				if !ok {
					logx.With(ctx, m.eventProduce).Error("get event value err，not key:%s", middleMsg.MiddleMsgKey)
					continue
				}

				// 发送事件到 全部的 plugs 里
				for _, val := range plugs {
					_, err := val.Exec(msg)
					if err != nil {
						logx.With(ctx, m.eventProduce).Errorln(err)
					}
				}
			}
		}()
	}
}
