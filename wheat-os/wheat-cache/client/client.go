package client

import (
	"gitee.com/wheat-os/wheatCache/client/middle"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"google.golang.org/grpc"
)

func newWheatClient(targer string, opt ...middle.ClientMiddle) (proto.CommServerClient, error) {

	interceptor := middle.GetUnaryInterceptor(opt...)
	comm, err := grpc.Dial(targer, grpc.WithInsecure(), grpc.WithUnaryInterceptor(interceptor))
	if err != nil {
		return nil, err
	}

	return proto.NewCommServerClient(comm), nil
}

func NewWheatClient(targer string, opt ...middle.ClientMiddle) (proto.CommServerClient, error) {
	return newWheatClient(targer, opt...)
}
