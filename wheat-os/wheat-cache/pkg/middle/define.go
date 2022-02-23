package middle

import (
	"sync"
)

var (
	oneMiddle        sync.Once
	middleWareDriver *MiddleWare
)

const (
	defaultConsumerCount = 5
	defaultDriverCount   = 1000
)
