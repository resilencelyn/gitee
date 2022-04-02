package config

// provides
const (
	AlidnsProvider     = "alidns"
	DnspodProvider     = "dnspod"
	CloudflareProvider = "cloudflare"
	HuaweiDnsProvider  = "huaweidns"
)

// ipv4 api endpoint
//goland:noinspection GoUnusedConst
const (
	V4Api1 = "https://ddns.oray.com/checkip"
	V4Api2 = "https://api-ipv4.ip.sb/ip"
	V4Api3 = "https://api.ip.sb/ip"
	V4Api4 = "https://myip.ipip.net"
	V4Api5 = "https://api-ipv4.ip.sb/ip"
)

// ipv6 api endpoint
//goland:noinspection GoUnusedConst
const (
	V6Api1 = "https://api-ipv6.ip.sb/ip"
	V6Api2 = "https://v6.myip.la/json"
	V6Api3 = "https://speed.neu6.edu.cn/getIP.php"
)

func GetIpv4ApiList() []string {
	return []string{V4Api1, V4Api2, V4Api3, V4Api4, V4Api5}
}

func GetIpv6ApiList() []string {
	return []string{V6Api1, V6Api2, V6Api3}
}
