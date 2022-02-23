package dao

import (
	"testing"

	"gitee.com/wheat-os/wheatCache/pkg/lru"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"github.com/stretchr/testify/require"
)

func initData(t *testing.T, key *proto.BaseKey, dao Interface) {
	_, err := dao.HSet(key, map[string]string{
		"test":  "test",
		"test1": "1",
		"test2": "1.1",
	})
	require.NoError(t, err)
}

func TestDao_HSet(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")

	initData(t, baseKey, dao)

	res, err := dao.HGet(baseKey, []string{"test", "test1"})
	require.NoError(t, err)

	require.Equal(t, res.Items, map[string]string{
		"test":  "test",
		"test1": "1",
	})

	res, err = dao.HGet(baseKey, []string{"test", "test1", "test2"})
	require.NoError(t, err)

	require.Equal(t, res.Items, map[string]string{
		"test":  "test",
		"test1": "1",
		"test2": "1.10", // 默认2个小数点
	})
}

func TestDao_HGet(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	initData(t, baseKey, dao)
	res, err := dao.HGet(baseKey, []string{"test", "test1", "test2"})
	require.NoError(t, err)

	require.Equal(t, res.Items, map[string]string{
		"test":  "test",
		"test1": "1",
		"test2": "1.10", // 默认2个小数点
	})

	_, err = dao.HGet(baseKey, []string{"test", "test1", "test2", "a"})
	require.Error(t, err)
}

func TestDao_HExists(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	initData(t, baseKey, dao)

	resp, err := dao.HExists(baseKey, "test")
	require.NoError(t, err)
	require.Equal(t, resp.Exists, true)

	resp, err = dao.HExists(baseKey, "kjd")
	require.NoError(t, err)
	require.Equal(t, resp.Exists, false)
}

func TestDao_HDel(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	initData(t, baseKey, dao)
	_, err := dao.HDel(baseKey, []string{"test", "test1"})
	require.NoError(t, err)

	_, err = dao.HDel(baseKey, []string{"test"})
	require.Error(t, err)

	res, err := dao.HExists(baseKey, "test")
	require.NoError(t, err)
	require.Equal(t, res.Exists, false)

	res, err = dao.HExists(baseKey, "test2")
	require.NoError(t, err)
	require.Equal(t, res.Exists, true)
}

func TestDao_HGetAll(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	initData(t, baseKey, dao)
	resp, err := dao.HGetAll(baseKey)
	require.NoError(t, err)

	require.Equal(t, resp.Items, map[string]string{
		"test":  "test",
		"test1": "1",
		"test2": "1.10",
	})
}

func TestDao_HSetX(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	initData(t, baseKey, dao)
	res, err := dao.HSetX(baseKey, map[string]string{
		"test":  "test",
		"test1": "1",
		"test2": "1.10", // 默认2个小数点
	})
	require.NoError(t, err)
	require.Equal(t, res.Count, 0)
}

func TestDao_HLen(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	initData(t, baseKey, dao)
	resp, err := dao.HLen(baseKey)
	require.NoError(t, err)

	require.Equal(t, resp.Length, int32(3))
}

func TestDao_HKeys(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	initData(t, baseKey, dao)

	res, err := dao.HKeys(baseKey)
	require.NoError(t, err)
	require.Equal(t, res.Keys, []string{"test", "test1", "test2"})
}

func TestDao_HIncrBy(t *testing.T) {
	lru := lru.NewLRUCache()
	dao := NewDao(lru)
	baseKey := proto.NewBaseKey("apple")
	initData(t, baseKey, dao)

	resp, err := dao.HIncrBy(baseKey, []string{"test", "test1"}, 10)
	require.NoError(t, err)
	require.Equal(t, resp.Count, int32(1))
	require.Equal(t, resp.Values, []string{"11"})

}
