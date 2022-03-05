package listx

import (
	"testing"

	"github.com/stretchr/testify/require"
)

func TestListx_LPush_And_Pop(t *testing.T) {
	list := NewListXSingle()
	up := list.LPush("1", "2", "3")
	require.Equal(t, list.Length(), 3)

	values, updateLength := list.LPop(3)

	require.Equal(t, values, []string{"3", "2", "1"})
	require.Equal(t, up, updateLength)

	list.LPush("1", "2", "3")
	values, updateLength = list.LPop(1)
	require.Equal(t, values, []string{"3"})
	require.Equal(t, int(updateLength), 24)

}

func TestListx_RPush_Pop(t *testing.T) {
	list := NewListXSingle()
	up := list.RPush("1", "2", "3")
	require.Equal(t, list.Length(), 3)

	values, updateLength := list.LPop(3)
	require.Equal(t, values, []string{"1", "2", "3"})
	require.Equal(t, up, updateLength)

	list.RPush("1", "2", "3")
	values, updateLength = list.RPop(2)
	require.Equal(t, values, []string{"3", "2"})
	require.Equal(t, int(updateLength), 48)
}

func TestListx_location(t *testing.T) {
	list := &Listx{
		head:   nil,
		tail:   nil,
		length: 0,
	}

	list.RPush("1", "2", "3")
	node, err := list.location(1)
	require.NoError(t, err)
	require.Equal(t, node.val.ToString(), "2")

	node, err = list.location(0)
	require.NoError(t, err)
	require.Equal(t, node.val.ToString(), "1")

	node, err = list.location(2)
	require.NoError(t, err)
	require.Equal(t, node.val.ToString(), "3")

	_, err = list.location(3)
	require.Error(t, err)

	node, err = list.location(-1)
	require.NoError(t, err)
	require.Equal(t, node.val.ToString(), "3")

	node, err = list.location(-2)
	require.NoError(t, err)
	require.Equal(t, node.val.ToString(), "2")

	node, err = list.location(-3)
	require.NoError(t, err)
	require.Equal(t, node.val.ToString(), "1")

	_, err = list.location(-4)
	require.Error(t, err)
}

func TestListx_Insert(t *testing.T) {
	list := NewListXSingle()
	list.LPush("a", "b", "c", "d", "e")
	val, _ := list.LPop(1)
	require.Equal(t, val, []string{"e"})

	res, err := list.Index(1)
	require.NoError(t, err)
	require.Equal(t, res, "c")

	_, err = list.Insert(1, false, "1", "2", "3")
	require.NoError(t, err)

	// 全部取出
	val, _ = list.LPop(list.Length())
	require.Equal(t, val, []string{"d", "3", "2", "1", "c", "b", "a"})

	list.RPush("1", "2", "3", "4", "5")
	res, err = list.Index(-2)
	require.NoError(t, err)
	require.Equal(t, res, "4")

	_, err = list.Insert(-1, true, "6", "7")
	require.NoError(t, err)

	val, _ = list.LPop(list.Length())
	require.Equal(t, val, []string{"1", "2", "3", "4", "5", "6", "7"})
}

func TestListx_Index(t *testing.T) {
	list := &Listx{
		head:   nil,
		tail:   nil,
		length: 0,
	}

	list.RPush("a", "b", "c", "d", "e")

	index := 2
	leftIndex, err := list.leftIndex(index)
	require.NoError(t, err)
	rightIndex, err := list.rightIndex(index)
	require.NoError(t, err)
	v1, err := list.Index(leftIndex)
	require.NoError(t, err)
	v2, err := list.Index(rightIndex)
	require.NoError(t, err)
	require.Equal(t, v1, v2)
}

func TestListx_Slice(t *testing.T) {
	list := NewListXSingle()
	list.RPush("a", "b", "c", "d", "e")

	// 主流程测试
	list.Slice(1, 2)
	values, _ := list.LPop(list.Length())
	require.Equal(t, values, []string{"b"})

	list2 := NewListXSingle()
	list2.RPush("1", "2", "3", "4", "5")

	list2.Slice(0, 4)
	values, _ = list2.LPop(list2.Length())
	require.Equal(t, values, []string{"1", "2", "3", "4"})

	list3 := NewListXSingle()
	list3.RPush("1", "2", "3", "4", "5")

	list3.Slice(2, list3.Length())
	values, _ = list3.LPop(list3.Length())
	require.Equal(t, values, []string{"3", "4", "5"})

	// 测试负数索引
	list3 = NewListXSingle()
	list3.RPush("1", "2", "3", "4", "5")

	_, err := list3.Slice(0, -2)
	require.NoError(t, err)
	values, _ = list3.LPop(list3.Length())
	require.Equal(t, values, []string{"1", "2", "3"})

	// 测试负数双边际
	list3 = NewListXSingle()
	list3.RPush("1", "2", "3", "4", "5")

	_, err = list3.Slice(-3, -2)
	require.NoError(t, err)
	values, _ = list3.LPop(list3.Length())
	require.Equal(t, values, []string{"3"})

	// 测试负数边际
	list3 = NewListXSingle()
	list3.RPush("1", "2", "3", "4", "5")

	_, err = list3.Slice(-5, 4)
	require.NoError(t, err)
	values, _ = list3.LPop(list3.Length())
	require.Equal(t, values, []string{"1", "2", "3", "4"})
}

func TestListx_Range(t *testing.T) {
	list := NewListXSingle()
	list.RPush("a", "b", "c", "d", "e")
	val, err := list.Range(0, 2)
	require.NoError(t, err)

	require.Equal(t, val, []string{"a", "b"})

	val, err = list.Range(0, -1)
	require.NoError(t, err)

	require.Equal(t, val, []string{"a", "b", "c", "d"})

	val, err = list.Range(-3, 3)
	require.NoError(t, err)

	require.Equal(t, val, []string{"c"})

	val, err = list.Range(-1, list.Length())
	require.NoError(t, err)

	require.Equal(t, val, []string{})

	_, err = list.Range(6, -1)
	require.Error(t, err)
}

func TestListx_Remove(t *testing.T) {
	list := NewListXSingle()
	list.RPush("a", "b", "c", "c", "e")

	count, up := list.Remove("c", -2)
	require.Equal(t, count, 2)
	require.Equal(t, int(up), 34)

	res, _ := list.LPop(list.Length())
	require.Equal(t, res, []string{"a", "b", "e"})

	list = NewListXSingle()
	list.RPush("a", "b", "c", "c", "e")

	count, up = list.Remove("b", 1)
	require.Equal(t, count, 1)
	require.Equal(t, int(up), 17)

	res, _ = list.LPop(list.Length())
	require.Equal(t, res, []string{"a", "c", "c", "e"})
}
