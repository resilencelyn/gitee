package server

import (
	"os"
	"os/signal"
	"syscall"

	"gitee.com/wheat-os/wheatCache/pkg/logx"
	"google.golang.org/grpc"
)

func ElegantExitServer(s *grpc.Server) {
	c := make(chan os.Signal)
	signal.Notify(c, syscall.SIGHUP, syscall.SIGINT)
	go func() {
		select {
		case <-c:
			s.Stop()

			msg := `
				|-------Wheat tools---------|
				|    see you next time      |
				|thank you for your efforts |
				|---------------------------|
				`
			logx.Infoln(msg)
		}
	}()
}
