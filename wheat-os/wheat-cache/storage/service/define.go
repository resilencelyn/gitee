package service

import (
	"sync"
)

var (
	oneSingleServer  sync.Once
	sysSingleService *singleService
)

const (
	timeOutDefault = 2
)
