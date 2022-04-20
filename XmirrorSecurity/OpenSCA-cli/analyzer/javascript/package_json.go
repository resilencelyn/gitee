/*
 * @Description: 解析package.json文件
 * @Date: 2022-01-07 17:00:41
 */

package javascript

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"sort"
	"util/bar"
	"util/cache"
	"util/enum/language"
	"util/logs"
	"util/model"

	"github.com/Masterminds/semver/v3"
)

// package.json 文件结构
type PkgJson struct {
	Name    string            `json:"name"`
	Version string            `json:"version"`
	License string            `json:"license"`
	DevDeps map[string]string `json:"devDependencies"`
	Deps    map[string]string `json:"dependencies"`
}

// npm下载文件结构
type NpmJson struct {
	Time     map[string]string  `json:"time"`
	Versions map[string]PkgJson `json:"versions"`
}

// parsePackage 解析package.json
func parsePackage(depRoot *model.DepTree, file *model.FileData) (deps []*model.DepTree) {
	deps = []*model.DepTree{}
	pkg := PkgJson{}
	if err := json.Unmarshal(file.Data, &pkg); err != nil {
		logs.Error(err)
		return
	}
	pkgDep := depRoot
	pkgDep.Version = model.NewVersion(pkg.Version)
	pkgDep.AddLicense(pkg.License)
	if pkg.Name != "" {
		pkgDep.Name = pkg.Name
		deps = append(deps, pkgDep)
	}
	// 依赖列表map[name]version
	depMap := map[string]string{}
	for name, version := range pkg.DevDeps {
		depMap[name] = version
	}
	for name, version := range pkg.Deps {
		depMap[name] = version
	}
	// 组件名排序后添加到deps
	names := []string{}
	for name := range depMap {
		names = append(names, name)
	}
	sort.Strings(names)
	for _, name := range names {
		version := depMap[name]
		dep := model.NewDepTree(pkgDep)
		dep.Name = name
		dep.Version = model.NewVersion(version)
		if pkg.Name == "" {
			deps = append(deps, dep)
		}
	}
	// 记录出现过的组件
	exist := map[string]struct{}{}
	// 搜索子依赖
	q := model.NewQueue()
	exist[pkgDep.Name] = struct{}{}
	for _, child := range pkgDep.Children {
		exist[child.Name] = struct{}{}
		q.Push(child)
	}
	for !q.Empty() {
		node := q.Pop().(*model.DepTree)
		for _, sub := range npmSimulation(node) {
			if _, ok := exist[sub.Name]; !ok {
				bar.Npm.Add(1)
				exist[sub.Name] = struct{}{}
				q.Push(sub)
			}
		}
	}
	return
}

// npmSimulation 模拟npm获取详细依赖信息
func npmSimulation(dep *model.DepTree) (subDeps []*model.DepTree) {
	subDeps = []*model.DepTree{}
	dep.Language = language.JavaScript
	// 获取依赖数据
	data := cache.LoadCache(dep.Dependency)
	if len(data) == 0 {
		url := fmt.Sprintf(`https://r.cnpmjs.org/%s`, dep.Name)
		if rep, err := http.Get(url); err != nil {
			logs.Error(err)
			return
		} else {
			if data, err = ioutil.ReadAll(rep.Body); err != nil {
				logs.Error(err)
			} else {
				cache.SaveCache(dep.Dependency, data)
			}
			rep.Body.Close()
		}
	}
	// 解析依赖信息
	npm := NpmJson{}
	json.Unmarshal(data, &npm)
	// 查找符合范围内的最大版本
	latestVersion := ""
	// 当前最大版本
	var max *semver.Version
	if dep.Version.Org == "" || dep.Version.Org == "latest" {
		dep.Version.Org = "x"
	}
	if constraint, err := semver.NewConstraint(dep.Version.Org); err == nil {
		for ver := range npm.Time {
			if v, err := semver.NewVersion(ver); err == nil {
				if constraint.Check(v) {
					if max == nil || max.LessThan(v) {
						max = v
						latestVersion = ver
					}
				}
			}
		}
	} else {
		logs.Warn(err)
	}
	if latestVersion == "" {
		return
	}
	info := npm.Versions[latestVersion]
	dep.Version = model.NewVersion(latestVersion)
	dep.AddLicense(info.License)
	// 解析子依赖
	names := []string{}
	for name := range info.Deps {
		names = append(names, name)
	}
	sort.Strings(names)
	for _, name := range names {
		sub := model.NewDepTree(dep)
		sub.Name = name
		sub.Version = model.NewVersion(info.Deps[name])
		subDeps = append(subDeps, sub)
	}
	return
}
