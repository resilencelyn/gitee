package event2

import (
	"sync/atomic"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
)

type event struct {
	msgCtx       map[string]interface{}
	eventName    string
	msg          map[string]string // 消息
	waitResult   chan interface{}  // 等待返回
	err          error
	eventStatus  int32
	ttlManage    *time.Timer
	parentDriver DriverInterface
}

func (e *event) reset() {
	if e.ttlManage != nil {
		e.ttlManage.Stop()

		if len(e.ttlManage.C) > 0 {
			<-e.ttlManage.C
		}
	}

	e.err = nil

	// 清空结果
	if len(e.waitResult) != 0 {
		<-e.waitResult
	}
}

func (e *event) Recovery() {
	e.parentDriver.recovery(e)
}

func (e *event) SetMsg(key string, val string) {
	if e.msg == nil {
		e.msg = make(map[string]string)
	}
	e.msg[key] = val
}

func (e *event) GetMsg(key string) string {
	if e.msg == nil {
		return ""
	}
	return e.msg[key]
}

func (e *event) GetEventName() string {
	return e.eventName
}

// SetValue 写入 ctx 传递用参数
func (e *event) SetValue(key string, value interface{}) {
	if e.msgCtx == nil {
		e.msgCtx = make(map[string]interface{})
	}
	e.msgCtx[key] = value
}

func (e *event) GetValue(key string) (interface{}, bool) {
	if e.msgCtx == nil {
		return nil, false
	}
	val, ok := e.msgCtx[key]
	return val, ok
}

func (e *event) InitWaitEvent() {
	e.reset()
	if e.waitResult == nil {
		e.waitResult = make(chan interface{})
	}

	atomic.SwapInt32(&e.eventStatus, waitEventState)
}

func (e *event) SetResultErr(err error) {
	if !atomic.CompareAndSwapInt32(&e.eventStatus, waitEventState, workEventState) {
		return
	}

	e.err = err
	e.waitResult <- nil
}

// StartWaitEvent 开始一个等待任务
func (e *event) StartWaitEvent(ttl time.Duration) (interface{}, error) {
	if e.ttlManage == nil {
		e.ttlManage = time.NewTimer(ttl)
	} else {
		e.ttlManage.Reset(ttl)
	}

	for {
		select {
		case <-e.ttlManage.C:
			if atomic.CompareAndSwapInt32(&e.eventStatus, waitEventState, closeEventState) {
				return nil, errorx.TimeOutErr()
			}
			continue

		case result := <-e.waitResult:
			atomic.SwapInt32(&e.eventStatus, closeEventState)
			return result, e.err
		}
	}
}

// 实际执行推送
func (e *event) execWorker(res interface{}, err error) {
	switch work := res.(type) {
	case EventAwaitFunc:
		await := func() (*event, interface{}, error) {
			result, err := work()
			return e, result, err
		}
		e.parentDriver.await(await)

	case EventWorkFunc:
		e.InitWaitEvent()
		e.SetValue(WorkFuncEventKey, work)
		e.parentDriver.Put(e)

	default:
		e.err = err
		e.waitResult <- res
	}
}

func (e *event) ExecWorkAndSendResult(work EventWorkFunc) (interface{}, error) {
	if !atomic.CompareAndSwapInt32(&e.eventStatus, waitEventState, workEventState) {
		return nil, errorx.New("not wait status, exec err")
	}

	res, err := work()
	e.execWorker(res, err)
	return res, err
}

type driver struct {
	waitQueue  chan awaitFunc
	eventQueue chan *event
	levelQueue chan *event

	// event 池的实现
	poll        chan *event
	maxPoolSize int32
	nowPoolSize int32
}

func NewDriver(maxSize int) DriverInterface {
	d := &driver{
		// pool
		maxPoolSize: int32(maxSize),
		nowPoolSize: 0,
		poll:        make(chan *event, maxSize),

		// waitQueue 1/3 的挂起指标
		waitQueue:  make(chan awaitFunc, maxSize/3),
		levelQueue: make(chan *event, maxSize/3),
		eventQueue: make(chan *event, maxSize),
	}
	d.awaitWorker()
	return d
}

func (d *driver) NewEvent(name string) *event {

	issSize := atomic.LoadInt32(&d.nowPoolSize)
	if issSize < d.maxPoolSize {
		atomic.AddInt32(&d.nowPoolSize, 1)
		return d.newEvent(name)
	}
	e := <-d.poll
	e.eventName = name
	return e
}

func (d *driver) newEvent(name string) *event {
	status := initEventState
	return &event{
		eventStatus:  status,
		parentDriver: d,
		eventName:    name,
	}
}

// 先尝试 level
func (d *driver) Get() *event {
	if len(d.levelQueue) > 0 {
		return <-d.levelQueue
	}
	return <-d.eventQueue
}

func (d *driver) Put(e *event) {
	d.eventQueue <- e
}

func (d *driver) GetLength() int {
	return len(d.eventQueue) + len(d.levelQueue)
}

func (d *driver) recovery(e *event) {
	atomic.SwapInt32(&e.eventStatus, recoveryEventState)
	e.reset()
	d.poll <- e
}

// 挂起操作相关
func (d *driver) await(a awaitFunc) {
	d.waitQueue <- a
}
