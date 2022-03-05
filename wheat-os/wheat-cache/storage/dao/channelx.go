package dao

import (
	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/event2"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
	"gitee.com/wheat-os/wheatCache/pkg/structure/channelx"
)

func (d *Dao) CPush(key *proto.BaseKey, Value []string) (interface{}, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}
	chanVal, ok := val.(structure.ChannelXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("channelx")
	}

	return event2.EventAwaitFunc(func() (interface{}, error) {
		var sumUp structure.UpdateLength
		for _, v := range Value {
			sumUp += chanVal.Push(v)
		}

		d.lru.UpdateLruSize(sumUp)

		return &proto.CPushResponse{}, nil
	}), nil

}

func (d *Dao) CPop(key *proto.BaseKey, count int32) (interface{}, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}
	chanVal, ok := val.(structure.ChannelXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("channelx")
	}

	return event2.EventAwaitFunc(func() (interface{}, error) {
		var sumUp structure.UpdateLength
		result := make([]string, 0, count)
		for i := int32(0); i < count; i++ {
			v, up := chanVal.Pop()
			sumUp += up
			result = append(result, v)
		}

		d.lru.UpdateLruSize(sumUp)
		return &proto.CPopResponse{Result: result}, nil
	}), nil
}

func (d *Dao) CMake(key *proto.BaseKey, length int32) (*proto.CMakeResponse, error) {
	chanVal := channelx.MakeChannelX(int(length))
	err := d.lru.Add(key, chanVal)
	if err != nil {
		return nil, err
	}

	return &proto.CMakeResponse{}, nil
}

func (d *Dao) CLen(key *proto.BaseKey) (*proto.CLenResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}
	chanVal, ok := val.(structure.ChannelXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("channelx")
	}

	return &proto.CLenResponse{Length: int32(chanVal.Length())}, nil
}

func (d *Dao) CClean(key *proto.BaseKey) (*proto.CCleanResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}
	chanVal, ok := val.(structure.ChannelXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("channelx")
	}
	up := chanVal.Clean()
	d.lru.UpdateLruSize(up)

	return &proto.CCleanResponse{}, nil
}
