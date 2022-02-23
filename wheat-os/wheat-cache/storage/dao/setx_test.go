package dao

import (
	"context"
	"testing"

	mockproto "gitee.com/wheat-os/wheatCache/mock/storage"
	"gitee.com/wheat-os/wheatCache/pkg/event2"
	"gitee.com/wheat-os/wheatCache/pkg/lru"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/storage/external"
	"github.com/golang/mock/gomock"
	"github.com/stretchr/testify/require"
)

func TestDao_SAdd(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")

	_, err := dao.SAdd(baseKey, []string{"1", "2", "3"})
	require.NoError(t, err)

	res, err := dao.SPop(baseKey, 3)
	require.NoError(t, err)
	t.Log(res.Members)
	require.Len(t, res.Members, 3)
}

func IsAdd(t *testing.T, baseKey *proto.BaseKey, dao Interface) {
	_, err := dao.SAdd(baseKey, []string{"1", "2", "3", "4", "4"})
	require.NoError(t, err)
}

func TestDao_SCard(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	IsAdd(t, baseKey, dao)

	res, err := dao.SCard(baseKey)
	require.NoError(t, err)
	require.Equal(t, res.Length, int32(4))
}

func mockSScan(t *testing.T, ctrl *gomock.Controller) {
	external.GateWayCtrl = ctrl
	client, err := external.NewGatewayClient()
	require.NoError(t, err)
	mockCli, ok := client.(*mockproto.MockCommServerClient)
	require.True(t, ok)

	gomock.InOrder(
		mockCli.EXPECT().SScan(gomock.Any(), gomock.Any()).Return(
			&proto.SScanResponse{
				Results: []string{"a", "b", "c"},
			}, nil).MaxTimes(1),

		mockCli.EXPECT().SScan(gomock.Any(), gomock.Any()).Return(
			&proto.SScanResponse{
				Results: []string{"1", "2", "3"},
			}, nil).AnyTimes(),

		mockCli.EXPECT().SScan(gomock.Any(), gomock.Any()).Return(
			&proto.SScanResponse{
				Results: []string{"1", "2", "4"},
			}, nil).AnyTimes(),

		mockCli.EXPECT().SScan(gomock.Any(), gomock.Any()).Return(
			&proto.SScanResponse{
				Results: []string{"99", "6", "1.20"},
			}, nil).AnyTimes(),
	)

}

func mockSAdd(t *testing.T, ctrl *gomock.Controller, dao Interface) {
	external.GateWayCtrl = ctrl
	client, err := external.NewGatewayClient()
	require.NoError(t, err)
	mockCli, ok := client.(*mockproto.MockCommServerClient)
	require.True(t, ok)

	execFunc := func(
		ctx context.Context,
		req *proto.SAddRequest,
	) (*proto.SAddResponse, error) {
		return dao.SAdd(req.Key, req.Member)
	}

	mockCli.EXPECT().SAdd(gomock.Any(), gomock.Any()).DoAndReturn(execFunc)
}

func TestDao_SDiff(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("diff")

	_, err := dao.SAdd(baseKey, []string{"a", "b", "s"})
	require.NoError(t, err)

	//  mock client
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()
	mockSScan(t, ctrl)

	wait, err := dao.SDiff(baseKey, []string{"opp"})
	require.NoError(t, err)
	waitWork, ok := wait.(event2.EventAwaitFunc)
	require.True(t, ok)

	resp, err := waitWork()
	require.NoError(t, err)

	res, ok := resp.(*proto.SDiffResponse)
	require.True(t, ok)

	require.Equal(t, res.Result, []string{"c"})

}

func TestDao_SDiffStore(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("diffStore")

	storeKey := proto.NewBaseKey("apple")

	_, err := dao.SAdd(baseKey, []string{"a", "b", "s"})
	require.NoError(t, err)

	//  mock client
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()
	mockSScan(t, ctrl)
	// 必须调用一次 storage
	mockSAdd(t, ctrl, dao)

	wait, err := dao.SDiffStore(baseKey, []string{"opp", "oo2"}, storeKey.Key)
	require.NoError(t, err)
	waitWork, ok := wait.(event2.EventAwaitFunc)
	require.True(t, ok)

	resp, err := waitWork()
	require.NoError(t, err)

	_, ok = resp.(*proto.SDiffStoreResponse)
	require.True(t, ok)
	res, err := dao.SScan(storeKey, 0, "", -1)
	require.NoError(t, err)
	t.Log(res.Results)

	require.Len(t, res.Results, 4)

}

func TestDao_SInter(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("inter")

	_, err := dao.SAdd(baseKey, []string{"a", "b", "s"})
	require.NoError(t, err)

	//  mock client
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()
	mockSScan(t, ctrl)

	wait, err := dao.SInter(baseKey, []string{"opp"})
	require.NoError(t, err)
	waitWork, ok := wait.(event2.EventAwaitFunc)
	require.True(t, ok)

	resp, err := waitWork()
	require.NoError(t, err)

	res, ok := resp.(*proto.SInterResponse)
	require.True(t, ok)

	require.Equal(t, res.Result, []string{"a", "b"})
}

