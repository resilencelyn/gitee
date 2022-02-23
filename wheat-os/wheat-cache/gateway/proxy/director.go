package proxy

import (
	"context"

	"gitee.com/wheat-os/wheatCache/gateway/codec"
	"gitee.com/wheat-os/wheatCache/gateway/endpoint"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/metadata"
	"google.golang.org/grpc/status"
)

func GetDirectorByServiceHash() StreamDirector {
	return func(ctx context.Context, fullMethodName string, endpoint endpoint.EndpointInterface) (context.Context, *grpc.ClientConn, error) {

		md, ok := metadata.FromIncomingContext(ctx)
		if !ok {
			return nil, nil, status.Errorf(codes.Unknown, "from FromIncomingContext err")
		}

		baseKey, ok := md[proto.BaseKeyMethodKey]
		if !ok {
			return nil, nil, status.Errorf(codes.Unknown,
				"grpc header is not found %s, please check the client interceptor", proto.BaseKeyMethodKey)
		}

		target, err := endpoint.GetTargetAddr(baseKey...)
		if err != nil {
			return nil, nil, status.Errorf(codes.Unknown, "get transport err, err:%v", err)
		}

		cli, err := grpc.DialContext(ctx, target, grpc.WithInsecure(), grpc.WithDefaultCallOptions(grpc.ForceCodec(codec.Codec())))
		return ctx, cli, err
	}
}
