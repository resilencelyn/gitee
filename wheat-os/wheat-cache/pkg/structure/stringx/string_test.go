package stringx

import (
	"fmt"
	"testing"
	"unsafe"

	"gitee.com/wheat-os/wheatCache/pkg/structure"
	"github.com/stretchr/testify/require"
)

func TestStringSingle_Set(t *testing.T) {
	s := NewStringSingle()
	resp, length := s.Set("189")
	require.Equal(t, resp, "189")
	require.Equal(t, length, structure.UpdateLength(0))
	i := s.Get()
	require.Equal(t, i, "189")

	resp, length = s.Set("189.12")
	require.Equal(t, resp, "189.12")
	require.Equal(t, length, structure.UpdateLength(0))
	i = s.Get()
	require.Equal(t, i, "189.12")

	resp, length = s.Set("awdawd")
	require.Equal(t, resp, "awdawd")
	require.Equal(t, length, structure.UpdateLength(-2))
	i = s.Get()
	require.Equal(t, i, "awdawd")
}

func TestStringSingle_Add(t *testing.T) {
	s := NewStringSingle()
	s.Set("189")
	s.Add(1)
	i := s.Get()
	require.Equal(t, i, "190")

	s.Set("189.2")
	s.Add(1)
	i = s.Get()
	require.Equal(t, i, "190.20")

	s.Set("wad")
	_, err := s.Add(1)
	require.Error(t, err)
}

func TestStringSingle_Reduce(t *testing.T) {
	s := NewStringSingle()
	s.Set("189")
	s.Reduce(1)
	i := s.Get()
	require.Equal(t, i, "188")

	s.Set("189.2")
	s.Reduce(1)
	i = s.Get()
	require.Equal(t, i, "188.20")

	s.Set("wad")
	_, err := s.Reduce(1)
	require.Error(t, err)
}

func TestStringSingle_Getbit(t *testing.T) {
	s := NewStringSingle()
	length := s.Setbit(1009, true)
	require.Equal(t, length, structure.UpdateLength(1002))

	res, err := s.Getbit(1009)
	require.NoError(t, err)
	require.Equal(t, res, true)

	res, err = s.Getbit(1008)
	require.NoError(t, err)
	require.Equal(t, res, false)

	length = s.Setbit(1009, false)
	require.Equal(t, length, structure.UpdateLength(0))
	res, err = s.Getbit(1009)
	require.NoError(t, err)
	require.Equal(t, res, false)
}

func TestStringSingle_Getrange(t *testing.T) {
	s := NewStringSingle()
	s.Set("abcdefg")
	k, err := s.Getrange(0, 3)
	require.NoError(t, err)
	require.Equal(t, "abc", k)
}

func TestPointSize(t *testing.T) {
	var a *int32
	fmt.Println(unsafe.Sizeof(a))
}
