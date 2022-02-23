package event

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
			consumer.Recovery(event)
			wait.Done()
		}
	}()

	wait.Wait()

	fmt.Println(*driver.(*Driver).poll.nowSize)
}

// 双向 event
func Test_WaitEventDriver(t *testing.T) {
	driver := NewDriver(200)
	produce := NewProduce(driver)
	consumer := NewConsumer(driver)

	ctx := context.Background()

	wait := sync.WaitGroup{}
	wait.Add(300000)

	go func() {
		for i := 0; i < 300000; i++ {
			event := produce.NewEvent(testEvent)
			event.SetMsg("k", strconv.Itoa(i))
			event.InitWaitEvent()
			produce.Call(ctx, event)
			val, err := event.StartWaitEvent(2 * time.Second)
			require.NoError(t, err)
			fmt.Println(val)
			produce.Recovery(event)
			wait.Done()
		}
	}()

	go func() {
		for {
			event := consumer.Receive(ctx)
			event.ExecWorkAndSendResult(func() (interface{}, error) {
				msg := event.GetMsg("k")
				return "hello: " + msg, nil
			})
		}
	}()

	wait.Wait()

	fmt.Println(*driver.(*Driver).poll.nowSize)
}
