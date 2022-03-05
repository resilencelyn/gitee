package persisted

import (
	"bytes"
	"os"
	"sync/atomic"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
	"gitee.com/wheat-os/wheatCache/pkg/logx"
	protobuf "google.golang.org/protobuf/proto"
)

type AOF struct {
	codec       AOFCodec
	byteIO      chan []byte
	logPath     string
	ioFrequency int32
	flushTime   time.Duration
	checkIOTime time.Duration
	ioNum       *int32
}

// NewAOF
// codec AOF 的编码解码器，nil 时默认 protoCodec
// logPath AOF 持久化的地址
// flushTime 刷新到磁盘的间隔时间
// frequency, checkIOTime 使用一定频率检查，是否需要提前执行磁盘刷新
func NewAOF(codec AOFCodec, logPath string, flushTime time.Duration, frequency int, checkIOTime time.Duration) *AOF {
	oneAOF.Do(func() {
		if codec == nil {
			codec = &codecProto{}
		}
		sysAOF = &AOF{
			codec:       codec,
			ioFrequency: int32(frequency),
			flushTime:   flushTime,
			logPath:     logPath,
			byteIO:      make(chan []byte, defaultByteIONum),
			ioNum:       new(int32), // 统计当前的变更情况
			checkIOTime: checkIOTime,
		}
		go sysAOF.flushLog()
	})

	return sysAOF
}

// 刷新数据到磁盘
func (a *AOF) flushLog() {
	flushTc := time.NewTicker(a.flushTime)
	frequencyTc := time.NewTicker(a.checkIOTime)

	file, err := os.OpenFile(a.logPath, os.O_APPEND|os.O_WRONLY|os.O_CREATE, 0666)
	if err != nil {
		logx.Errorln(err)
		return
	}

	flush := func() {
		num := len(a.byteIO)
		for i := 0; i < num; i++ {
			aof := <-a.byteIO
			buffer := bytes.NewBuffer(aof)
			buffer.WriteString("\n")
			file.Write(buffer.Bytes())
		}

		file.Sync()
	}

	for {
		select {
		case <-flushTc.C:
			flush()
			atomic.SwapInt32(a.ioNum, 0)
		case <-frequencyTc.C:
			if num := atomic.LoadInt32(a.ioNum); num >= a.ioFrequency {
				flush()
			}
			atomic.SwapInt32(a.ioNum, 0)
		}
	}

}

func (a *AOF) DecodeAOF(buf []byte) (protobuf.Message, error) {
	return a.codec.Decode(buf)
}

func (a *AOF) EncodeAOF(method string, m protobuf.Message) ([]byte, error) {
	return a.codec.Encode(method, m)
}

func (a *AOF) SendRequest(method string, req interface{}) error {
	if req, ok := req.(protobuf.Message); ok {
		aof, err := a.EncodeAOF(method, req)
		if err != nil {
			return err
		}
		a.byteIO <- aof
		atomic.AddInt32(a.ioNum, 1)
	}
	return errorx.New("type not is protobuf.message")
}

func (a *AOF) GetAOFLogPath() string {
	return a.logPath
}
