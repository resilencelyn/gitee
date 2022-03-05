package lru

import (
	"sync"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/util/skiplist"
)

// lru 的 ttl 管理器
type lruTTl struct {
	sk        *skiplist.SkipList
	memoryKey chan string //  缓存过期的 key
	detachNum int         // 每次移除的数量
	mu        sync.Mutex
}

func (l *lruTTl) setKeys(key *proto.BaseKey) int64 {
	l.mu.Lock()
	defer l.mu.Unlock()

	// 永久存储
	if key.Expire == nil && key.Ttl == 0 {
		return 0
	}

	ttlTime := time.Now().Unix()
	if key.Expire != nil {
		ttlTime = key.Expire.GetSeconds()
	}

	ttlTime += key.GetTtl()
	l.sk.Insert(float64(ttlTime), key.GetKey())

	return ttlTime
}

// 加载过期的 Key 到 Memory
func (l *lruTTl) ttlKeyToMemoryBySecond() {
	t := time.Now()
	values := l.sk.PopLeft(float64(t.Unix()))

	for _, val := range values {
		l.memoryKey <- val.(string)
	}
}

func newLruTTl(detachNum int) *lruTTl {
	return &lruTTl{
		sk: skiplist.NewSkipList(defaultTtlMaxLevel),
		// 默认 10000 个 Key
		memoryKey: make(chan string, 10000),
		detachNum: detachNum,
	}
}
