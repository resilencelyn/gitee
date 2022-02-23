package skiplist

import (
	"fmt"
	"math/rand"
	"testing"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/logx"
	"github.com/stretchr/testify/require"
)

// 时间测试
func TestSkiplist_InsertTimeB(t *testing.T) {
	// 测试 达到 maxLevel = 18 时性能 OK
	list := NewSkipList(18)

	for i := 0; i < 100; i++ {
		list.Insert(float64(i), i)
	}
	list.debugPrint()

}

func TestSkiplist_InsertTimeA(t *testing.T) {
	// 测试 达到 maxLevel = 18 时性能 OK
	list := NewSkipList(20)

	for i := 0; i < 200; i++ {
		list.Insert(float64(rand.Intn(100)), i)
	}
	list.debugPrint()

	start := time.Now()
	list.Insert(7890, 1)
	end := time.Now()

	fmt.Println(end.UnixNano() - start.UnixNano())
}

func TestSkipList_Insert(t *testing.T) {
	s := NewSkipList(18)
	s.Insert(20, 1)
	s.Insert(30, 2)
	s.Insert(11, 3)
	s.Insert(20, 11)
	s.debugPrint()
	val := s.Get(30)
	require.Equal(t, 2, val)
	val = s.Get(11)
	require.Equal(t, 3, val)

	require.Len(t, s.GetAll(20), 2)

	s.RemoveAll(20)
	require.Equal(t, s.GetAll(20), []interface{}{})
}

func Test_skipList_ClearLeft(t *testing.T) {
	s := NewSkipList(18)
	for i := 0; i < 100; i++ {
		s.Insert(float64(rand.Intn(100)), i)
	}

	val := s.PopLeft(50)
	logx.Debug("val:%v", val)
	s.debugPrint()
	require.Equal(t, s.Get(20), nil)

}

func TestSkipList_PopRangeByScore(t *testing.T) {
	startNum := 5
	endNum := 79
	result := make([]interface{}, 0)
	for i := startNum; i <= endNum; i++ {
		result = append(result, i)
	}

	for i := 0; i < 100; i++ {
		s := NewSkipList(14)
		for i := 0; i < 100; i++ {
			s.Insert(float64(i), i)
		}
		res := s.PopRangeByScore(float64(startNum), float64(endNum))
		require.Equal(t, res, result)
	}
}

func TestSkipList_PopRight(t *testing.T) {
	score := 33
	result := make([]interface{}, 0)
	for i := score; i < 100; i++ {
		result = append(result, i)
	}

	for i := 0; i < 100; i++ {
		s := NewSkipList(14)
		for i := 0; i < 100; i++ {
			s.Insert(float64(i), i)
		}
		res := s.PopRight(float64(score))
		require.Equal(t, res, result)
		s.debugPrint()
	}
}

func Test_skipList_InsertTime(t *testing.T) {
	now := time.Now()
	s := NewSkipList(18)
	for i := 0; i < 20; i++ {
		s.Insert(float64(now.UnixMicro()), now)
	}
	s.debugPrint()

}
