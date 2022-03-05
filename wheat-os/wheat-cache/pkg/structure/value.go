package structure

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"math"
	"strconv"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
)

// Value 提供一个基础的 动态类型

type Value struct {
	val    []byte
	length int
	onType DynamicType
}

func NewValue(val ...string) *Value {

	stcValue := &Value{
		val:    make([]byte, defaultLen),
		length: 0,
		onType: DynamicNull,
	}

	if len(val) > 0 {
		stcValue.InferValue(val[0])
	}
	return stcValue
}

func (v *Value) GetLength() int {
	return v.length
}

func (v *Value) GetSize() int {
	return len(v.val) + 16
}

func (v *Value) GetDynamicType() DynamicType {
	return v.onType
}

func (v *Value) SetString(str string) {
	v.onType = DynamicString
	if len(v.val) >= len(str) {
		copy(v.val, str)
		v.length = len(str)
		v.val = v.val[:v.length]
		return
	}

	// 超过 cap
	if len(str) > cap(v.val) {
		v.val = make([]byte, len(str))
		copy(v.val, str)
		v.length = len(str)
		return
	}

	// 在 cap 以内
	copy(v.val, str[:v.length])
	v.val = append(v.val, str[v.length:]...)
}

func (v *Value) ToString() string {
	switch v.onType {
	case DynamicNull:
		return ""
	case DynamicString:
		return string(v.val[:v.length])
	case DynamicInt:
		i, _ := v.ToInt()
		return strconv.FormatInt(i, 10)
	case DynamicFloat:
		f, _ := v.ToFloat64()
		return strconv.FormatFloat(f, 'f', 2, 64)
	}
	return string(v.val[:v.length])
}

// SetInt 使用高位存储
func (v *Value) SetInt(i int64) {
	byteBuf := bytes.NewBuffer([]byte{})
	binary.Write(byteBuf, binary.BigEndian, i)
	v.val = byteBuf.Bytes()
	v.length = len(v.val)
	v.onType = DynamicInt
}

func (v *Value) ToInt() (reI int64, err error) {
	if v.onType != DynamicInt {
		return 0, errorx.New("can only be resolved from dynamic int")
	}

	byteBuff := bytes.NewBuffer(v.val)
	err = binary.Read(byteBuff, binary.BigEndian, &reI)
	if err != nil {
		return 0, err
	}

	return reI, nil
}

func (v *Value) SetFloat64(f float64) {
	bits := math.Float64bits(f)
	v.length = 8
	binary.LittleEndian.PutUint64(v.val[:v.length], bits)
	v.val = v.val[:v.length]
	v.onType = DynamicFloat
}

func (v *Value) ToFloat64() (float64, error) {
	if v.onType != DynamicFloat {
		return 0, errorx.New("can only be resolved from dynamic float")
	}

	bits := binary.LittleEndian.Uint64(v.val[:v.length])
	return math.Float64frombits(bits), nil
}

// InferValue 通过字符串来自动推导类型，性能较低
func (v *Value) InferValue(str string) {
	rInt, err := strconv.Atoi(str)
	if err == nil {
		v.SetInt(int64(rInt))
		return
	}

	rf, err := strconv.ParseFloat(str, 64)
	if err == nil {
		v.SetFloat64(rf)
		return
	}

	v.SetString(str)
}

// ChangeValueLength 根据类型推断 change 的大小, 只用于 Set 操作不发生错误
func (v *Value) ChangeValueLength(f func()) UpdateLength {
	startLen := v.GetLength()
	f()
	return UpdateLength(v.GetLength() - startLen)
}

func (v *Value) SetByte(offset int, val bool) {
	v.onType = DynamicNull // 位图使用无类型

	// 扩容
	if len(v.val) <= offset/8 {
		newByte := make([]byte, (offset/8)+1)
		copy(newByte, v.val[:len(v.val)])
		v.val = newByte
		v.length = len(v.val)
	}
	if val {
		// true 位
		v.val[offset/8] |= (0b1 << (offset % 8))
		return
	}

	// false 位
	v.val[offset/8] ^= (0b1 << (offset % 8))
}

func (v *Value) GetByte(offset int) (bool, error) {
	if len(v.val) >= offset/8 {
		// 采用 & 来运算 是否为 true
		return v.val[offset/8]&(0b1<<(offset%8)) != 0, nil
	}

	return false, errorx.New("the maximum length is exceeded")
}

func (v *Value) SliceByString(start, end int) ([]byte, error) {
	if start > end {
		return nil, errorx.New("the end cannot be greater than the beginning")
	}

	if v.onType == DynamicInt {
		ret, err := v.ToInt()
		if err != nil {
			return nil, err
		}
		value := strconv.Itoa(int(ret))
		if end > len(value) {
			return nil, errorx.New("the maximum index is exceeded, max index: %d", len(value))
		}
		return []byte(value[start:end]), nil
	}

	if v.onType == DynamicFloat {
		ret, err := v.ToFloat64()
		if err != nil {
			return nil, err
		}
		value := fmt.Sprintf("%.2f", ret)
		if end > len(value) {
			return nil, errorx.New("the maximum index is exceeded, max index: %d", len(value))
		}
		return []byte(value[start:end]), nil
	}

	if end > v.length {
		return nil, errorx.New("the maximum index is exceeded, max index: %d", v.length)
	}

	return v.val[start:end], nil
}

// 自增
func (v *Value) Incr(renewal int32) (string, error) {
	switch v.GetDynamicType() {
	case DynamicNull:
		v.SetInt(int64(renewal))
		return strconv.Itoa(int(renewal)), nil
	case DynamicFloat:
		f, err := v.ToFloat64()
		if err != nil {
			return "", err
		}
		v.SetFloat64(f + float64(renewal))
		return strconv.FormatFloat(f+float64(renewal), 'f', 2, 64), nil
	case DynamicInt:
		i, err := v.ToInt()
		if err != nil {
			return "", err
		}
		v.SetInt(int64(renewal) + i)
		return strconv.Itoa(int(i + int64(renewal))), nil
	default:
		return "", errorx.New("string cannot perform add operations")
	}
}
