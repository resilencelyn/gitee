/*
 * @Description: parse go.mod/go.sum
 * @Date: 2022-02-10 16:16:24
 */

package golang

import (
	"regexp"
	"strings"
	"util/model"
)

// parseGomod parse go.mod
func parseGomod(dep *model.DepTree, file *model.FileData) (deps []*model.DepTree) {
	deps = []*model.DepTree{}
	for _, match := range regexp.MustCompile(`(\S*)\s+v([\d\w\-+.]*)[\s\n]`).FindAllStringSubmatch(string(file.Data), -1) {
		if len(match) != 3 {
			continue
		}
		sub := model.NewDepTree(dep)
		sub.Name = strings.Trim(match[1], `'"`)
		sub.Version = model.NewVersion(match[2])
		deps = append(deps, sub)
	}
	return deps
}

// parseGosum parse go.sum
func parseGosum(dep *model.DepTree, file *model.FileData) (deps []*model.DepTree) {
	deps = parseGomod(dep, file)
	exist := map[string]struct{}{}
	for _, dep := range deps {
		exist[dep.Name] = struct{}{}
	}
	for _, match := range regexp.MustCompile(`(\S*)\s+v([\d\w\-+.]*)/go.mod[\s\n]`).FindAllStringSubmatch(string(file.Data), -1) {
		if len(match) != 3 {
			continue
		}
		if _, ok := exist[match[1]]; ok {
			continue
		}
		sub := model.NewDepTree(dep)
		sub.Name = strings.Trim(match[1], `'"`)
		sub.Version = model.NewVersion(match[2])
		exist[sub.Name] = struct{}{}
		deps = append(deps, sub)
	}
	return deps
}
