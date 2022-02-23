package lru

import (
	"fmt"
	"testing"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure/stringx"
	"github.com/stretchr/testify/require"
)

func TestNewLRUCache(t *testing.T) {
	cache := NewLRUCache()
	v1 := stringx.NewStringSingle()
	v2 := stringx.NewStringSingle()
	v3 := stringx.NewStringSingle()
	key1 := proto.BaseKey{
		Key: "1",
	}
	key2 := proto.BaseKey{
		Key: "2",
	}
	key3 := proto.BaseKey{
		Key: "3",
	}
	cache.Add(&key1, v1)
	cache.Add(&key2, v2)
	cache.Add(&key3, v3)
	cache.Add(&key1, v1)
	fmt.Println(cache.nowSize)
	cache.Del()
	fmt.Println(cache.nowSize)
	_, isTrue := cache.Get(&key1)
	require.Equal(t, isTrue, true)
}

func TestNewLRUCache2(t *testing.T) {
	//根据key删除
	cache := NewLRUCache()
	v1 := stringx.NewStringSingle()
	v2 := stringx.NewStringSingle()
	v3 := stringx.NewStringSingle()
	key1 := proto.BaseKey{
		Key: "1",
	}
	key2 := proto.BaseKey{
		Key: "2",
	}
	key3 := proto.BaseKey{
		Key: "3",
	}
	cache.Add(&key1, v1)
	cache.Add(&key2, v2)
	cache.Add(&key3, v3)
	cache.DelByKey(&key1)
	_, ok := cache.Get(&key1)
	require.Equal(t, ok, false)
	require.Error(t, cache.DelByKey(&key1))
}

func TestLruProcess(t *testing.T) {
	lru := NewLRUCache()
	lru.clearSize = 3600

	for i := 100; i < 200; i++ {
		lru.Add(&proto.BaseKey{
			Key: fmt.Sprint(i),
			Ttl: 20 << 2,
		}, stringx.NewStringSingle())
	}

	// mock LruKey
	for i := 0; i < 100; i++ {
		lru.Add(&proto.BaseKey{
			Key: fmt.Sprint(i),
			Ttl: 4,
		}, stringx.NewStringSingle())
	}

	require.Equal(t, lru.nowSize, int64(200*24))

	// 自动清理测试
	fmt.Println(lru.clearSize)
	require.Equal(t, lru.li.Len(), 200)
	time.Sleep(3 * time.Second)
	require.Less(t, lru.nowSize, lru.clearSize+1)

	// TTL 测试, 100-200 key 发生自动清理 留下 50-100 共 100（0-100） + 20个 key，5s 后，前 0-100的 key 过期，剩下
	time.Sleep(2 * time.Second)
	require.Equal(t, lru.li.Len(), 50)

	// 过期全部的 Key
	for i := 100; i < 200; i++ {
		lru.UpdateTTl(&proto.BaseKey{
			Key: fmt.Sprint(i),
			Ttl: -1,
		})
	}

	time.Sleep(2 * time.Second)
	require.Equal(t, lru.nowSize, int64(0))
}
