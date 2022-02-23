package middle

import (
	"context"

	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/metadata"
	"google.golang.org/grpc/status"
)

func WithUnaryColonyClient(ctx context.Context, method string, req, reply interface{}, header map[string]string) error {
	key, ok := req.(proto.GetKeyBaseInterface)
	if !ok {
		return status.Errorf(codes.Unknown, "key base err")
	}
	if header == nil {
		return nil
	}
	// meta 解析会出现 全部小写问题
	header[proto.BaseKeyMethodKey] = key.GetKey().Key
	return nil
}

func getKeyByKeyMapvalue(m map[string]string) []string {
	l := make([]string, 0)
	for key, value := range m {
		l = append(l, key, value)
	}
	return l
}

func GetUnaryInterceptor(middleOpts ...ClientMiddle) grpc.UnaryClientInterceptor {
	return func(ctx context.Context, method string, req, reply interface{},
		cc *grpc.ClientConn, invoker grpc.UnaryInvoker, opts ...grpc.CallOption) error {
		// 加载中间件
		header := make(map[string]string)
		for _, mid := range middleOpts {
			err := mid(ctx, method, req, reply, header)
			if err != nil {
				return err
			}
		}

		lm := getKeyByKeyMapvalue(header)

		headerData := metadata.Pairs(lm...)
		ctxH := metadata.NewOutgoingContext(ctx, headerData)

		return invoker(ctxH, method, req, reply, cc, opts...)
	}
}
