// Code generated by gen-struct. DO NOT EDIT.
// make gen-service generated
package dao

import (
	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/lru"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	protobuf "google.golang.org/protobuf/proto"
)

type Dao struct {
	lru lru.CacheInterface
}

func NewDao(lru lru.CacheInterface) Interface {
	return &Dao{
		lru: lru,
	}
}

// 执行 msg
func (d *Dao) ExecMessage(message protobuf.Message) error {
	switch req := message.(type) {

	case *proto.LIndexRequest:
		_, err := d.LIndex(req.Key, req.Index)
		return err
	case *proto.LLenRequest:
		_, err := d.LLen(req.Key)
		return err
	case *proto.LPopRequest:
		_, err := d.LPop(req.Key, req.Count)
		return err
	case *proto.LPushRequest:
		_, err := d.LPush(req.Key, req.Values)
		return err
	case *proto.LPushXRequest:
		_, err := d.LPushX(req.Key, req.Values)
		return err
	case *proto.LRangeRequest:
		_, err := d.LRange(req.Key, req.Start, req.End)
		return err
	case *proto.LRemRequest:
		_, err := d.LRem(req.Key, req.Count, req.Value)
		return err
	case *proto.LSetRequest:
		_, err := d.LSet(req.Key, req.Index, req.Value)
		return err
	case *proto.RPopRequest:
		_, err := d.RPop(req.Key, req.Count)
		return err
	case *proto.LTrimRequest:
		_, err := d.LTrim(req.Key, req.Start, req.End)
		return err
	case *proto.RPushRequest:
		_, err := d.RPush(req.Key, req.Values)
		return err
	case *proto.RPushXRequest:
		_, err := d.RPushX(req.Key, req.Values)
		return err
	case *proto.HDelRequest:
		_, err := d.HDel(req.Key, req.HKeys)
		return err
	case *proto.HExistsRequest:
		_, err := d.HExists(req.Key, req.HKey)
		return err
	case *proto.HGetRequest:
		_, err := d.HGet(req.Key, req.HKeys)
		return err
	case *proto.HGetAllRequest:
		_, err := d.HGetAll(req.Key)
		return err
	case *proto.HIncrByRequest:
		_, err := d.HIncrBy(req.Key, req.HKeys, req.Renewal)
		return err
	case *proto.HKeysRequest:
		_, err := d.HKeys(req.Key)
		return err
	case *proto.HLenRequest:
		_, err := d.HLen(req.Key)
		return err
	case *proto.HSetRequest:
		_, err := d.HSet(req.Key, req.Items)
		return err
	case *proto.HSetXRequest:
		_, err := d.HSetX(req.Key, req.Items)
		return err
	case *proto.CPushRequest:
		_, err := d.CPush(req.Key, req.Value)
		return err
	case *proto.CPopRequest:
		_, err := d.CPop(req.Key, req.Count)
		return err
	case *proto.CMakeRequest:
		_, err := d.CMake(req.Key, req.Length)
		return err
	case *proto.CLenRequest:
		_, err := d.CLen(req.Key)
		return err
	case *proto.CCleanRequest:
		_, err := d.CClean(req.Key)
		return err
	case *proto.SetRequest:
		_, err := d.Set(req.Key, req.Val)
		return err
	case *proto.GetRequest:
		_, err := d.Get(req.Key)
		return err
	case *proto.AddRequest:
		_, err := d.Add(req.Key, req.Renewal)
		return err
	case *proto.ReduceRequest:
		_, err := d.Reduce(req.Key, req.Renewal)
		return err
	case *proto.SetnxRequest:
		_, err := d.Setnx(req.Key, req.Val)
		return err
	case *proto.SetBitRequest:
		_, err := d.SetBit(req.Key, req.Val, req.Offer)
		return err
	case *proto.GetBitRequest:
		_, err := d.GetBit(req.Key, req.Offer)
		return err
	case *proto.GetRangeRequest:
		_, err := d.GetRange(req.Key, req.Start, req.End)
		return err
	case *proto.GetSetRequest:
		_, err := d.GetSet(req.Key, req.Val)
		return err
	case *proto.StrLenRequest:
		_, err := d.StrLen(req.Key)
		return err
	case *proto.SAddRequest:
		_, err := d.SAdd(req.Key, req.Member)
		return err
	case *proto.SCardRequest:
		_, err := d.SCard(req.Key)
		return err
	case *proto.SDiffRequest:
		_, err := d.SDiff(req.Key, req.SKeys)
		return err
	case *proto.SDiffStoreRequest:
		_, err := d.SDiffStore(req.Key, req.SKeys, req.SaveKey)
		return err
	case *proto.SInterRequest:
		_, err := d.SInter(req.Key, req.SKeys)
		return err
	case *proto.SInterStoreRequest:
		_, err := d.SInterStore(req.Key, req.SKeys, req.SaveKey)
		return err
	case *proto.SIsMemberRequest:
		_, err := d.SIsMember(req.Key, req.Member)
		return err
	case *proto.SMoveRequest:
		_, err := d.SMove(req.Key, req.MoveKey, req.Members)
		return err
	case *proto.SPopRequest:
		_, err := d.SPop(req.Key, req.Count)
		return err
	case *proto.SRemRequest:
		_, err := d.SRem(req.Key, req.Count)
		return err
	case *proto.SUnionRequest:
		_, err := d.SUnion(req.Key, req.SKeys)
		return err
	case *proto.SUnionStoreRequest:
		_, err := d.SUnionStore(req.Key, req.SKeys, req.SaveKey)
		return err
	case *proto.SScanRequest:
		_, err := d.SScan(req.Key, req.Cursor, req.Regexp, req.Count)
		return err
	default:
		return errorx.New("The type that is not registered exec err")
	}
}
