package external

import (
	"sync"

	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"github.com/golang/mock/gomock"
)

var (
	oneGatewayClient sync.Once
	gatewayClient    proto.CommServerClient
)

var (
	GateWayCtrl          *gomock.Controller
	mockGatewayClient    proto.CommServerClient
	oneMockGatewayClient sync.Once
)
