package middle

import "context"

// type UnaryClientInterceptor func(ctx context.Context, method string, req, reply interface{}, cc *ClientConn, invoker UnaryInvoker, opts ...CallOption) error
type ClientMiddle func(ctx context.Context, method string, req, reply interface{}, header map[string]string) error
