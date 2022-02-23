package dao

import (
	"context"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/event2"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
	"gitee.com/wheat-os/wheatCache/pkg/structure/hashx"
	"gitee.com/wheat-os/wheatCache/storage/external"
)

func (d *Dao) SAdd(key *proto.BaseKey, setVal []string) (*proto.SAddResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		hashVal := hashx.NewHashXSingle()
		for _, sv := range setVal {
			hashVal.SetX(sv, sv)
		}
		d.lru.Add(key, hashVal)
		return &proto.SAddResponse{}, nil
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	for _, sv := range setVal {
		b, up := hashVal.SetX(sv, sv)
		if b {
			d.lru.UpdateLruSize(up)
		}
	}

	return &proto.SAddResponse{}, nil
}

func (d *Dao) SCard(key *proto.BaseKey) (*proto.SCardResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}
	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	return &proto.SCardResponse{Length: int32(hashVal.Length())}, nil
}

func mathSDiff(masterItem []string, extKey []string) ([]string, error) {
	cli, err := external.NewGatewayClient()
	if err != nil {
		return nil, err
	}

	m := make(map[string]struct{})
	for _, bVal := range masterItem {
		m[bVal] = struct{}{}
	}

	setItem := make([]string, 0, len(masterItem))
	ctx := context.Background()
	for _, sKey := range extKey {
		baseKey := proto.NewBaseKey(sKey)
		resp, err := cli.SScan(ctx, &proto.SScanRequest{
			Key:   baseKey,
			Count: -1,
		})

		if err != nil {
			continue
		}

		for _, item := range resp.Results {
			if _, ok := m[item]; !ok {
				setItem = append(setItem, item)
				m[item] = struct{}{}
			}
		}
	}

	return setItem, nil
}

func (d *Dao) SDiff(key *proto.BaseKey, setKey []string) (interface{}, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	baseItem := hashVal.Key()

	// await 挂起
	return event2.EventAwaitFunc(func() (interface{}, error) {
		setItem, err := mathSDiff(baseItem, setKey)
		if err != nil {
			return nil, err
		}

		return &proto.SDiffResponse{Result: setItem}, nil

	}), nil
}

func (d *Dao) SDiffStore(key *proto.BaseKey, setKey []string, saveKey string) (interface{}, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	baseItem := hashVal.Key()

	// await 挂起
	return event2.EventAwaitFunc(func() (interface{}, error) {
		setItem, err := mathSDiff(baseItem, setKey)
		if err != nil {
			return nil, err
		}

		cli, err := external.NewGatewayClient()
		if err != nil {
			return nil, err
		}

		ctx := context.Background()
		_, err = cli.SAdd(ctx, &proto.SAddRequest{
			Key:    proto.NewBaseKey(saveKey),
			Member: setItem,
		})

		if err != nil {
			return nil, err
		}

		return &proto.SDiffStoreResponse{}, nil

	}), nil
}

func mathSInter(masterItem []string, extKey []string) ([]string, error) {
	cli, err := external.NewGatewayClient()
	if err != nil {
		return nil, err
	}

	m := make(map[string]struct{})
	for _, bVal := range masterItem {
		m[bVal] = struct{}{}
	}

	setItem := make([]string, 0, len(masterItem))
	ctx := context.Background()
	for _, sKey := range extKey {

		resp, err := cli.SScan(ctx, &proto.SScanRequest{
			Key:   proto.NewBaseKey(sKey),
			Count: -1,
		})

		if err != nil {
			continue
		}

		for _, item := range resp.Results {
			if _, ok := m[item]; ok {
				setItem = append(setItem, item)
				delete(m, item)
			}
		}
	}

	return setItem, nil
}

func (d *Dao) SInter(key *proto.BaseKey, setKey []string) (interface{}, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	baseItem := hashVal.Key()

	// await 挂起
	return event2.EventAwaitFunc(func() (interface{}, error) {
		setItem, err := mathSInter(baseItem, setKey)
		if err != nil {
			return nil, err
		}

		return &proto.SInterResponse{Result: setItem}, nil

	}), nil
}

