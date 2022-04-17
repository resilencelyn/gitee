/*
 * @Descripation: 解压相关操作
 * @Date: 2021-11-04 16:13:57
 */

package engine

import (
	"archive/tar"
	"archive/zip"
	"compress/gzip"
	"io/ioutil"
	"opensca/internal/bar"
	"opensca/internal/filter"
	"opensca/internal/logs"
	"opensca/internal/srt"
	"os"
	"path"
	"strings"

	"github.com/axgle/mahonia"
	"github.com/mholt/archiver"
	"github.com/nwaples/rardecode"
	"github.com/pkg/errors"
)

/**
 * @description: 检测是否为可检测的文件
 * @param {string} filename 文件名
 * @return {bool} 可解析的文件返回true
 */
func (e Engine) checkFile(filename string) bool {
	for _, analyzer := range e.Analyzers {
		if analyzer.CheckFile(filename) {
			return true
		}
	}
	return false
}

/**
 * @description:
 * 解压压缩包
 * 将其中可解析的压缩包作为依赖返回
 * 可解析的文件放到当前依赖(root)的FileData中
 * @param {string} filepath 文件路径
 * @return {*srt.DirTree} root 目录树
 */
func (e Engine) unArchiveFile(filepath string) (root *srt.DirTree) {
	filepath = strings.ReplaceAll(filepath, `\`, `/`)
	// 目录树根
	root = srt.NewDirTree()
	root.Path = path.Base(filepath)
	var walker archiver.Walker
	if filter.Tar(filepath) {
		walker = archiver.NewTar()
	} else if filter.Zip(filepath) || filter.Jar(filepath) {
		walker = archiver.NewZip()
	} else if filter.Rar(filepath) {
		walker = archiver.NewRar()
	} else if filter.TarGz(filepath) {
		walker = archiver.NewTarGz()
	} else if filter.TarBz2(filepath) {
		walker = archiver.NewTarBz2()
	}
	if err := walker.Walk(filepath, func(f archiver.File) error {
		if !f.IsDir() {
			bar.Archive.Add(1)
			// 跳过过大文件
			if f.Size() > 1024*1024*1024 {
				return nil
			}
			fileName := f.Name()
			if file, ok := f.Header.(zip.FileHeader); ok {
				if file.Flags == 0 {
					gbkName := mahonia.NewDecoder("gbk").ConvertString(file.Name)
					_, cdata, _ := mahonia.NewDecoder("utf-8").Translate([]byte(gbkName), true)
					fileName = string(cdata)
				} else {
					fileName = file.Name
				}
			} else if file, ok := f.Header.(*rardecode.FileHeader); ok {
				fileName = file.Name
			} else if file, ok := f.Header.(*gzip.Header); ok {
				fileName = file.Name
			} else if file, ok := f.Header.(*tar.Header); ok {
				fileName = file.Name
			}
			// 读取文件数据
			data, err := ioutil.ReadAll(f)
			if err != nil {
				return errors.WithStack(err)
			}
			// 格式化路径
			fileName = strings.ReplaceAll(fileName, `\`, `/`)
			if e.checkFile(fileName) {
				// 支持解析的文件
				root.AddFile(srt.NewFileData(fileName, data))
			} else if filter.AllPkg(fileName) {
				// 支持检测的压缩包
				rootPath, _ := os.Executable()
				rootPath = path.Dir(strings.ReplaceAll(rootPath, `\`, `/`))
				tempPath := path.Join(rootPath, ".temp_path")
				// 创建临时文件夹
				os.Mkdir(tempPath, os.ModeDir)
				targetPath := path.Join(tempPath, path.Base(fileName))
				// 将压缩包解压到本地
				if out, err := os.Create(targetPath); err == nil {
					_, err = out.Write(data)
					out.Close()
					if err != nil {
						return errors.WithStack(err)
					}
					// 获取当前目录树
					dir := root.GetDir(fileName)
					name := path.Base(fileName)
					if _, ok := dir.SubDir[name]; !ok {
						// 将压缩包的内容添加到当前目录树
						dir.DirList = append(dir.DirList, name)
						dir.SubDir[name] = e.unArchiveFile(targetPath)
					}
					// 删除压缩包
					if err = os.Remove(targetPath); err != nil {
						logs.Error(err)
					}
				} else {
					logs.Error(err)
				}
			}
		}
		return nil
	}); err != nil {
		logs.Error(errors.WithMessage(err, filepath))
	}
	return root
}

/**
 * @description:
 * 打开目录
 * 将其中可解析的文件或压缩包作为依赖返回
 * 可解析的文件放到当前依赖(root)的Files中
 * @param {context.Context} ctx 上下文
 * @param {string} dirpath 目录路径
 * @return {*srt.DirTree} 目录树
 */
func (e Engine) opendir(dirpath string) (dir *srt.DirTree) {
	bar.Dir.Add(1)
	dir = srt.NewDirTree()
	files, err := ioutil.ReadDir(dirpath)
	if err != nil {
		logs.Error(errors.WithMessage(err, dirpath))
		return
	}
	for _, file := range files {
		filename := file.Name()
		filepath := path.Join(dirpath, filename)
		if file.IsDir() {
			dir.DirList = append(dir.DirList, filename)
			dir.SubDir[filename] = e.opendir(filepath)
		} else {
			if filter.AllPkg(filename) {
				dir.DirList = append(dir.DirList, filename)
				dir.SubDir[filename] = e.unArchiveFile(filepath)
			} else if e.checkFile(filename) {
				if data, err := ioutil.ReadFile(filepath); err != nil {
					logs.Error(err)
				} else {
					dir.Files = append(dir.Files, srt.NewFileData(filepath, data))
				}
			}
		}
	}
	return
}
