package external

import (
	"errors"

	"gitee.com/wheat-os/wheatCache/client"
	"gitee.com/wheat-os/wheatCache/client/middle"
	_ "gitee.com/wheat-os/wheatCache/conf"
	mockClient "gitee.com/wheat-os/wheatCache/mock/storage"
	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"github.com/spf13/viper"
)

func NewGatewayClient() (proto.CommServerClient, error) {

	if viper.GetString("env") == "dev" {
		if GateWayCtrl == nil {
			return nil, errorx.New("mock ctrl not init")
		}

		oneMockGatewayClient.Do(func() {
			mockGatewayClient = mockClient.NewMockCommServerClient(GateWayCtrl)
		})

		return mockGatewayClient, nil
	}

	oneGatewayClient.Do(func() {
		cli, err := client.NewWheatClient("127.0.0.1:5891", middle.WithUnaryColonyClient)
		if err == nil {
			gatewayClient = cli
		}
	})

	if gatewayClient != nil {
		return gatewayClient, nil
	}

	return nil, errors.New("get gateway err")
}
