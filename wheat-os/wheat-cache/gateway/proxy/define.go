package proxy

import (
	"context"

	"gitee.com/wheat-os/wheatCache/gateway/endpoint"
	"google.golang.org/grpc"
)

type StreamDirector func(ctx context.Context, fullMethodName string, endpoint endpoint.EndpointInterface) (context.Context, *grpc.ClientConn, error)

var (
	clientStreamDescForProxying = &grpc.StreamDesc{
		ServerStreams: true,
		ClientStreams: true,
	}
)
