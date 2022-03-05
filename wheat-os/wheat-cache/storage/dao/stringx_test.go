package dao

import (
	// "reflect"
	"testing"

	"gitee.com/wheat-os/wheatCache/pkg/lru"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"github.com/stretchr/testify/require"
)

func TestDao_Set_Get(t *testing.T) {
	baseKey := proto.NewBaseKey("abbs")
	lru := lru.NewLRUCache()
	dao := NewDao(lru)

	_, err := dao.Set(baseKey, "bbq")
	require.NoError(t, err)
	resp, err := dao.Get(baseKey)
	require.NoError(t, err)
	require.Equal(t, resp.Result, "bbq")
}

func TestDao_Add(t *testing.T) {
	baseKey := proto.NewBaseKey("test")
	lru := lru.NewLRUCache()
	dao := NewDao(lru)

	// 整数 add
	dao.Set(baseKey, "1")
	dao.Add(baseKey, 2)
	resp, err := dao.Get(baseKey)
	require.NoError(t, err)

	require.Equal(t, resp.Result, "3")

	// 浮点数
	dao.Set(baseKey, "1.1")
	dao.Add(baseKey, 2)
	resp, err = dao.Get(baseKey)
	require.NoError(t, err)

	require.Equal(t, resp.Result, "3.10")

	// 字符串
	dao.Set(baseKey, "1awd.1")
	_, err = dao.Add(baseKey, 2)
	require.Error(t, err)
}

func TestDao_Reduce(t *testing.T) {
	baseKey := proto.NewBaseKey("test")
	lru := lru.NewLRUCache()
	dao := NewDao(lru)

	// 整数 add
	dao.Set(baseKey, "1")
	dao.Reduce(baseKey, 2)
	resp, err := dao.Get(baseKey)
	require.NoError(t, err)

	require.Equal(t, resp.Result, "-1")

	// 浮点数
	dao.Set(baseKey, "1.1")
	dao.Reduce(baseKey, 2)
	resp, err = dao.Get(baseKey)
	require.NoError(t, err)

	require.Equal(t, resp.Result, "-0.90")

	// 字符串
	dao.Set(baseKey, "1awd.1")
	_, err = dao.Reduce(baseKey, 2)
	require.Error(t, err)
}

func TestDao_SetBit_GitBit(t *testing.T) {
	baseKey := proto.NewBaseKey("s")
	lru := lru.NewLRUCache()
	dao := NewDao(lru)

	_, err := dao.GetBit(baseKey, 8)
	require.Error(t, err)

	_, err = dao.SetBit(baseKey, true, 8)
	require.NoError(t, err)
	resp, err := dao.GetBit(baseKey, 8)
	require.NoError(t, err)
	require.Equal(t, resp.Val, true)

	resp, err = dao.GetBit(baseKey, 7)
	require.NoError(t, err)
	require.Equal(t, resp.Val, false)

	resp, err = dao.GetBit(baseKey, 9)
	require.NoError(t, err)
	require.Equal(t, resp.Val, false)
}

func TestDao_GetRange(t *testing.T) {
	baseKey := proto.NewBaseKey("s")
	lru := lru.NewLRUCache()
	dao := NewDao(lru)

	_, err := dao.GetRange(baseKey, 0, 5)
	require.Error(t, err)

	_, err = dao.Set(baseKey, "abcdef")
	require.NoError(t, err)
	resp, err := dao.GetRange(baseKey, 0, 3)
	require.NoError(t, err)
	require.Equal(t, resp.Result, "abc")

	_, err = dao.GetRange(baseKey, 0, 7)
	require.Error(t, err)

	_, err = dao.Set(baseKey, "123456")
	require.NoError(t, err)
	resp, err = dao.GetRange(baseKey, 0, 3)
	require.NoError(t, err)
	require.Equal(t, resp.Result, "123")
}

func TestDao_GetSet(t *testing.T) {
	baseKey := proto.NewBaseKey("s")
	lru := lru.NewLRUCache()
	dao := NewDao(lru)

	_, err := dao.Set(baseKey, "a")
	require.NoError(t, err)
	resp, err := dao.GetSet(baseKey, "ab")
	require.NoError(t, err)
	require.Equal(t, resp.Result, "a")

	resp, err = dao.GetSet(baseKey, "s")
	require.NoError(t, err)
	require.Equal(t, resp.Result, "a")

	l := proto.NewBaseKey("l")
	resp, err = dao.GetSet(l, "s")
	require.NoError(t, err)
	require.Equal(t, resp.Result, "")
}

func TestDao_StrLen(t *testing.T) {
	baseKey := proto.NewBaseKey("s")
	lru := lru.NewLRUCache()
	dao := NewDao(lru)

	_, err := dao.StrLen(baseKey)
	require.Error(t, err)

	_, err = dao.Set(baseKey, "abc")
	require.NoError(t, err)
	resp, err := dao.StrLen(baseKey)
	require.NoError(t, err)
	require.Equal(t, resp.Length, int32(3))

}

func TestDao_Setnx(t *testing.T) {
	baseKey := proto.NewBaseKey("s")
	lru := lru.NewLRUCache()
	dao := NewDao(lru)

	_, err := dao.Setnx(baseKey, "abc")
	require.NoError(t, err)
	resp, err := dao.Get(baseKey)
	require.NoError(t, err)
	require.Equal(t, resp.Result, "abc")

	_, err = dao.Setnx(baseKey, "abc")
	require.Error(t, err)
}
