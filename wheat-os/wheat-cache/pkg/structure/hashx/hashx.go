package hashx

import (
	"regexp"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/structure"
)

type HashX map[string]*structure.Value

func NewHashXSingle() structure.HashXInterface {
	return make(HashX)
}

func (h HashX) SizeByte() int64 {
	var size int
	for _, val := range h {
		size += val.GetSize()
	}
	return int64(size)
}

// RollBack TODO 事务相关, V2 实现
func (h HashX) RollBack() error {
	panic("not implemented") // TODO: Implement
}

// Begin 事务相关, V2 实现
func (h HashX) Begin() error {
	panic("not implemented") // TODO: Implement
}

// Comment 事务相关, V2 实现
func (h HashX) Comment() error {
	panic("not implemented") // TODO: Implement
}

func (h HashX) Encode() ([]byte, error) {
	panic("not implemented") // TODO: Implement
}

func (h HashX) Set(key string, val string) structure.UpdateLength {

	var Length structure.UpdateLength
	if v, ok := h[key]; ok {
		Length -= structure.UpdateLength(v.GetSize())
	}

	strVal := structure.NewValue(val)
	h[key] = strVal
	return Length + structure.UpdateLength(strVal.GetSize())
}

func (h HashX) Get(key string) (string, error) {
	if v, ok := h[key]; ok {
		return v.ToString(), nil
	}

	return "", errorx.New("this key does not exist in hashx, key:%s", key)
}

func (h HashX) Del(key string) (structure.UpdateLength, error) {
	if v, ok := h[key]; ok {
		delete(h, key)
		return structure.UpdateLength(v.GetSize()), nil
	}

	return 0, errorx.New("this key does not exist in hashx, key:%s", key)
}

func (h HashX) Key() []string {
	result := make([]string, 0, len(h))
	for key := range h {
		result = append(result, key)
	}
	return result
}

func (h HashX) Value() []string {
	result := make([]string, 0, len(h))
	for _, val := range h {
		result = append(result, val.ToString())
	}
	return result
}

func (h HashX) Item() map[string]string {
	result := make(map[string]string, len(h))
	for key, val := range h {
		result[key] = val.ToString()
	}
	return result
}

func (h HashX) Add(renewal int, keys ...string) (count int, result []string, err error) {
	for _, key := range keys {
		if v, ok := h[key]; ok {
			res, err := v.Incr(int32(renewal))
			if err != nil {
				continue
			}

			count += 1
			result = append(result, res)
		}
	}
	return count, result, nil
}

func (h HashX) SetX(key string, val string) (bool, structure.UpdateLength) {
	if _, ok := h[key]; ok {
		return false, 0
	}
	strVal := structure.NewValue(val)
	h[key] = strVal
	return true, structure.UpdateLength(strVal.GetSize())
}

func (h HashX) Length() int {
	return len(h)
}

func (h HashX) Range(consur, count int, regex string) []string {

	var reComp *regexp.Regexp
	if regex == "" {
		reComp = nil
	} else {
		reComp = regexp.MustCompile(regex)
	}

	result := make([]string, 0)
	for _, val := range h {
		if consur > 0 {
			consur--
			continue
		}

		if count == 0 && count != -1 {
			break
		}

		s := val.ToString()
		if reComp == nil {
			count--
			result = append(result, s)
			continue
		}
		if reComp.MatchString(s) {
			count--
			result = append(result, s)
		}
	}

	return result
}
