package dao

import (
	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
	"gitee.com/wheat-os/wheatCache/pkg/structure/listx"
)

func (d *Dao) LIndex(key *proto.BaseKey, index int32) (*proto.LIndexResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	result, err := listVal.Index(int(index))
	if err != nil {
		return nil, err
	}

	return &proto.LIndexResponse{Result: result}, nil
}

func (d *Dao) LLen(key *proto.BaseKey) (*proto.LLenResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	return &proto.LLenResponse{Length: int32(listVal.Length())}, nil
}

func (d *Dao) LPop(key *proto.BaseKey, count int32) (*proto.LPopResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	result, upLen := listVal.LPop(int(count))

	d.lru.UpdateLruSize(upLen)

	return &proto.LPopResponse{Results: result}, nil
}

func (d *Dao) LPush(key *proto.BaseKey, values []string) (*proto.LPushResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		list := listx.NewListXSingle()
		list.LPush(values...)
		err := d.lru.Add(key, list)
		return &proto.LPushResponse{}, err
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	upLen := listVal.LPush(values...)
	d.lru.UpdateLruSize(upLen)

	return &proto.LPushResponse{}, nil
}

func (d *Dao) LPushX(key *proto.BaseKey, values []string) (*proto.LPushXResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	upLen := listVal.LPush(values...)
	d.lru.UpdateLruSize(upLen)

	return &proto.LPushXResponse{}, nil
}

func (d *Dao) LRange(key *proto.BaseKey, start, end int32) (*proto.LRangeResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	res, err := listVal.Range(int(start), int(end))
	if err != nil {
		return nil, err
	}
	return &proto.LRangeResponse{Values: res}, nil
}

func (d *Dao) LRem(key *proto.BaseKey, count int32, value string) (*proto.LRemResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	remCount, upLen := listVal.Remove(value, int(count))
	d.lru.UpdateLruSize(upLen)

	return &proto.LRemResponse{Count: int32(remCount)}, nil
}

func (d *Dao) LSet(key *proto.BaseKey, index int32, value string) (*proto.LSetResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	upLen, err := listVal.Insert(int(index), false, value)
	d.lru.UpdateLruSize(upLen)
	return &proto.LSetResponse{}, err
}

func (d *Dao) RPop(key *proto.BaseKey, count int32) (*proto.RPopResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	result, upLen := listVal.RPop(int(count))

	d.lru.UpdateLruSize(upLen)

	return &proto.RPopResponse{Result: result}, nil
}

func (d *Dao) LTrim(key *proto.BaseKey, start, end int32) (*proto.LTrimResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	upLen, err := listVal.Slice(int(start), int(end))
	if err != nil {
		return nil, err
	}
	d.lru.UpdateLruSize(upLen)
	return &proto.LTrimResponse{}, nil
}

func (d *Dao) RPush(key *proto.BaseKey, values []string) (*proto.RPushResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		list := listx.NewListXSingle()
		list.RPush(values...)
		return &proto.RPushResponse{}, d.lru.Add(key, list)
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	upLen := listVal.RPush(values...)
	d.lru.UpdateLruSize(upLen)

	return &proto.RPushResponse{}, nil
}

func (d *Dao) RPushX(key *proto.BaseKey, values []string) (*proto.RPushXResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.KeyBaseIsNilErr()
	}

	listVal, ok := val.(structure.ListXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("listx")
	}

	upLen := listVal.RPush(values...)
	d.lru.UpdateLruSize(upLen)

	return &proto.RPushXResponse{}, nil
}
