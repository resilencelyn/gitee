package lru

import (
	"context"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/event"
	"gitee.com/wheat-os/wheatCache/pkg/event2"
	"gitee.com/wheat-os/wheatCache/pkg/logx"
	mMsg "gitee.com/wheat-os/wheatCache/pkg/middle-msg"
)

func (lru *SingleCache) lruSingleWork() {
	ctx := context.Background()
	for {
		workEvent := lru.lruConsumer.Receive(ctx)
		workFunc, ok := workEvent.GetValue(event2.WorkFuncEventKey)
		if !ok {
			workEvent.SetResultErr(errorx.LruNotWorkFuncEventErr())
			continue
		}

		switch workEvent.GetEventName() {
		case OptionEventName:
			if work, ok := workFunc.(event2.EventWorkFunc); ok {
				workEvent.ExecWorkAndSendResult(work)
			}

		case CleanEventName:
			// 对当前的io数量进行判断
			ioNum := lru.GetDriver().GetLength()
			if ioNum > lru.lruMaxDiverSize/2 {
				lru.lruCleanProduce.Call(ctx, workEvent)
				continue
			}
			if work, ok := workFunc.(event2.EventWorkFunc); ok {
				workEvent.ExecWorkAndSendResult(work)
			}

		case TtlEventName:
			if work, ok := workFunc.(event2.EventWorkFunc); ok {
				workEvent.ExecWorkAndSendResult(work)
			}
		}
	}
}

// 执行过期事件
func (lru *SingleCache) lruTtlWork() {

	ctx := context.Background()

	// 清理事件
	go func() {
		work := event2.EventWorkFunc(func() (interface{}, error) {

			beforeTime := time.Now().Unix()
			cle := lru.lruTtlManage.detachNum
			if cle > len(lru.lruTtlManage.memoryKey) {
				cle = len(lru.lruTtlManage.memoryKey)
			}

			keys := make([]string, 0)
			for i := 0; i < cle; i++ {
				key := <-lru.lruTtlManage.memoryKey
				keys = append(keys, key)
				lru.delByKeyAndExTtl(key, beforeTime)
			}
			return keys, nil
		})

		cleanTTlTicker := time.NewTicker(500 * time.Millisecond)
		defer cleanTTlTicker.Stop()

		for {
			// 清理事件
			<-cleanTTlTicker.C
			if len(lru.lruTtlManage.memoryKey) == 0 {
				continue
			}

			ttlEvent := lru.lruCleanProduce.NewEvent(TtlEventName)
			ttlEvent.SetValue(event2.WorkFuncEventKey, work)
			ttlEvent.InitWaitEvent()

			lru.lruCleanProduce.Call(ctx, ttlEvent)
			keys, err := ttlEvent.StartWaitEvent(time.Second * 2)
			ttlEvent.Recovery()

			mMsg.SendMiddleMsg(ctx, lru.middleProduce, mMsg.LruTTlContext{
				Keys:      keys.([]string),
				CleanTime: time.Now(),
			})

			if err != nil {
				logx.With(ctx, lru.middleProduce).Errorln(err)
			}
		}
	}()

	// 收集事件
	for {
		time.Sleep(1 * time.Second)
		lru.lruTtlManage.ttlKeyToMemoryBySecond()
	}

}

func (lru *SingleCache) cleanWork() {
	cxt := context.Background()
	work := event.EventWorkFunc(func() (interface{}, error) {
		err := lru.DelToClearSize()
		return nil, err
	})

	for {

		time.Sleep(2 * time.Second)
		if lru.clearSize < lru.nowSize {
			lruCleanEvent := lru.lruCleanProduce.NewEvent(CleanEventName)
			lruCleanEvent.SetValue(event2.WorkFuncEventKey, work)

			lruCleanEvent.InitWaitEvent()
			lru.lruCleanProduce.Call(cxt, lruCleanEvent)
			_, err := lruCleanEvent.StartWaitEvent(defaultWaitTime)
			if err != nil {
				logx.With(cxt, lru.middleProduce).Errorln(err)
			}

			// 归还
			lruCleanEvent.Recovery()
		}
	}
}
