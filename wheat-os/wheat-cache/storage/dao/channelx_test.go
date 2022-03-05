package dao

import (
	"strconv"
	"testing"

	"gitee.com/wheat-os/wheatCache/pkg/event2"
	"gitee.com/wheat-os/wheatCache/pkg/lru"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"github.com/stretchr/testify/require"
)

func execWaitFunc(t *testing.T, work interface{}) (interface{}, error) {
	wait, ok := work.(event2.EventAwaitFunc)
	require.True(t, ok)

	return wait()
}

func TestDao_CPush_CPop(t *testing.T) {
	testBaseKey := proto.NewBaseKey("123")
	l := lru.NewLRUCache()
	dao := NewDao(l)

	_, err := dao.CMake(testBaseKey, 200)
	require.NoError(t, err)

	wait, err := dao.CPush(testBaseKey, []string{"1", "2", "3"})
	require.NoError(t, err)
	waitFunc, ok := wait.(event2.EventAwaitFunc)
	require.True(t, ok)
	_, err = waitFunc()
	require.NoError(t, err)

	wait, err = dao.CPop(testBaseKey, 3)
	require.NoError(t, err)

	res, err := execWaitFunc(t, wait)
	require.NoError(t, err)
	require.Equal(t, res.(*proto.CPopResponse).Result, []string{"1", "2", "3"})
}

func TestDao_Async_Push(t *testing.T) {
	testBaseKey := proto.NewBaseKey("123")
	l := lru.NewLRUCache()
	dao := NewDao(l)

	_, err := dao.CMake(testBaseKey, 200)
	require.NoError(t, err)

	rq := make([]string, 0)
	for i := 0; i < 300; i++ {
		rq = append(rq, strconv.Itoa(i))
	}

	go func() {

		wait, err := dao.CPush(testBaseKey, rq)
		require.NoError(t, err)
		_, err = execWaitFunc(t, wait)
		require.NoError(t, err)
	}()

	wait, err := dao.CPop(testBaseKey, 300)
	require.NoError(t, err)
	resp, err := execWaitFunc(t, wait)
	require.NoError(t, err)

	require.Equal(t, resp.(*proto.CPopResponse).Result, rq)
}