func TestDao_SInterStore(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("interStore")

	storeKey := proto.NewBaseKey("apple")

	_, err := dao.SAdd(baseKey, []string{"a", "b", "s"})
	require.NoError(t, err)

	//  mock client
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()
	mockSScan(t, ctrl)
	// 必须调用一次 storage
	mockSAdd(t, ctrl, dao)

	wait, err := dao.SInterStore(baseKey, []string{"opp", "oo2"}, storeKey.Key)
	require.NoError(t, err)
	waitWork, ok := wait.(event2.EventAwaitFunc)
	require.True(t, ok)

	resp, err := waitWork()
	require.NoError(t, err)

	_, ok = resp.(*proto.SInterStoreResponse)
	require.True(t, ok)
	res, err := dao.SScan(storeKey, 0, "", -1)
	require.NoError(t, err)
	t.Log(res.Results)

	require.Len(t, res.Results, 2)
}

func TestDao_SIsMember(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	testKey := proto.NewBaseKey("testKey")
	IsAdd(t, testKey, dao)

	resp, err := dao.SIsMember(testKey, "1")
	require.NoError(t, err)
	require.Equal(t, resp.Exist, true)

	resp, err = dao.SIsMember(testKey, "6")
	require.NoError(t, err)
	require.Equal(t, resp.Exist, false)
}

func TestDao_SMove(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	testKey := proto.NewBaseKey("testKey")

	moveKey := proto.NewBaseKey("moveKey")

	//  mock client
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()
	mockSAdd(t, ctrl, dao)

	_, err := dao.SAdd(testKey, []string{"1", "2", "3", "4"})
	require.NoError(t, err)

	waitWork, err := dao.SMove(testKey, moveKey.Key, []string{"3", "4", "5"})
	require.NoError(t, err)
	_, err = waitWork.(event2.EventAwaitFunc)()
	require.NoError(t, err)

	res, err := dao.SScan(testKey, 0, "", -1)
	require.NoError(t, err)
	require.Equal(t, res.Results, []string{"1", "2"})

	// 移动 key
	res, err = dao.SScan(moveKey, 0, "", -1)
	require.NoError(t, err)
	require.Equal(t, res.Results, []string{"3", "4"})

}

func TestDao_SPop(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	testKey := proto.NewBaseKey("testKey")

	_, err := dao.SAdd(testKey, []string{"1", "2", "3", "4"})
	require.NoError(t, err)

	resp, err := dao.SPop(testKey, 2)
	require.NoError(t, err)
	require.Len(t, resp.Members, 2)

	res, err := dao.SScan(testKey, 0, "", -1)
	require.NoError(t, err)
	require.Len(t, res.Results, 2)
}

func TestDao_SRem(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	testKey := proto.NewBaseKey("testKey")

	_, err := dao.SAdd(testKey, []string{"1", "2", "3", "4"})
	require.NoError(t, err)

	_, err = dao.SRem(testKey, 2)
	require.NoError(t, err)

	res, err := dao.SScan(testKey, 0, "", -1)
	require.NoError(t, err)
	require.Len(t, res.Results, 2)
}

func TestDao_SUnion(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("union")

	_, err := dao.SAdd(baseKey, []string{"a", "b", "s"})
	require.NoError(t, err)

	//  mock client
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()
	mockSScan(t, ctrl)

	wait, err := dao.SUnion(baseKey, []string{"opp"})
	require.NoError(t, err)
	waitWork, ok := wait.(event2.EventAwaitFunc)
	require.True(t, ok)

	resp, err := waitWork()
	require.NoError(t, err)

	res, ok := resp.(*proto.SUnionResponse)
	require.True(t, ok)

	require.Equal(t, res.Result, []string{"a", "b", "s", "c"})
}

func TestDao_SUnionStore(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("unionStore")

	storeKey := proto.NewBaseKey("apple")

	_, err := dao.SAdd(baseKey, []string{"a", "b", "s"})
	require.NoError(t, err)

	//  mock client
	ctrl := gomock.NewController(t)
	defer ctrl.Finish()
	mockSScan(t, ctrl)
	// 必须调用一次 storage
	mockSAdd(t, ctrl, dao)

	wait, err := dao.SUnionStore(baseKey, []string{"opp", "oo2"}, storeKey.Key)
	require.NoError(t, err)
	waitWork, ok := wait.(event2.EventAwaitFunc)
	require.True(t, ok)

	resp, err := waitWork()
	require.NoError(t, err)

	_, ok = resp.(*proto.SUnionStoreResponse)
	require.True(t, ok)
	res, err := dao.SScan(storeKey, 0, "", -1)
	require.NoError(t, err)
	t.Log(res.Results)

	require.Len(t, res.Results, 7)
}

func TestDao_SScan(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")

	_, err := dao.SAdd(baseKey, []string{"1", "2", "3", "a", "b", "c"})
	require.NoError(t, err)

	scanResp, err := dao.SScan(baseKey, 0, "", -1)
	require.NoError(t, err)
	require.Len(t, scanResp.Results, 6)

	scanResp, err = dao.SScan(baseKey, 0, `\d`, -1)
	require.NoError(t, err)
	require.Len(t, scanResp.Results, 3)

	scanResp, err = dao.SScan(baseKey, 0, `\d`, 4)
	require.NoError(t, err)
	require.Len(t, scanResp.Results, 3)

	scanResp, err = dao.SScan(baseKey, 0, `\d`, 2)
	require.NoError(t, err)
	require.Len(t, scanResp.Results, 2)

	scanResp, err = dao.SScan(baseKey, 0, `[a-z]+`, -1)
	require.NoError(t, err)
	require.Len(t, scanResp.Results, 3)

}
