package persisted

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"strconv"
	"testing"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"github.com/stretchr/testify/require"
)

func TestAOF_SendRequest(t *testing.T) {
	aof := NewAOF(nil, "./test.aof", time.Second*3, 5, time.Second*1)
	for i := 0; i < 100; i++ {
		req := &proto.ReduceRequest{
			Key:     proto.NewBaseKey(strconv.Itoa(i), int64(i)),
			Renewal: int32(i),
		}

		aof.SendRequest("Reduce", req)
	}

	time.Sleep(4 * time.Second)
	fmt.Println(len(aof.byteIO))
}

func TestAOF_ReadRequest(t *testing.T) {

	aof := NewAOF(nil, "./test.aof", time.Second*2, 5, time.Second*1)
	f, _ := os.Open("test.aof")
	defer f.Close()

	rd := bufio.NewReader(f)
	for {
		line, err := rd.ReadBytes('\n')

		if err != nil {
			if err != io.EOF {
				require.NoError(t, err)
			}
			break
		}
		msg, err := aof.DecodeAOF(line[:len(line)-1])
		require.NoError(t, err)
		fmt.Println(msg)
	}
}
