package proxy

import (
	"context"
	"io"

	wheatCodec "gitee.com/wheat-os/wheatCache/gateway/codec"
	"gitee.com/wheat-os/wheatCache/gateway/endpoint"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

// TransparentHandler returns a handler that attempts to proxy all requests that are not registered in the server.
// The indented use here is as a transparent proxy, where the server doesn't know about the services implemented by the
// backends. It should be used as a `grpc.UnknownServiceHandler`.
//
// This can *only* be used if the `server` also uses grpcproxy.CodecForServer() ServerOption.
func TransparentHandler(director StreamDirector, endpoint endpoint.EndpointInterface) grpc.StreamHandler {
	streamer := &handler{
		director,
		endpoint,
	}
	return streamer.handler
}

type handler struct {
	director StreamDirector
	endpoint endpoint.EndpointInterface
}

// handler is where the real magic of proxying happens.
// It is invoked like any gRPC server stream and uses the gRPC server framing to get and receive bytes from the wire,
// forwarding it to a ClientStream established against the relevant ClientConn.
func (s *handler) handler(srv interface{}, serverStream grpc.ServerStream) error {
	fullMethodName, ok := grpc.MethodFromServerStream(serverStream)
	if !ok {
		return status.Errorf(codes.Internal, "lowLevelServerStream not exists in context")
	}

	outgoingCtx, backendConn, err := s.director(serverStream.Context(), fullMethodName, s.endpoint)
	if err != nil {
		return err
	}

	clientCtx, clientCancel := context.WithCancel(outgoingCtx)
	defer clientCancel()

	clientStream, err := grpc.NewClientStream(clientCtx, clientStreamDescForProxying, backendConn, fullMethodName)
	if err != nil {
		return err
	}

	s2cErrChan := s.forwardServerToClient(serverStream, clientStream)
	c2sErrChan := s.forwardClientToServer(clientStream, serverStream)

	for i := 0; i < 2; i++ {
		select {
		case s2cErr := <-s2cErrChan:
			if s2cErr == io.EOF {
				// 客户端流发送完毕正常关闭结束， Proxy 关闭对 Backend 的连接
				clientStream.CloseSend()
				break
			}

			clientCancel()
			return status.Errorf(codes.Internal, "failed proxying s2c: %v", s2cErr)
		case c2sErr := <-c2sErrChan:
			// 服务的没用在提供数据触发这个分支
			serverStream.SetTrailer(clientStream.Trailer())

			if c2sErr != io.EOF {
				return c2sErr
			}

			return nil
		}
	}

	return status.Errorf(codes.Internal, "gRPC proxying should never reach this stage.")
}

func (s *handler) forwardClientToServer(src grpc.ClientStream, dst grpc.ServerStream) chan error {
	ret := make(chan error, 1)
	go func() {
		f := &wheatCodec.Frame{}
		for i := 0; ; i++ {
			if err := src.RecvMsg(f); err != nil {
				ret <- err // this can be io.EOF which is happy case
				break
			}
			if i == 0 {
				// This is a bit of a hack, but client to server headers are only readable after first client msg is
				// received but must be written to server stream before the first msg is flushed.
				// This is the only place to do it nicely.
				md, err := src.Header()
				if err != nil {
					ret <- err
					break
				}
				if err := dst.SendHeader(md); err != nil {
					ret <- err
					break
				}
			}
			if err := dst.SendMsg(f); err != nil {
				ret <- err
				break
			}
		}
	}()
	return ret
}

func (s *handler) forwardServerToClient(src grpc.ServerStream, dst grpc.ClientStream) chan error {
	ret := make(chan error, 1)
	go func() {
		f := &wheatCodec.Frame{}
		for i := 0; ; i++ {
			if err := src.RecvMsg(f); err != nil {
				ret <- err // this can be io.EOF which is happy case
				break
			}
			if err := dst.SendMsg(f); err != nil {
				ret <- err
				break
			}
		}
	}()
	return ret
}
