package dao

import (
	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
	"gitee.com/wheat-os/wheatCache/pkg/structure/hashx"
)

func (d *Dao) HDel(key *proto.BaseKey, hKeys []string) (*proto.HDelResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}

	for _, hK := range hKeys {
		up, err := hashVal.Del(hK)
		if err != nil {
			return nil, err
		}
		d.lru.UpdateLruSize(up)
	}

	return &proto.HDelResponse{}, nil

}

func (d *Dao) HExists(key *proto.BaseKey, hKeys string) (*proto.HExistsResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}
	_, err := hashVal.Get(hKeys)
	if err != nil {
		return &proto.HExistsResponse{Exists: false}, nil
	}
	return &proto.HExistsResponse{Exists: true}, nil
}

func (d *Dao) HGet(key *proto.BaseKey, hKeys []string) (*proto.HGetResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}

	result := make(map[string]string)
	for _, hK := range hKeys {
		res, err := hashVal.Get(hK)
		if err != nil {
			return nil, err
		}
		result[hK] = res
	}
	return &proto.HGetResponse{Items: result}, nil
}

func (d *Dao) HGetAll(key *proto.BaseKey) (*proto.HGetAllResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}
	return &proto.HGetAllResponse{Items: hashVal.Item()}, nil
}

func (d *Dao) HIncrBy(key *proto.BaseKey, hKeys []string, renewal int32) (*proto.HIncrByResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}
	count, result, err := hashVal.Add(int(renewal), hKeys...)
	return &proto.HIncrByResponse{
		Count:  int32(count),
		Values: result,
	}, err
}

func (d *Dao) HKeys(key *proto.BaseKey) (*proto.HKeysResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}

	return &proto.HKeysResponse{Keys: hashVal.Key()}, nil
}

func (d *Dao) HLen(key *proto.BaseKey) (*proto.HLenResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return nil, errorx.NotKeyErr(key.Key)
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}

	return &proto.HLenResponse{Length: int32(hashVal.Length())}, nil
}

func (d *Dao) HSet(key *proto.BaseKey, set map[string]string) (*proto.HSetResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		hashVal := hashx.NewHashXSingle()
		for k, v := range set {
			hashVal.Set(k, v)
		}
		err := d.lru.Add(key, hashVal)
		return &proto.HSetResponse{}, err
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}
	for k, v := range set {
		upLength := hashVal.Set(k, v)
		d.lru.UpdateLruSize(upLength)
	}

	return &proto.HSetResponse{}, nil
}

func (d *Dao) HSetX(key *proto.BaseKey, set map[string]string) (*proto.HSetXResponse, error) {
	value, ok := d.lru.Get(key)
	if !ok {
		return &proto.HSetXResponse{}, errorx.NotKeyErr(key.Key)
	}

	hashVal, ok := value.(structure.HashXInterface)
	if !ok {
		return nil, errorx.DaoTypeErr("hashx")
	}
	for k, v := range set {
		upLength := hashVal.Set(k, v)
		d.lru.UpdateLruSize(upLength)
	}

	return &proto.HSetXResponse{}, nil
}
