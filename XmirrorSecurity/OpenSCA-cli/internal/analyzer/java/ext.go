/*
 * @Descripation: mvn解析依赖树
 * @Date: 2021-12-16 10:10:13
 */

package java

import (
	"bytes"
	"fmt"
	"io/ioutil"
	"net/http"
	"opensca/internal/bar"
	"opensca/internal/cache"
	"opensca/internal/enum/language"
	"opensca/internal/logs"
	"opensca/internal/srt"
	"os"
	"os/exec"
	"path"
	"regexp"
	"strings"

	"github.com/pkg/errors"
)

/**
 * @description: 调用mvn解析项目获取依赖树
 * @param {string} path 项目目录
 * @return {*srt.DepTree} 项目根节点
 */
func MvnDepTree(path string) (root *srt.DepTree) {
	root = srt.NewDepTree(nil)
	pwd, err := os.Getwd()
	if err != nil {
		logs.Error(err)
		return
	}
	os.Chdir(path)
	cmd := exec.Command("mvn", "dependency:tree", "--fail-never")
	out, _ := cmd.CombinedOutput()
	os.Chdir(pwd)
	// 统一替换换行符为\n
	out = bytes.ReplaceAll(out, []byte("\r\n"), []byte("\n"))
	out = bytes.ReplaceAll(out, []byte("\n\r"), []byte("\n"))
	out = bytes.ReplaceAll(out, []byte("\r"), []byte("\n"))
	// 获取mvn解析内容
	lines := strings.Split(string(out), "\n")
	for i := range lines {
		lines[i] = strings.TrimPrefix(lines[i], "[INFO] ")
	}
	// 捕获依赖树起始位置
	title := regexp.MustCompile(`--- [^\n]+ ---`)
	// 记录依赖树起始位置行号
	start := 0
	// 标记是否在依赖范围内树
	tree := false
	// 获取mvn依赖树
	for i, line := range lines {
		if title.MatchString(line) {
			tree = true
			start = i
			continue
		}
		if tree && strings.Trim(line, "-") == "" {
			tree = false
			buildMvnDepTree(root, lines[start+1:i])
			continue
		}
	}
	return
}

/**
 * @description: 构建mvn树
 * @param {*srt.DepTree} root 依赖树根节点
 * @param {[]string} lines 依赖树信息
 */
func buildMvnDepTree(root *srt.DepTree, lines []string) {
	// 记录当前的顶点节点列表
	tops := []*srt.DepTree{root}
	// 上一层级
	lastLevel := -1
	for _, line := range lines {
		// 计算层级
		level := 0
		for line[level*3+2] == ' ' {
			level++
		}
		tops = tops[:len(tops)-lastLevel+level-1]
		root = tops[len(tops)-1]
		tags := strings.Split(line[level*3:], ":")
		if len(tags) < 4 {
			logs.Error(errors.New("mvn parse error"))
			break
		}
		dep := srt.NewDepTree(root)
		dep.Vendor = tags[0]
		dep.Name = tags[1]
		dep.Version = srt.NewVersion(tags[3])
		dep.Language = language.Java
		tops = append(tops, dep)
		lastLevel = level
	}
}

/**
 * @description: 下载pom文件
 * @param {srt.Dependency} dep 依赖信息
 * @param {...string} repos 仓库地址
 * @return {[]byte} pom文件数据
 * @return {error} 错误
 */
func downloadPom(dep srt.Dependency, repos ...string) (data []byte, err error) {
	if repos == nil {
		repos = []string{}
	}
	tags := strings.Split(dep.Vendor, ".")
	tags = append(tags, dep.Name)
	tags = append(tags, dep.Version.Org)
	tags = append(tags, fmt.Sprintf("%s-%s.pom", dep.Name, dep.Version.Org))
	// 遍历仓库地址, 默认maven仓库
	for i, repo := range append(repos, `https://repo.maven.apache.org/maven2/`) {
		// 是否是最后一个仓库(默认的maven仓库)
		last := i == len(repos)
		// 拼接完整的pom下载地址
		url := repo + strings.Join(tags, "/")
		if rep, err := http.Get(url); err != nil {
			if last {
				return nil, err
			} else {
				continue
			}
		} else {
			defer rep.Body.Close()
			if rep.StatusCode != 200 {
				if last {
					return ioutil.ReadAll(rep.Body)
				} else {
					continue
				}
			} else {
				return ioutil.ReadAll(rep.Body)
			}
		}
	}
	// 应该走不到这里
	return nil, fmt.Errorf("download failure")
}

// getpom is get pom from index
func getpom(groupId, artifactId, version string) (p *Pom) {
	p = &Pom{Properties: PomProperties{}}
	if groupId == "" || artifactId == "" || version == "" {
		return nil
	}
	dep := srt.Dependency{
		Vendor:  groupId,
		Name:    artifactId,
		Version: srt.NewVersion(version),
	}
	data := cache.LoadCache(dep)
	if len(data) != 0 {
		return ReadPom(data)
	} else {
		// 无本地缓存下载pom文件
		if data, err := downloadPom(dep); err == nil {
			// 保存pom文件
			cache.SaveCache(dep, data)
			return ReadPom(data)
		} else {
			logs.Warn(err)
		}
	}
	return nil
}

// BuildTree is build maven tree after all pom file parsed
func (a Analyzer) BuildTree(root *srt.DepTree) {
	for _, p := range a.mvn.MvnSimulation() {
		buildTree(p, root)
	}
}

func buildTree(p *Pom, root *srt.DepTree) {
	dep := srt.NewDepTree(root)
	dep.Vendor = p.GroupId
	dep.Name = p.ArtifactId
	dep.Version = srt.NewVersion(p.Version)
	dep.Language = language.Java
	dep.Path = path.Join(root.Path, path.Join(p.Path, dep.Dependency.String()))
	bar.Dependency.Add(1)
	for _, dp := range p.DependenciesPom {
		bar.Maven.Add(1)
		buildTree(dp, dep)
	}
}
