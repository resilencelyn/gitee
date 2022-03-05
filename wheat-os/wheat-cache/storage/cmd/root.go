// Package cmd /*
package cmd

import (
	"fmt"
	"log"
	"net"

	"gitee.com/wheat-os/wheatCache/storage/service"

	_ "gitee.com/wheat-os/wheatCache/conf"
	"gitee.com/wheat-os/wheatCache/pkg/logx"
	"gitee.com/wheat-os/wheatCache/pkg/proto"
	"gitee.com/wheat-os/wheatCache/pkg/util/server"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"google.golang.org/grpc"
	"google.golang.org/grpc/reflection"
)

// rootCmd represents the base command when called without any subcommands
var rootCmd = &cobra.Command{
	Use:   "storage",
	Short: "storage",
	Long:  `start storage server`,
	Run: func(cmd *cobra.Command, args []string) {
		storageServer := service.NewSingleServer()
		conf := viper.GetStringMap("storage")
		host := conf["host"].(string)
		port := conf["port"].(int)

		tcpAddr, err := net.ResolveTCPAddr("tcp", fmt.Sprintf("%s:%d", host, port))
		if err != nil {
			log.Fatalln(err)
		}

		listen, err := net.ListenTCP("tcp", tcpAddr)
		if err != nil {
			logx.Panicln(err)
		}
		s := grpc.NewServer()
		proto.RegisterCommServerServer(s, storageServer)
		reflection.Register(s)

		server.ElegantExitServer(s)

		logx.Info("start storage in addr: %s", tcpAddr.String())
		if err := s.Serve(listen); err != nil {
			logx.Errorln(err)
		}
	},
}

// Execute adds all child commands to the root command and sets flags appropriately.
// This is called by main.main(). It only needs to happen once to the rootCmd.
func Execute() {
	cobra.CheckErr(rootCmd.Execute())
}
