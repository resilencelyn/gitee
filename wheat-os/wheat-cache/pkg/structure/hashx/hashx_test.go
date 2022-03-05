package hashx

import (
	"fmt"
	"reflect"
	"regexp"
	"testing"
	"unsafe"

	"github.com/stretchr/testify/require"
)

func TestHashX_Set_SetX(t *testing.T) {
	h := NewHashXSingle()
	h.Set("key", "opq")
	res, err := h.Get("key")
	require.NoError(t, err)
	require.Equal(t, res, "opq")
	b, _ := h.SetX("key", "opq")
	require.Equal(t, b, false)

	b, _ = h.SetX("key1", "opq")
	require.Equal(t, b, true)
	res, err = h.Get("key1")
	require.NoError(t, err)
	require.Equal(t, res, "opq")
}

func TestHashX_Del(t *testing.T) {
	h := NewHashXSingle()
	up := h.Set("key", "opq")
	upu, err := h.Del("key")
	require.NoError(t, err)
	require.Equal(t, up, upu)
}

func TestHashX_Key(t *testing.T) {
	h := NewHashXSingle()
	h.Set("key", "opq")
	h.Set("key1", "opq")
	h.Set("key2", "opq")

	require.Equal(t, h.Key(), []string{"key", "key1", "key2"})
}

func TestHashX_Value(t *testing.T) {
	h := NewHashXSingle()
	h.Set("key", "opq")
	h.Set("key1", "opq")
	h.Set("key2", "opq")

	require.Equal(t, h.Value(), []string{"opq", "opq", "opq"})
}

func TestHashX_Add(t *testing.T) {
	h := NewHashXSingle()
	h.Set("1", "1")
	c, res, err := h.Add(1, "1")
	require.NoError(t, err)
	require.Equal(t, c, 1)
	require.Equal(t, res, []string{"2"})

	s, err := h.Get("1")
	require.NoError(t, err)
	require.Equal(t, s, "2")
}

func Test_Pointer(t *testing.T) {
	s := make([]int, 9, 20)
	lens := *(*int)(unsafe.Pointer(uintptr(unsafe.Pointer(&s)) + uintptr(8)))
	fmt.Println(lens, len(s))

	mp := make(map[string]int)
	mp["qcrao"] = 100
	mp["stefno"] = 18

	count := **(**uint8)(unsafe.Pointer(uintptr(unsafe.Pointer(&mp)) + uintptr(16)))
	fmt.Println(count, len(mp)) // 2
}

func string2bytes(s string) []byte {
	stringHeader := (*reflect.StringHeader)(unsafe.Pointer(&s))

	result := reflect.SliceHeader{
		Data: stringHeader.Data,
		Len:  stringHeader.Len,
		Cap:  stringHeader.Len,
	}
	return *(*[]byte)(unsafe.Pointer(&result))
}

func TestHashX_Range(t *testing.T) {

	reComp := regexp.MustCompile("a.+")
	require.True(t, reComp.MatchString("abbs"))

	h := NewHashXSingle()
	h.Set("abbs", "abbs")
	h.Set("ppp", "ppp")
	h.Set("accs", "accs")

	result := h.Range(0, 3, "")
	require.Len(t, result, 3)

	result = h.Range(0, -1, "")
	require.Len(t, result, 3)

	result = h.Range(0, -1, "a.+")
	require.Len(t, result, 2)

	result = h.Range(1, -1, "a.+")
	require.Len(t, result, 1)
}
