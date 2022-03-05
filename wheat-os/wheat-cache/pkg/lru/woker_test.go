package lru

import (
	"context"
	"testing"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/event2"
	"gitee.com/wheat-os/wheatCache/pkg/logx"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure/stringx"
	"github.com/stretchr/testify/require"
)

func TestWorker(t *testing.T) {
	ctx := context.Background()
	lru := NewLRUCache()
	produce := event2.NewProduce(lru.GetDriver())
	workEvent := produce.NewEvent(OptionEventName)
	workEvent.SetValue(event2.WorkFuncEventKey, event2.EventWorkFunc(func() (interface{}, error) {
		v1 := stringx.NewStringSingle()
		key := proto.BaseKey{
			Key: "v1",
		}
		res, _ := v1.Set("123")
		lru.Add(&key, v1)
		return res, nil
	}))
	workEvent.InitWaitEvent()
	produce.Call(ctx, workEvent)
	res, err := workEvent.StartWaitEvent(2 * time.Second)
	require.NoError(t, err)
	require.Equal(t, res, "123")
}

func TestSingleCache_DelToClearSize(t *testing.T) {
	ctx := context.Background()
	lru := NewLRUCache()
	produce := event2.NewProduce(lru.GetDriver())

	for i := int32(20000); i > 0; i-- {
		workEvent := produce.NewEvent(OptionEventName)
		workEvent.SetValue(event2.WorkFuncEventKey, event2.EventWorkFunc(func() (interface{}, error) {
			v1 := stringx.NewStringSingle()
			key := proto.BaseKey{
				Key: string(i),
				Ttl: 1,
			}
			u := v1.Setbit(i, true)
			lru.Add(&key, v1)
			return u, nil
		}))
		workEvent.InitWaitEvent()
		produce.Call(ctx, workEvent)
		workEvent.StartWaitEvent(2 * time.Second)
		workEvent.Recovery()
	}

	logx.Info("start size is %d", lru.nowSize)
	time.Sleep(10 * time.Second)
	logx.Info("end size is %d", lru.nowSize)
}
