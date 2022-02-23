package channelx

import (
	"sync/atomic"

	"gitee.com/wheat-os/wheatCache/pkg/structure"
)

type ChannelX struct {
	channel  chan *structure.Value
	sizeByte int64
}

func MakeChannelX(length int) structure.ChannelXInterface {
	return &ChannelX{
		channel:  make(chan *structure.Value, length),
		sizeByte: 0,
	}
}

func (c *ChannelX) SizeByte() int64 {
	return c.sizeByte
}

// RollBack TODO 事务相关, V2 实现
func (c *ChannelX) RollBack() error {
	panic("not implemented") // TODO: Implement
}

// Begin 事务相关, V2 实现
func (c *ChannelX) Begin() error {
	panic("not implemented") // TODO: Implement
}

// Comment 事务相关, V2 实现
func (c *ChannelX) Comment() error {
	panic("not implemented") // TODO: Implement
}

func (c *ChannelX) Encode() ([]byte, error) {
	panic("not implemented") // TODO: Implement
}

func (c *ChannelX) Push(value string) structure.UpdateLength {
	val := structure.NewValue(value)
	up := val.GetSize()
	c.channel <- val
	atomic.AddInt64(&c.sizeByte, int64(up))
	return structure.UpdateLength(up)
}

func (c *ChannelX) Pop() (string, structure.UpdateLength) {
	val := <-c.channel
	return val.ToString(), structure.UpdateLength(val.GetSize()) * -1
}

func (c *ChannelX) Length() int {
	return len(c.channel)
}

func (c *ChannelX) Clean() structure.UpdateLength {
	c.channel = make(chan *structure.Value, cap(c.channel))
	up := c.sizeByte
	c.sizeByte = 0
	return structure.UpdateLength(up) * -1
}
