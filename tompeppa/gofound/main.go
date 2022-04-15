package main

import (
	"flag"
	"fmt"
	"github.com/gin-gonic/gin"
	"gofound/router/api"
	"gofound/searcher"
	"log"
	"os"
	"os/exec"
)

func main() {

	var addr string
	flag.StringVar(&addr, "addr", "127.0.0.1:5678", "设置监听地址和端口")

	var dataDir string

	//兼容windows
	dir := fmt.Sprintf(".%sdata", string(os.PathSeparator))

	flag.StringVar(&dataDir, "data", dir, "设置数据存储目录")

	var debug bool
	flag.BoolVar(&debug, "debug", false, "设置是否开启调试模式")

	var daemon = flag.Bool("daemon", false, "后台模式")
	if *daemon {
		cmd := exec.Command(os.Args[0], os.Args[1:]...)
		cmd.Stdin = os.Stdin
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr
		err := cmd.Start()
		if err != nil {
			fmt.Fprintf(os.Stderr, "[-] Error: %s\n", err)
		}

		os.Exit(0)
		return
	}

	flag.Parse()

	if debug {
		gin.SetMode(gin.DebugMode)
	} else {
		gin.SetMode(gin.ReleaseMode)
	}

	router := gin.Default()
	//处理异常
	router.Use(api.Recover)
	router.SetTrustedProxies(nil)

	//注册api
	api.Register(router)

	var Engine = &searcher.Engine{
		IndexPath: dataDir,
	}
	option := Engine.GetOptions()

	go Engine.InitOption(option)
	//保存索引到磁盘
	defer Engine.FlushIndex()
	api.SetEngine(Engine)

	log.Println("API url： \t http://" + addr + "/api")

	err := router.Run(addr)
	defer func() {

		if r := recover(); r != nil {

			fmt.Printf("panic: %s\n", r)

		}

		fmt.Println("-- 2 --")

	}()
	fmt.Println("-- 1 --")
	if err != nil {
		fmt.Println("错误", err)
		return
	}
}
