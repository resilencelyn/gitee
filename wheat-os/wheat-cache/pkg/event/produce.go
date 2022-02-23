package event

import "context"

type Produce struct {
	driver DriverInterface
}

func (p *Produce) NewEvent(name string) *event {
	return p.driver.NewEvent(name)
}

func (p *Produce) Recovery(e *event) {
	p.driver.Recovery(e)
}

func (p *Produce) Call(ctx context.Context, e *event) {
	p.driver.Put(e)
}

func NewProduce(driver DriverInterface) ProduceInterface {
	return &Produce{
		driver: driver,
	}
}
