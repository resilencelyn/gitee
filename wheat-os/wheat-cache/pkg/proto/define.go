package proto

import (
	"google.golang.org/protobuf/types/known/timestamppb"
)

type GetKeyBaseInterface interface {
	GetKey() *BaseKey
}

const (
	BaseKeyMethodKey = "basekey"
)

// NewBaseKey
// key，ttl，expire
func NewBaseKey(key string, t ...int64) *BaseKey {
	var expire *timestamppb.Timestamp = nil
	var ttl int64

	if len(t) > 1 {
		expire = &timestamppb.Timestamp{
			Seconds: t[1],
		}
		ttl = t[0]
	} else if len(t) == 1 {
		ttl = t[0]
	}

	return &BaseKey{
		Key:    key,
		Expire: expire,
		Ttl:    ttl,
	}
}
