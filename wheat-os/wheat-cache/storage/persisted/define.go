package persisted

import (
	"sync"

	protobuf "google.golang.org/protobuf/proto"
)

type AOFCodec interface {
	Encode(method string, m protobuf.Message) ([]byte, error)
	Decode(buf []byte) (protobuf.Message, error)
}

var (
	oneAOF sync.Once
	sysAOF *AOF
)

const (
	defaultByteIONum = 20000
)
