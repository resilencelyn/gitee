package lru

import (
	"sync"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
)

type SingleWorkFunc func() interface{}

const (
	OptionEventName = "operateEvent"
	CleanEventName  = "clearEvent"
	TtlEventName    = "ttlEvent"
)

var (
	lruCacheOnce sync.Once
	lruCache     *SingleCache
)

const (
	defaultLruMaxSize     = 1 * 1024 * 1024 * 1024
	defaultLruClearSize   = 0.5 * 1024 * 1024 * 1024
	defaultLruEventDriver = 2000
)
const (
	defaultWaitTime = 20 * time.Minute
)

type CacheInterface interface {
	Del() error
	Get(key *proto.BaseKey) (structure.KeyBaseInterface, bool)
	Add(key *proto.BaseKey, val structure.KeyBaseInterface) error
	UpdateLruSize(length structure.UpdateLength)
	DelByKey(key *proto.BaseKey) error
	DelToClearSize() error
}

// TTL
const (
	defaultDetachNum   = 300
	defaultTtlMaxLevel = 18
)
