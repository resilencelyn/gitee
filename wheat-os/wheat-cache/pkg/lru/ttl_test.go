package lru

import (
	"fmt"
	"testing"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/structure/stringx"
	"github.com/stretchr/testify/require"
)

func TestTTlCup(t *testing.T) {
	k := make([]string, 100, 3000)
	fmt.Println(cap(k))
	p := k[:50]
	fmt.Println(cap(p))
}

func Test_LruTTl(t *testing.T) {
	lru := NewLRUCache()
	s := stringx.NewStringSingle()
	lru.Add(&proto.BaseKey{
		Key: "k8s",
		Ttl: 1,
	}, s)
	lru.Add(&proto.BaseKey{
		Key: "990",
		Ttl: 10,
	}, s)
	require.Equal(t, lru.nowSize, int64(48))

	time.Sleep(4 * time.Second)
	require.Equal(t, lru.nowSize, int64(24))

}
