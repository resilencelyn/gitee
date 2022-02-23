package external

import (
	"context"
	"testing"

	_ "gitee.com/wheat-os/wheatCache/conf"
	mockClient "gitee.com/wheat-os/wheatCache/mock/storage"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"github.com/golang/mock/gomock"
	"github.com/stretchr/testify/require"
)

func TestNewGatewayClient(t *testing.T) {
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()
	GateWayCtrl = ctrl

	cli, err := NewGatewayClient()
	require.NoError(t, err)
	mockClient := cli.(*mockClient.MockCommServerClient)
	ctx := context.Background()

	mockClient.EXPECT().Get(ctx, gomock.Any()).Return(&proto.GetResponse{
		Result: "mockData",
	}, nil)

	resp, err := mockClient.Get(ctx, nil)
	require.NoError(t, err)
	require.Equal(t, resp.Result, "mockData")

	resp, err = mockClient.Get(ctx, nil)
	require.NoError(t, err)
	require.Equal(t, resp.Result, "mockData")

}
