package event

import "context"

type Consumer struct {
	driver DriverInterface
}

func (c *Consumer) Receive(ctx context.Context) *event {
	return c.driver.Get()
}

func (c *Consumer) NewEvent(name string) *event {
	return c.driver.NewEvent(name)
}

func (c *Consumer) Recovery(e *event) {
	c.driver.Recovery(e)
}

func NewConsumer(driver DriverInterface) ConsumerInterface {
	return &Consumer{
		driver: driver,
	}
}
