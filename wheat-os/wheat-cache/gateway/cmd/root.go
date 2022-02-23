package cmd

import (
	"fmt"
	"net"

	_ "gitee.com/wheat-os/wheatCache/conf"
	wheatCodec "gitee.com/wheat-os/wheatCache/gateway/codec"
	"gitee.com/wheat-os/wheatCache/gateway/endpoint"
	"gitee.com/wheat-os/wheatCache/gateway/proxy"
	"gitee.com/wheat-os/wheatCache/pkg/logx"
	"gitee.com/wheat-os/wheatCache/pkg/util/server"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"google.golang.org/grpc"
)

// rootCmd represents the base command when called without any subcommands
var rootCmd = &cobra.Command{
	Use:   "getway",
	Short: "getway",
	Long:  `start getway server`,
	Run: func(cmd *cobra.Command, args []string) {
		host := viper.GetString("gateway.host")
		port := viper.GetInt("gateway.port")

		tcpAddr, err := net.ResolveTCPAddr("tcp", fmt.Sprintf("%s:%d", host, port))
		if err != nil {
			logx.Panic("get gateway addr err:%v", err)
		}
		listen, err := net.ListenTCP("tcp", tcpAddr)
		if err != nil {
			logx.Panic("get gateway tcp conn err:%v", err)
		}

		gatewayServer := GetGatewayServer()
		server.ElegantExitServer(gatewayServer)

		logx.Info("start gateway in addr: %s", tcpAddr.String())
		if err := gatewayServer.Serve(listen); err != nil {
			logx.Errorln(err)
		}
	},
}

// Execute adds all child commands to the root command and sets flags appropriately.
// This is called by main.main(). It only needs to happen once to the rootCmd.
func Execute() {
	cobra.CheckErr(rootCmd.Execute())
}

func GetGatewayServer() *grpc.Server {

	targets := viper.GetStringSlice("gateway.target")

	logx.Debug("service target in %v", targets)

	stream := proxy.GetDirectorByServiceHash()
	endpoint := endpoint.NewHashEndpoint(endpoint.HashReplicasDefault, nil, targets...)

	opts := make([]grpc.ServerOption, 0)
	opts = append(
		opts,
		grpc.ForceServerCodec(wheatCodec.Codec()),
		grpc.UnknownServiceHandler(proxy.TransparentHandler(stream, endpoint)),
	)

	return grpc.NewServer(opts...)
}
