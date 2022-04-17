/*
 * @Descripation: 从本地漏洞库获取漏洞
 * @Date: 2021-12-08 16:31:45
 */

package vuln

import (
	"encoding/json"
	"io/ioutil"
	"opensca/internal/args"
	"opensca/internal/enum/language"
	"opensca/internal/logs"
	"opensca/internal/srt"
	"strings"
)

type vulnInfo struct {
	Vendor   string `json:"vendor"`
	Product  string `json:"product"`
	Version  string `json:"version"`
	Language string `json:"language"`
	*srt.Vuln
}

// 漏洞信息 map[language]map[name][]vulninfo
var vulnDB map[string]map[string][]vulnInfo

/**
 * @description: 加载本地漏洞
 */
func loadVulnDB() {
	vulnDB = map[string]map[string][]vulnInfo{}
	if args.VulnDB != "" {
		// 读取本地漏洞数据
		if data, err := ioutil.ReadFile(args.VulnDB); err != nil {
			logs.Error(err)
		} else {
			// 解析本地漏洞
			db := []vulnInfo{}
			json.Unmarshal(data, &db)
			for _, info := range db {
				// 有中文描述则省略英文描述
				if info.Description != "" {
					info.DescriptionEn = ""
				}
				// 将漏洞信息存到vulnDB中
				name := strings.ToLower(info.Product)
				language := strings.ToLower(info.Language)
				if _, ok := vulnDB[language]; !ok {
					vulnDB[language] = map[string][]vulnInfo{}
				}
				vulns := vulnDB[language]
				vulns[name] = append(vulns[name], info)
			}
		}
	}
}

/**
 * @description: 使用本地漏洞库获取漏洞
 * @param {[]srt.Dependency} deps 组件依赖信息列表
 * @return {[][]*srt.Vuln} 组件漏洞列表
 */
func GetLocalVulns(deps []srt.Dependency) (vulns [][]*srt.Vuln) {
	if vulnDB == nil {
		loadVulnDB()
	}
	vulns = make([][]*srt.Vuln, len(deps))
	for i, dep := range deps {
		vulns[i] = []*srt.Vuln{}
		if vs, ok := vulnDB[dep.Language.Vuln()][strings.ToLower(dep.Name)]; ok {
			for _, v := range vs {
				switch dep.Language {
				case language.Java:
					if !strings.EqualFold(v.Vendor, dep.Vendor) {
						continue
					}
				default:
				}
				// 在漏洞影响范围内
				if srt.InRangeInterval(dep.Version, v.Version) {
					vulns[i] = append(vulns[i], v.Vuln)
				}
			}
		}
	}
	return
}
