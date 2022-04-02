package terminal

import (
	"fmt"
	"github.com/liushuochen/gotable"
	"github.com/liushuochen/gotable/table"
	"github.com/urfave/cli/v2"
	"math"
	"vdns/config"
	"vdns/lib/util/strs"
)

//goland:noinspection SpellCheckingInspection
func SearchCommand() *cli.Command {
	return &cli.Command{
		Name:    "search",
		Aliases: []string{"s"},
		Usage:   "Search vdns information",
		Subcommands: []*cli.Command{
			{
				Name:    "provider",
				Aliases: []string{"p"},
				Usage:   "Support providers",
				Flags: []cli.Flag{
					&cli.StringFlag{
						Name:  "path",
						Usage: "sava table to csv file",
					},
				},
				Action: providerAction(),
			},
			{
				Name:    "record",
				Aliases: []string{"r"},
				Usage:   "Support record types",
				Flags: []cli.Flag{
					&cli.StringFlag{
						Name:  "path",
						Usage: "sava table to csv file",
					},
				},
				Action: recordAction(),
			},
			{
				Name:  `cat`,
				Usage: "Print common Request APIs",
				Flags: []cli.Flag{
					&cli.StringFlag{
						Name:  "path",
						Usage: "sava table to csv file",
					},
				},
				Action: catAction(),
			},
		},
	}
}

func recordAction() func(ctx *cli.Context) error {
	return func(ctx *cli.Context) error {
		fmt.Println("Supports record types: A、AAAA、NS、MX、CNAME、TXT、SRV、CA、REDIRECT_URL、FORWARD_URL")
		table, err := gotable.Create("type", "value", "description")
		if err != nil {
			return err
		}
		_ = table.AddRow([]string{"A", "A", "将域名指向一个IPV4地址"})
		_ = table.AddRow([]string{"AAAA", "AAAA", "将域名指向一个IPV6地址"})
		_ = table.AddRow([]string{"NS", "NS", "将子域名指定其他DNS服务器解析"})
		_ = table.AddRow([]string{"MX", "MX", "将域名指向邮件服务器地址"})
		_ = table.AddRow([]string{"CNAME", "CNAME", "将域名指向另外一个域名"})
		_ = table.AddRow([]string{"TXT", "TXT", "文本长度限制512，通常做SPF记录（反垃圾邮件）"})
		_ = table.AddRow([]string{"SRV", "SRV", "记录提供特定的服务的服务器"})
		_ = table.AddRow([]string{"CA", "CA", "CA证书颁发机构授权校验"})
		_ = table.AddRow([]string{"REDIRECT_URL", "REDIRECT_URL", "将域名重定向到另外一个地址"})
		_ = table.AddRow([]string{"FORWARD_URL", "FORWARD_URL", "显性URL类似，但是会隐藏真实目标地址"})

		fmt.Print(table)
		fmt.Println("Reference: https://help.aliyun.com/document_detail/29805.html?spm=a2c4g.11186623.0.0.30e73067AXxwak")
		path := ctx.String("path")
		err = toCSVFile(table, path)
		if err != nil {
			return err
		}
		return nil
	}
}

func providerAction() func(*cli.Context) error {
	return func(ctx *cli.Context) error {
		table, err := gotable.Create("provider", "DNS API Document")
		if err != nil {
			return err
		}
		_ = table.AddRow([]string{"AliDNS", "https://help.aliyun.com/document_detail/39863.html"})
		_ = table.AddRow([]string{"DNSPod", "https://cloud.tencent.com/document/product/1427"})
		_ = table.AddRow([]string{"Cloudflare", "https://api.cloudflare.com/#dns-records-for-a-zone-properties"})
		_ = table.AddRow([]string{"HuaweiDNS", "https://support.huaweicloud.com/function-dns/index.html"})

		fmt.Print(table)
		path := ctx.String("path")
		err = toCSVFile(table, path)
		if err != nil {
			return err
		}
		return nil
	}
}

func catAction() func(ctx *cli.Context) error {
	return func(ctx *cli.Context) error {
		table, err := gotable.Create("Ipv4 Request API", "Ipv6 Request API")
		if err != nil {
			return err
		}
		ipv4ApiList := config.GetIpv4ApiList()
		ipv6ApiList := config.GetIpv6ApiList()
		abs := int(math.Abs(float64(len(ipv4ApiList) - len(ipv6ApiList))))
		max := int(math.Max(float64(len(ipv4ApiList)), float64(len(ipv6ApiList))))
		if len(ipv4ApiList) > len(ipv6ApiList) {
			for i := 0; i < abs; i++ {
				ipv6ApiList = append(ipv6ApiList, "")
			}
		} else {
			for i := 0; i < abs; i++ {
				ipv4ApiList = append(ipv4ApiList, "")
			}
		}
		for i := 0; i < max; i++ {
			_ = table.AddRow([]string{ipv4ApiList[i], ipv6ApiList[i]})
		}

		fmt.Print(table)
		path := ctx.String("path")
		err = toCSVFile(table, path)
		if err != nil {
			return err
		}
		return nil
	}
}

func toCSVFile(table *table.Table, path string) error {
	if !strs.IsEmpty(path) {
		err := table.ToCSVFile(path)
		if err != nil {
			return err
		}
		fmt.Printf("sava to: %s\n", path)
	}
	return nil
}

func toJsonFile(table *table.Table, path string) error {
	if !strs.IsEmpty(path) {
		err := table.ToJsonFile(path, 2)
		if err != nil {
			return err
		}
		fmt.Printf("\nsava to: %s\n", path)
	}
	return nil
}
