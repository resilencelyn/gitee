package stringx

import (
	"gitee.com/wheat-os/wheatCache/pkg/structure"
)

type StringSingle struct {
	val *structure.Value
}

func NewStringSingle() structure.StringXInterface {
	return &StringSingle{
		val: structure.NewValue(),
	}
}

func (s *StringSingle) SizeByte() int64 {
	return int64(s.val.GetSize())
}

// RollBack TODO 事务相关, V2 实现
func (s *StringSingle) RollBack() error {
	panic("not implemented") // TODO: Implement
}

// Begin 事务相关, V2 实现
func (s *StringSingle) Begin() error {
	panic("not implemented") // TODO: Implement
}

// Comment 事务相关, V2 实现
func (s *StringSingle) Comment() error {
	panic("not implemented") // TODO: Implement
}

func (s *StringSingle) Encode() ([]byte, error) {
	panic("not implemented") // TODO: Implement
}

func (s *StringSingle) Set(val string) (string, structure.UpdateLength) {
	length := s.val.ChangeValueLength(func() {
		s.val.InferValue(val)
	})
	return val, length
}

func (s *StringSingle) Get() string {
	return s.val.ToString()
}

func (s *StringSingle) Add(renewal int32) (string, error) {
	result, err := s.val.Incr(renewal)
	if err != nil {
		return "", err
	}
	return result, nil
}

func (s *StringSingle) Reduce(renewal int32) (string, error) {
	result, err := s.val.Incr(-1 * renewal)
	if err != nil {
		return "", err
	}
	return result, nil
}

func (s *StringSingle) Setbit(offer int32, val bool) structure.UpdateLength {
	length := s.val.ChangeValueLength(func() {
		s.val.SetByte(int(offer), val)
	})
	return length
}

func (s *StringSingle) Getbit(offer int32) (bool, error) {
	b, err := s.val.GetByte(int(offer))
	if err != nil {
		return false, err
	}

	return b, err
}

func (s *StringSingle) Getrange(start, end int32) (string, error) {
	b, err := s.val.SliceByString(int(start), int(end))
	if err != nil {
		return "", err
	}
	return string(b), nil
}

func (s *StringSingle) GetLength() int {
	return s.val.GetLength()
}
