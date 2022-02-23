package event2

import (
	"context"
	"fmt"
	"strconv"
	"sync"
	"testing"
	"time"

	"github.com/stretchr/testify/require"
)

const testEvent = "1001"
const waitTestEvent = "1002"

// 简单的 单向 event 使用
func Test_EventDriver(t *testing.T) {
	driver := NewDriver(2000)
	produce := NewProduce(driver)
	consumer := NewConsumer(driver)

	ctx := context.Background()

	wait := sync.WaitGroup{}
	wait.Add(30000)

	go func() {
		for i := 0; i < 30000; i++ {
			event := produce.NewEvent(testEvent)
			event.SetMsg("k", strconv.Itoa(i))
			produce.Call(ctx, event)

		}
	}()

	go func() {
		for {
			event := consumer.Receive(ctx)
			fmt.Println(event.GetMsg("k"))
			event.Recovery()
			wait.Done()
		}
	}()

	wait.Wait()

}

// 双向 event
func Test_EventDriver_Tow_way(t *testing.T) {

	ctx := context.Background()
	driver := NewDriver(2000)
	produce := NewProduce(driver)
	consumer := NewConsumer(driver)

	go func() {
		for {
			event := consumer.Receive(ctx)
			work, ok := event.GetValue(WorkFuncEventKey)
			if !ok {
				panic("get work key err")
			}
			workFunc, ok := work.(EventWorkFunc)
			if !ok {
				panic("work func err")
			}
			_, err := event.ExecWorkAndSendResult(workFunc)
			require.NoError(t, err)
		}
	}()

	// 一般的 two-way 模式
	for i := 0; i < 10000; i++ {
		event := produce.NewEvent(waitTestEvent)
		event.InitWaitEvent()
		event.SetValue(WorkFuncEventKey, EventWorkFunc(func() (interface{}, error) {
			return i + 1, nil
		}))
		produce.Call(ctx, event)
		res, err := event.StartWaitEvent(2 * time.Second)
		require.NoError(t, err)
		require.Equal(t, res, i+1)
		event.Recovery()
	}

	// 挂起模式，2 秒左右的执行时间
	group := sync.WaitGroup{}
	group.Add(5)
	for i := 0; i < 5; i++ {
		go func(i int) {
			event := produce.NewEvent(waitTestEvent)
			event.InitWaitEvent()
			event.SetValue(WorkFuncEventKey, EventWorkFunc(func() (interface{}, error) {
				// 访问 await Work 来发起一个 异步请求操作
				return EventAwaitFunc(func() (interface{}, error) {
					time.Sleep(time.Second)
					return i + 1, nil
				}), nil
			}))
			produce.Call(ctx, event)
			res, err := event.StartWaitEvent(2 * time.Second)
			require.NoError(t, err)
			require.Equal(t, res, i+1)
			event.Recovery()
			group.Done()
		}(i)
	}

	// 挂起成功不发生超时
	for i := 0; i < 10000; i++ {
		event := produce.NewEvent(waitTestEvent)
		event.InitWaitEvent()
		event.SetValue(WorkFuncEventKey, EventWorkFunc(func() (interface{}, error) {
			return i + 1, nil
		}))
		produce.Call(ctx, event)
		res, err := event.StartWaitEvent(500 * time.Millisecond)
		require.NoError(t, err)
		require.Equal(t, res, i+1)
		event.Recovery()
	}

	group.Wait()

	// 挂起一个高延迟操作, 保证局部操作还在事件中
	group = sync.WaitGroup{}
	group.Add(5)
	for i := 0; i < 5; i++ {
		event := produce.NewEvent(waitTestEvent)
		event.InitWaitEvent()
		event.SetValue(WorkFuncEventKey, EventWorkFunc(func() (interface{}, error) {
			return EventAwaitFunc(func() (interface{}, error) {
				// 返回值为 EventWorkFunc 时， 会重新加入末端队列
				return EventWorkFunc(func() (interface{}, error) {
					return i + 1, nil
				}), nil

			}), nil
		}))
		produce.Call(ctx, event)
		res, err := event.StartWaitEvent(2 * time.Second)
		require.NoError(t, err)
		require.Equal(t, res, i+1)
		event.Recovery()
		group.Done()
		fmt.Println(i)
	}
	group.Wait()

}
