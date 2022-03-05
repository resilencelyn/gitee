package event

import (
	"sync/atomic"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
)

// 事件 poll 降低 new 对象的频率
type eventPoll struct {
	poll    chan *event
	maxSize int32
	nowSize *int32
}

func (e *eventPoll) getEvent() *event {
	issSize := atomic.LoadInt32(e.nowSize)
	if issSize < e.maxSize {
		atomic.AddInt32(e.nowSize, 1)
		return newEvent()
	}

	return <-e.poll
}

func (e *eventPoll) recovery(rEvent *event) {
	rEvent.Reset()
	e.poll <- rEvent
}

func newEventPoll(maxSize int) *eventPoll {
	return &eventPoll{
		poll:    make(chan *event, maxSize),
		maxSize: int32(maxSize),
		nowSize: new(int32),
	}
}

type event struct {
	msgCtx      map[string]interface{}
	eventName   string
	msg         map[string]string // 消息
	waitResult  chan interface{}  // 等待返回
	err         error
	eventStatus *int32
	ttlManage   *time.Timer
}

func newEvent() *event {
	status := defaultEventState
	return &event{
		eventStatus: &status,
	}
}

func (e *event) Reset() {
	if e.ttlManage != nil {
		e.ttlManage.Stop()
	}

	e.err = nil

	atomic.SwapInt32(e.eventStatus, defaultEventState)
}

func (e *event) SetMsg(key string, val string) {
	if e.msg == nil {
		e.msg = make(map[string]string)
	}
	e.msg[key] = val
}

func (e *event) GetMsg(key string) string {
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
	val, ok := e.msgCtx[key]
	return val, ok
}

// InitWaitEvent 初始化 wait event 必须调用才拥有等待特性
func (e *event) InitWaitEvent() {
	if e.waitResult == nil || len(e.waitResult) > 0 {
		e.waitResult = make(chan interface{})
	}

	// 清理残留
	if e.ttlManage == nil {
		e.ttlManage = time.NewTimer(0)
	}
	e.ttlManage.Stop()
	if len(e.ttlManage.C) > 0 {
		<-e.ttlManage.C
	}

	atomic.CompareAndSwapInt32(e.eventStatus, defaultEventState, waitEventState)
}

// StartWaitEvent 开始一个等待任务
func (e *event) StartWaitEvent(ttl time.Duration) (interface{}, error) {
	e.ttlManage.Reset(ttl)

	for {
		select {
		case <-e.ttlManage.C:
			if atomic.CompareAndSwapInt32(e.eventStatus, waitEventState, closeEventState) {
				return nil, errorx.TimeOutErr()
			}
			continue

		case result := <-e.waitResult:
			atomic.CompareAndSwapInt32(e.eventStatus, workEventState, closeEventState)
			return result, e.err
		}
	}
}

func (e *event) ExecWorkAndSendResult(work EventWorkFunc) (interface{}, error) {
	if !atomic.CompareAndSwapInt32(e.eventStatus, waitEventState, workEventState) {
		return nil, errorx.New("not wait status, exec err")
	}

	res, err := work()
	e.err = err
	e.waitResult <- res
	return res, err
}

func (e *event) SetResultErr(err error) {
	if !atomic.CompareAndSwapInt32(e.eventStatus, waitEventState, workEventState) {
		return
	}

	e.err = err
	e.waitResult <- nil
}

type Driver struct {
	maxQueueSize int
	queue        chan *event
	poll         *eventPoll
}

// Get 获取驱动
func (d *Driver) Get() *event {
	return <-d.queue
}

func (d *Driver) Put(event *event) {
	d.queue <- event
}

func (d *Driver) GetLength() int {
	return len(d.queue)
}

func (d *Driver) NewEvent(name string) *event {
	event := d.poll.getEvent()
	event.eventName = name
	return event
}

//  任何时候回收事件都应该由 最后使用者回收
func (d *Driver) Recovery(e *event) {
	d.poll.recovery(e)
}

// NewDriver 新建 Driver
func NewDriver(maxSize int) DriverInterface {
	return &Driver{
		maxQueueSize: maxSize,
		queue:        make(chan *event, maxSize),
		poll:         newEventPoll(maxSize),
	}
}
