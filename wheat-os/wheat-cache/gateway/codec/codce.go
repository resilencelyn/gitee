package codec

import (
	"google.golang.org/grpc/encoding"
	"google.golang.org/protobuf/proto"
)

// protoCodec 用于 gateway 解析全部的 grpc 类型的消息
type protoCodec struct{}

func (protoCodec) Name() string {
	return "wheat-cache-proto"
}

func (protoCodec) Marshal(v interface{}) ([]byte, error) {
	return proto.Marshal(v.(proto.Message))
}

func (protoCodec) Unmarshal(data []byte, v interface{}) error {
	return proto.Unmarshal(data, v.(proto.Message))
}

type Frame struct {
	payload []byte
}

type proxyCodec struct {
	baseCodec encoding.Codec
}

func (p *proxyCodec) Name() string {
	return "wheat-cache-proxy"
}

func (p *proxyCodec) Marshal(v interface{}) ([]byte, error) {
	out, ok := v.(*Frame)
	if !ok {
		return p.Marshal(v)
	}
	return out.payload, nil
}

func (p *proxyCodec) Unmarshal(data []byte, v interface{}) error {
	dst, ok := v.(*Frame)
	if !ok {
		return p.Unmarshal(data, v)
	}

	dst.payload = data
	return nil
}

// CodeWithParent 生成基于 proto 的解码器
func CodeWithParent(parent encoding.Codec) encoding.Codec {
	return &proxyCodec{parent}
}

func Codec() encoding.Codec {
	return CodeWithParent(protoCodec{})
}
