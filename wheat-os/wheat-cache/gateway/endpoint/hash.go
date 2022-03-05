package endpoint

import (
	"hash/crc32"
	"sort"
	"strconv"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
)

type HashFunc func(data []byte) uint32

// 实现 sort
type UInt32Slice []uint32

func (s UInt32Slice) Len() int {
	return len(s)
}

func (s UInt32Slice) Less(i, j int) bool {
	return s[i] < s[j]
}

func (s UInt32Slice) Swap(i, j int) {
	s[i], s[j] = s[j], s[i]
}

type HashEndpoint struct {
	hash     HashFunc
	replicas int // 复制因子
	keys     UInt32Slice
	hashMap  map[uint32]string // taraget 隐射
}

func NewHashEndpoint(replicas int, fn HashFunc, target ...string) EndpointInterface {
	endpoint := &HashEndpoint{
		replicas: replicas,
		hash:     fn,
		hashMap:  make(map[uint32]string, len(target)),
	}

	if endpoint.hash == nil {
		endpoint.hash = crc32.ChecksumIEEE // 默认使用 CRC32 算法
	}

	endpoint.AddTarget(target...)

	return endpoint
}

func (h *HashEndpoint) IsEmpty() bool {
	return len(h.keys) == 0
}

func (h *HashEndpoint) AddTarget(targets ...string) {
	for _, tar := range targets {

		for i := 0; i < h.replicas; i++ {
			hash := h.hash([]byte(strconv.Itoa(i) + tar))
			h.keys = append(h.keys, hash)
			h.hashMap[hash] = tar
		}
	}

	// 虚拟值排序，方便查找
	sort.Sort(h.keys)
}

func (h *HashEndpoint) GetTargetAddr(str ...string) (string, error) {
	if h.IsEmpty() {
		return "", errorx.New("gateway not register transport")
	}

	if len(str) != 1 {
		return "", errorx.New("must give key")

	}
	hash := h.hash([]byte(str[0]))
	idx := sort.Search(len(h.keys), func(i int) bool { return h.keys[i] >= hash })
	if idx == len(h.keys) {
		return h.hashMap[h.keys[0]], nil
	}

	return h.hashMap[h.keys[idx]], nil
}