func (d *Dao) SInterStore(key *proto.BaseKey, setKey []string, saveKey string) (interface{}, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	baseItem := hashVal.Key()

	// await 挂起
	return event2.EventAwaitFunc(func() (interface{}, error) {
		setItem, err := mathSInter(baseItem, setKey)
		if err != nil {
			return nil, err
		}

		cli, err := external.NewGatewayClient()
		if err != nil {
			return nil, err
		}

		ctx := context.Background()
		_, err = cli.SAdd(ctx, &proto.SAddRequest{
			Key:    proto.NewBaseKey(saveKey),
			Member: setItem,
		})

		if err != nil {
			return nil, err
		}

		return &proto.SInterStoreResponse{}, nil
	}), nil
}

func (d *Dao) SIsMember(key *proto.BaseKey, member string) (*proto.SIsMemberResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	_, err := hashVal.Get(member)
	if err != nil {
		return &proto.SIsMemberResponse{Exist: false}, nil
	}

	return &proto.SIsMemberResponse{Exist: true}, nil
}

func (d *Dao) SMove(key *proto.BaseKey, moveKey string, members []string) (interface{}, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	moveMembers := make([]string, 0, len(members))
	for _, member := range members {
		up, err := hashVal.Del(member)
		if err == nil {
			d.lru.UpdateLruSize(up)
			moveMembers = append(moveMembers, member)
		}
	}

	return event2.EventAwaitFunc(func() (interface{}, error) {
		cli, err := external.NewGatewayClient()
		if err != nil {
			return nil, err
		}

		ctx := context.Background()
		_, err = cli.SAdd(ctx, &proto.SAddRequest{
			Key:    proto.NewBaseKey(moveKey),
			Member: moveMembers,
		})
		if err != nil {
			return nil, err
		}

		return &proto.SMoveResponse{}, nil
	}), nil

}

func (d *Dao) SPop(key *proto.BaseKey, count int32) (*proto.SPopResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	members := make([]string, 0, count)

	result := hashVal.Range(0, int(count), "")
	for _, res := range result {
		up, err := hashVal.Del(res)
		if err != nil {
			return nil, err
		}
		d.lru.UpdateLruSize(up)
		members = append(members, res)
	}

	return &proto.SPopResponse{Members: members}, nil
}

func (d *Dao) SRem(key *proto.BaseKey, count int32) (*proto.SRemResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	result := hashVal.Range(0, int(count), "")
	for _, res := range result {
		up, err := hashVal.Del(res)
		if err != nil {
			return nil, err
		}
		d.lru.UpdateLruSize(up)
	}

	return &proto.SRemResponse{}, nil
}

func mathSUnion(masterItem []string, extKey []string) ([]string, error) {
	cli, err := external.NewGatewayClient()
	if err != nil {
		return nil, err
	}

	m := make(map[string]struct{})
	for _, bVal := range masterItem {
		m[bVal] = struct{}{}
	}

	ctx := context.Background()
	for _, sKey := range extKey {

		resp, err := cli.SScan(ctx, &proto.SScanRequest{
			Key:   proto.NewBaseKey(sKey),
			Count: -1,
		})

		if err != nil {
			continue
		}

		for _, item := range resp.Results {
			if _, ok := m[item]; !ok {
				masterItem = append(masterItem, item)
				m[item] = struct{}{}
			}
		}
	}

	return masterItem, nil
}

func (d *Dao) SUnion(key *proto.BaseKey, setKey []string) (interface{}, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	baseItem := hashVal.Key()

	// await 挂起
	return event2.EventAwaitFunc(func() (interface{}, error) {
		setItem, err := mathSUnion(baseItem, setKey)
		if err != nil {
			return nil, err
		}

		return &proto.SUnionResponse{Result: setItem}, nil

	}), nil
}

func (d *Dao) SUnionStore(key *proto.BaseKey, setKey []string, saveKey string) (interface{}, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	baseItem := hashVal.Key()

	// await 挂起
	return event2.EventAwaitFunc(func() (interface{}, error) {
		setItem, err := mathSUnion(baseItem, setKey)
		if err != nil {
			return nil, err
		}

		cli, err := external.NewGatewayClient()
		if err != nil {
			return nil, err
		}

		ctx := context.Background()
		_, err = cli.SAdd(ctx, &proto.SAddRequest{
			Key:    proto.NewBaseKey(saveKey),
			Member: setItem,
		})

		if err != nil {
			return nil, err
		}

		return &proto.SUnionStoreResponse{}, nil
	}), nil
}

func (d *Dao) SScan(key *proto.BaseKey, cursor int32, regex string, count int32) (*proto.SScanResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("setx")
	}

	result := hashVal.Range(int(cursor), int(count), regex)
	return &proto.SScanResponse{Results: result}, nil
}
