package dao

import (
	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
	"gitee.com/wheat-os/wheatCache/pkg/structure/stringx"
)

// stringx 相关的方法
func (d *Dao) Set(key *proto.BaseKey, strVal string) (*proto.SetResponse, error) {
	value, ok := d.lru.Get(key)
	if ok {
		if val, ok := value.(structure.StringXInterface); ok {
			res, length := val.Set(strVal)
			d.lru.UpdateLruSize(length)
			return &proto.SetResponse{Result: res}, nil
		} else {
			return nil, errorx.New("the key:%s is not stringx type", key)
		}
	}

	// 不存在新建
	strValue := stringx.NewStringSingle()
	result, _ := strValue.Set(strVal)
	err := d.lru.Add(key, strValue)
	if err != nil {
		return nil, err
	}
	return &proto.SetResponse{Result: result}, nil
}

func (d *Dao) Get(key *proto.BaseKey) (*proto.GetResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	strVal, ok := val.(structure.StringXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("stringx")
	}

	return &proto.GetResponse{Result: strVal.Get()}, nil
}

func (d *Dao) Add(key *proto.BaseKey, renewal int32) (*proto.AddResponse, error) {
	value, lruOk := d.lru.Get(key)
	if !lruOk {
		val := stringx.NewStringSingle()
		res, err := val.Add(renewal)
		if err != nil {
			return nil, nil
		}
		d.lru.Add(key, val)
		return &proto.AddResponse{Result: res}, nil
	}

	strVal, ok := value.(structure.StringXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("stringx")
	}

	res, err := strVal.Add(renewal)
	if err != nil {
		return nil, err
	}
	return &proto.AddResponse{Result: res}, nil
}

func (d *Dao) Reduce(key *proto.BaseKey, renewal int32) (*proto.ReduceResponse, error) {
	value, lruOk := d.lru.Get(key)
	if !lruOk {
		val := stringx.NewStringSingle()
		res, err := val.Reduce(renewal)
		if err != nil {
			return nil, err
		}
		d.lru.Add(key, val)
		return &proto.ReduceResponse{Result: res}, nil
	}

	strVal, ok := value.(structure.StringXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("stringx")
	}

	res, err := strVal.Reduce(renewal)
	if err != nil {
		return nil, err
	}
	return &proto.ReduceResponse{Result: res}, nil
}

func (d *Dao) SetBit(key *proto.BaseKey, val bool, offer int32) (*proto.SetBitResponse, error) {
	value, lruOk := d.lru.Get(key)
	if !lruOk {
		valStr := stringx.NewStringSingle()
		length := valStr.Setbit(offer, val)
		d.lru.UpdateLruSize(length)
		d.lru.Add(key, valStr)
		return &proto.SetBitResponse{}, nil
	}

	strVal, ok := value.(structure.StringXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("stringx")
	}

	length := strVal.Setbit(offer, val)
	d.lru.UpdateLruSize(length)
	return &proto.SetBitResponse{}, nil
}

func (d *Dao) GetBit(key *proto.BaseKey, offer int32) (*proto.GetBitResponse, error) {
	value, lruOk := d.lru.Get(key)
	if !lruOk {
		return nil, errorx.NotKeyErr(key.Key)
	}
	strVal, ok := value.(structure.StringXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("stringx")
	}
	res, err := strVal.Getbit(offer)
	if err != nil {
		return nil, err
	}
	return &proto.GetBitResponse{Val: res}, nil
}

func (d *Dao) GetRange(key *proto.BaseKey, start, end int32) (*proto.GetRangeResponse, error) {
	value, lruOk := d.lru.Get(key)
	if !lruOk {
		return nil, errorx.NotKeyErr(key.Key)
	}
	strVal, ok := value.(structure.StringXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("stringx")
	}

	res, err := strVal.Getrange(start, end)
	if err != nil {
		return nil, err
	}
	return &proto.GetRangeResponse{Result: res}, nil
}

func (d *Dao) GetSet(key *proto.BaseKey, value string) (*proto.GetSetResponse, error) {
	val, ok := d.lru.Get(key)

	var oldValue string

	if !ok {
		oldValue = ""
		strValue := stringx.NewStringSingle()
		strValue.Set(value)
		err := d.lru.Add(key, strValue)
		if err != nil {
			return nil, err
		}

	} else {
		strVal, ok := val.(structure.StringXInterface)
		if !ok {
			return nil, errorx.DaoTypeErr("stringx")
		}
		oldValue = strVal.Get()
		_, updateLength := strVal.Set(value)
		d.lru.UpdateLruSize(updateLength)
	}

	return &proto.GetSetResponse{Result: oldValue}, nil
}

func (d *Dao) StrLen(key *proto.BaseKey) (*proto.StrLenResponse, error) {
	val, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	strVal, ok := val.(structure.StringXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("stringx")
	}

	return &proto.StrLenResponse{Length: int32(strVal.GetLength())}, nil
}

func (d *Dao) Setnx(key *proto.BaseKey, val string) (*proto.SetnxResponse, error) {
	_, ok := d.lru.Get(key)
	if ok {
		return nil, errorx.New("the key already exists")
	}

	strValue := stringx.NewStringSingle()
	strValue.Set(val)
	err := d.lru.Add(key, strValue)
	if err != nil {
		return nil, err
	}
	return &proto.SetnxResponse{}, nil
}
