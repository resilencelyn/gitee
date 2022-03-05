package lru

import (
	"container/list"
	"sync/atomic"

	_ "gitee.com/wheat-os/wheatCache/conf"
	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/event"
	"gitee.com/wheat-os/wheatCache/pkg/event2"
	"gitee.com/wheat-os/wheatCache/pkg/middle"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
	"gitee.com/wheat-os/wheatCache/pkg/util"
	"github.com/spf13/viper"
)

type keyBaseValue struct {
	key    string
	val    structure.KeyBaseInterface
	expire int64 // 过期时间戳
}

type SingleCache struct {
	maxsize         int64 //最大的长度
	clearSize       int64 // 清理长度
	nowSize         int64 // 现在的长度
	li              *list.List
	lruMap          map[string]*list.Element
	lruMaxDiverSize int
	lruTtlManage    *lruTTl // 定时清理器

	lruDriver       event2.DriverInterface
	lruConsumer     event2.ConsumerInterface
	lruCleanProduce event2.ProduceInterface // 发送清理事件

	middleProduce event.ProduceInterface // 中间件驱动
}

// UpdateLruSize 更新现在的长度
func (lru *SingleCache) UpdateLruSize(length structure.UpdateLength) {
	atomic.AddInt64(&lru.nowSize, int64(length))
}

func cacheInit() (int64, int64, int, int) {
	maxSize := viper.GetString("lruCache.maxSize")
	retMaxSize, maxErr := util.ParseSizeToBit(maxSize)
	if maxErr != nil {
		return 0, 0, 0, 0
	}
	if retMaxSize == 0 {
		retMaxSize = defaultLruMaxSize
	}

	clearSize := viper.GetString("lruCache.clearSize")
	retClearSize, clearErr := util.ParseSizeToBit(clearSize)
	if clearErr != nil {
		return 0, 0, 0, 0
	}
	if retClearSize == 0 {
		retClearSize = defaultLruClearSize
	}

	maxDriver := viper.GetInt("lruCache.eventDriverSize")
	if maxDriver == 0 {
		maxDriver = defaultLruEventDriver
	}

	detachNum := viper.GetInt("lruCache.detachNum")
	if detachNum == 0 {
		detachNum = defaultDetachNum
	}

	return retMaxSize, retClearSize, maxDriver, detachNum
}

// NewLRUCache lru初始化
func NewLRUCache() *SingleCache {
	maxSize, clearSize, maxDriverSize, detachNum := cacheInit()
	lruDriver := event2.NewDriver(maxDriverSize)
	lruCacheOnce.Do(func() {
		lru := &SingleCache{
			maxsize:         maxSize,
			clearSize:       clearSize,
			nowSize:         0,
			li:              list.New(),
			lruMap:          make(map[string]*list.Element),
			lruMaxDiverSize: maxDriverSize,
			lruDriver:       lruDriver,
			lruConsumer:     event2.NewConsumer(lruDriver),
			lruCleanProduce: event2.NewProduce(lruDriver),
			middleProduce:   event.NewProduce(middle.NewMiddleWare().GetEventDriver()),
			lruTtlManage:    newLruTTl(detachNum),
		}
		lruCache = lru

		// 启动 lru 事件驱动
		go lru.lruSingleWork()
		go lru.lruTtlWork()
		go lru.cleanWork()

	})
	return lruCache
}

// GetDriver 获取驱动
func (lru *SingleCache) GetDriver() event2.DriverInterface {
	return lru.lruDriver
}

//Add 增加
func (lru *SingleCache) Add(key *proto.BaseKey, val structure.KeyBaseInterface) error {

	if key == nil {
		return errorx.KeyBaseIsNilErr()
	}

	exp := lru.lruTtlManage.setKeys(key)
	keyBaseVal := &keyBaseValue{
		key:    key.Key,
		val:    val,
		expire: exp,
	}
	if elVal, ok := lru.lruMap[key.Key]; ok {
		lru.li.MoveToFront(elVal)
		oldSize := elVal.Value.(structure.KeyBaseInterface).SizeByte()

		lru.UpdateLruSize(structure.UpdateLength(val.SizeByte() - oldSize))
		elVal.Value = keyBaseVal
		return nil
	}
	valEl := lru.li.PushFront(keyBaseVal)
	lru.lruMap[key.Key] = valEl
	//增加大小
	lru.UpdateLruSize(structure.UpdateLength(valEl.Value.(*keyBaseValue).val.SizeByte()))
	return nil
}

// Get 查找key对应的value
func (lru *SingleCache) Get(key *proto.BaseKey) (structure.KeyBaseInterface, bool) {

	if key == nil {
		return nil, false
	}

	if elVal, ok := lru.lruMap[key.Key]; ok {
		lru.li.MoveToFront(elVal)
		return elVal.Value.(*keyBaseValue).val, true
	}
	return nil, false
}

//Del 删除机制
func (lru *SingleCache) Del() error {
	if lru.lruMap == nil {
		return errorx.New("lru is nil")
	}
	data := lru.li.Back()
	delete(lru.lruMap, data.Value.(*keyBaseValue).key)
	//删除大小
	lru.UpdateLruSize(structure.UpdateLength(-1 * data.Value.(*keyBaseValue).val.SizeByte()))
	lru.li.Remove(data)
	return nil
}

//DelByKey 根据key删除
func (lru *SingleCache) DelByKey(key *proto.BaseKey) error {

	if key == nil {
		return errorx.KeyBaseIsNilErr()
	}

	if lru.lruMap == nil {
		return errorx.New("lru is nil")
	}
	if el, ok := lru.lruMap[key.Key]; ok {
		delete(lru.lruMap, key.Key)
		lru.li.Remove(el)
		lru.UpdateLruSize(structure.UpdateLength(-1 * el.Value.(*keyBaseValue).val.SizeByte()))
		return nil
	}
	return errorx.New("lru no this key")
}

//DelByKeyAndExTtl 根据key(string)删除已经过期的 key
func (lru *SingleCache) delByKeyAndExTtl(key string, beforeTime int64) {
	if elVal, ok := lru.lruMap[key]; ok {
		exp := elVal.Value.(*keyBaseValue).expire
		if exp <= beforeTime {
			delete(lru.lruMap, key)
			lru.li.Remove(elVal)
			lru.UpdateLruSize(structure.UpdateLength(-1 * elVal.Value.(*keyBaseValue).val.SizeByte()))
		}
	}
}

func (lru *SingleCache) DelToClearSize() error {
	if lru.lruMap == nil {
		return errorx.New("lru is nil")
	}
	for lru.nowSize > lru.clearSize {
		//del自动给nowSize进行大小的改变
		err := lru.Del()
		if err != nil {
			return err
		}

	}
	return nil
}

// 更新过期时间
func (lru *SingleCache) UpdateTTl(key *proto.BaseKey) error {

	if key == nil {
		return errorx.KeyBaseIsNilErr()
	}

	if elVal, ok := lru.lruMap[key.Key]; ok {
		expire := lru.lruTtlManage.setKeys(key)
		elVal.Value.(*keyBaseValue).expire = expire
	}

	return errorx.New("the key is not in lru cache, key:%s", key.Key)
}
