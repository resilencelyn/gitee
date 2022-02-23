package middlemsg

import "time"

const LruCleanContextName = "lru-clean-context"

type LruCleanContext struct {
	Keys            []string
	BeforeCleanSize int64
	BehindCleanSize int64

	StartTime time.Time
	EndTime   time.Time
}

const LruTTlContextName = "lru-ttl-context"

type LruTTlContext struct {
	Keys      []string
	CleanTime time.Time
}
