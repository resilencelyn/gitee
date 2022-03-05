package service

import (
	"bufio"
	"io"
	"os"
	"time"

	"gitee.com/wheat-os/wheatCache/pkg/event"
	"gitee.com/wheat-os/wheatCache/pkg/event2"
	"gitee.com/wheat-os/wheatCache/pkg/logx"
	"gitee.com/wheat-os/wheatCache/pkg/lru"
	"gitee.com/wheat-os/wheatCache/pkg/middle"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/storage/dao"
	"gitee.com/wheat-os/wheatCache/storage/persisted"
	"github.com/spf13/viper"
)

type singleService struct {
	middleProduce event.ProduceInterface
	lruProduce    event2.ProduceInterface
	timeOut       time.Duration
	lruCache      *lru.SingleCache
	dao           dao.Interface
	aof           *persisted.AOF
}

func loadAOF() *persisted.AOF {
	decName := viper.GetString("storage.aof-codec")
	var aofCodec persisted.AOFCodec
	switch decName {
	case "":
		return nil
	case "b16":
		aofCodec = nil
	default:
		aofCodec = nil
	}

	aofPath := viper.GetString("storage.aof-path")
	aofFlush := viper.GetInt("storage.aof-flush-time")
	aofCheckTime := viper.GetInt("storage.aof-check-time")
	aofCheckFreq := viper.GetInt("storage.aof-check-freq")

	return persisted.NewAOF(aofCodec, aofPath, time.Second*time.Duration(aofFlush),
		aofCheckFreq, time.Second*time.Duration(aofCheckTime))
}

// 尝试使用 aof 进行恢复
func recoverByAOF(d dao.Interface, aof *persisted.AOF) {
	if aof == nil || d == nil {
		return
	}

	path := aof.GetAOFLogPath()
	f, err := os.Open(path)
	if err != nil {
		return
	}
	defer f.Close()

	rd := bufio.NewReader(f)
	for {
		std, err := rd.ReadBytes('\n')
		if err != nil {
			if err != io.EOF {
				logx.Errorln(err)
			}
			break
		}

		message, err := aof.DecodeAOF(std[:len(std)-1])
		if err != nil {
			logx.Errorln(err)
			return
		}

		// 执行 恢复
		err = d.ExecMessage(message)
		if err != nil {
			logx.Errorln(err)
			return
		}
	}
}

func NewSingleServer() proto.CommServerServer {
	oneSingleServer.Do(func() {
		timeOut := viper.GetInt("storage.timeOut")
		if timeOut == 0 {
			timeOut = timeOutDefault
		}

		lruCache := lru.NewLRUCache()
		dao := dao.NewDao(lruCache)

		// 加载 aof
		aof := loadAOF()
		recoverByAOF(dao, aof)

		ser := &singleService{
			lruCache:      lruCache,
			lruProduce:    event2.NewProduce(lruCache.GetDriver()),
			timeOut:       time.Duration(timeOut) * time.Second,
			dao:           dao,
			middleProduce: event.NewProduce(middle.NewMiddleWare().GetEventDriver()),
			aof:           aof,
		}
		sysSingleService = ser

	})
	return sysSingleService
}
