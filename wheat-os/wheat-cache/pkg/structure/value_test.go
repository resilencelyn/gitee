package structure

import (
	"fmt"
	"strconv"
	"testing"

	"github.com/stretchr/testify/require"
)

func TestValue_SetString(t *testing.T) {
	value := Value{
		val: make([]byte, 32, 64),
	}
	testStr := "awjkbk沙袋jdawd"
	value.SetString(testStr)

	require.Equal(t, testStr, value.ToString())
}

func TestValue_GetString(t *testing.T) {
	value := NewValue()
	str := ""
	for i := 0; i < 100; i++ {
		str += strconv.Itoa(i)
	}
	value.SetString(str)
	require.Equal(t, value.ToString(), str)

	value.SetString("aw1")
	require.Equal(t, value.ToString(), "aw1")
}

func Test_IntToBytes(t *testing.T) {
	value := NewValue()
	value.SetInt(2<<62 - 1)
	require.Equal(t, value.GetLength(), 8)
}

func TestValue_ToInt(t *testing.T) {
	value := NewValue()
	value.SetInt(90)
	rel, err := value.ToInt()
	require.NoError(t, err)
	require.Equal(t, rel, int64(90))
	value.SetString("abc")
	require.Equal(t, value.ToString(), "abc")
	_, err = value.ToInt()
	require.Error(t, err)
}

func TestValue_SetFloat64(t *testing.T) {
	value := NewValue()
	value.SetFloat64(2.35)
	f, err := value.ToFloat64()
	require.NoError(t, err)
	require.Equal(t, f, 2.35)

	value.SetInt(-10)
	rel, err := value.ToInt()
	require.NoError(t, err)
	require.Equal(t, rel, int64(-10))

	value.SetFloat64(-2.35)
	f, err = value.ToFloat64()
	require.NoError(t, err)
	require.Equal(t, f, -2.35)
}

func TestValue_InferValue(t *testing.T) {
	value := NewValue()
	value.InferValue("hello world")
	require.Equal(t, value.GetDynamicType(), DynamicString)
	require.Equal(t, value.ToString(), "hello world")

	value.InferValue("1")
	require.Equal(t, value.GetDynamicType(), DynamicInt)
	i, err := value.ToInt()
	require.NoError(t, err)
	require.Equal(t, i, int64(1))

	value.InferValue("-1.2")
	require.Equal(t, value.GetDynamicType(), DynamicFloat)
	f, err := value.ToFloat64()
	require.NoError(t, err)
	require.Equal(t, f, -1.2)
}

func TestValue_ChangeValue(t *testing.T) {
	value := NewValue()
	oldLen := value.GetLength()
	chanageLen := value.ChangeValueLength(func() {
		value.SetString("小葵花课堂开课了")
	})
	require.Equal(t, int64(chanageLen), int64(value.GetLength()-oldLen))

	lens := value.ChangeValueLength(func() {
		value.SetInt(100)
	})
	require.Equal(t, lens, int64(-16))

	lengs := value.ChangeValueLength(func() {
		value.SetFloat64(9.99)
	})
	require.Equal(t, lengs, int64(0))

	value = NewValue()
	lengs = value.ChangeValueLength(func() {
		value.SetFloat64(10.99)
	})
	require.Equal(t, lengs, int64(0))

	value = NewValue()
	value.SetString("909awdawdawd")
	lengs = value.ChangeValueLength(func() {
		value.SetString("9akjwbddddddddddddddddddddddddddddddddddddddddddddd")
	})
	require.Equal(t, lengs, int64(39))
}

func TestValue_SetByte(t *testing.T) {
	value := NewValue()
	value.SetByte(10001, true)
	v, err := value.GetByte(123)
	require.NoError(t, err)
	require.Equal(t, v, false)
	v, err = value.GetByte(10001)
	require.NoError(t, err)
	require.Equal(t, v, true)
	require.Equal(t, value.GetSize(), (10001/8)+1+16)
}

func TestValue_SetByteWei(t *testing.T) {
	k := make([]byte, 100)
	offset := 700
	k[offset/8] = 0b00000001
	k[offset/8] |= 0b1 << (offset % 8)

	fmt.Printf("%b\n", k[offset/8])

	fmt.Printf("%v", (k[offset/8]&(0b1<<(offset%8))) != 0)

	k[offset/8] ^= 0b1 << (offset % 8)

	fmt.Printf("%v", (k[offset/8]&(0b1<<(offset%8))) != 0)
}
