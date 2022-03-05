package client

import (
	"context"
	"testing"

	"gitee.com/wheat-os/wheatCache/client/middle"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"github.com/stretchr/testify/require"
)

func TestClient(t *testing.T) {
	cli, err := NewWheatClient("127.0.0.1:5891", middle.WithUnaryColonyClient)
	require.NoError(t, err)
	ctx := context.Background()

	bKey := proto.NewBaseKey("apple")
	resp, err := cli.Set(ctx, &proto.SetRequest{
		Key: bKey,
		Val: "yyyy",
	})

	require.NoError(t, err)
	require.Equal(t, resp.Result, "yyyy")

	getResp, err := cli.Get(ctx, &proto.GetRequest{
		Key: bKey,
	})
	require.NoError(t, err)
	require.Equal(t, getResp.Result, "yyyy")
}

func TestClientGet(t *testing.T) {
	cli, err := NewWheatClient("127.0.0.1:5891", middle.WithUnaryColonyClient)
	require.NoError(t, err)
	ctx := context.Background()

	bKey := &proto.BaseKey{
		Key: "apple",
	}

	getResp, err := cli.Get(ctx, &proto.GetRequest{
		Key: bKey,
	})
	require.NoError(t, err)
	require.Equal(t, getResp.Result, "yyyy")
}
