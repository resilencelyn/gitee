package structure

const (
	defaultLen = 8 // 默认创建的 value 大小
)

type DynamicType int8

type UpdateLength int64

const (
	DynamicNull = DynamicType(iota)
	DynamicInt
	DynamicFloat
	DynamicString
)

type KeyBaseInterface interface {
	SizeByte() int64

	// RollBack TODO 事务相关, V2 实现
	RollBack() error
	// Begin 事务相关, V2 实现
	Begin() error
	// Comment 事务相关, V2 实现
	Comment() error

	Encode() ([]byte, error)
}

type StringXInterface interface {
	KeyBaseInterface
	Set(string) (string, UpdateLength)
	Get() string
	Add(int32) (string, error)
	Reduce(int32) (string, error)
	Setbit(int32, bool) UpdateLength
	Getbit(int32) (bool, error)
	Getrange(start, end int32) (string, error)
	GetLength() int
}

type ListXInterface interface {
	KeyBaseInterface
	LPush(...string) UpdateLength
	RPush(...string) UpdateLength
	LPop(int) ([]string, UpdateLength)
	RPop(int) ([]string, UpdateLength)
	Index(int) (string, error)
	Insert(int, bool, ...string) (UpdateLength, error)
	Length() int
	Slice(start, end int) (UpdateLength, error) // 切片， O(n)复杂度
	Range(start, end int) ([]string, error)
	Remove(value string, count int) (int, UpdateLength)
}

type HashXInterface interface {
	KeyBaseInterface
	Set(key string, val string) UpdateLength
	Get(key string) (string, error)
	Del(key string) (UpdateLength, error)
	Key() []string
	Value() []string
	Item() map[string]string
	Add(renewal int, key ...string) (int, []string, error) // 访问影响成功的结果
	SetX(key string, val string) (bool, UpdateLength)      // 不存在才插入
	Length() int
	Range(consur, count int, regex string) []string
}

type ChannelXInterface interface {
	KeyBaseInterface
	Push(value string) UpdateLength
	Pop() (string, UpdateLength)
	Length() int
	Clean() UpdateLength
}
