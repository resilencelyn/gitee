/*
 * @Descripation: ruby解析器
 * @Date: 2021-11-30 14:36:49
 */

package ruby

import (
	"opensca/internal/enum/language"
	"opensca/internal/filter"
	"opensca/internal/srt"
)

type Analyzer struct{}

/**
 * @description: 创建ruby解析器
 * @return {ruby.Analyzer} ruby解析器
 */
func New() Analyzer {
	return Analyzer{}
}

/**
 * @description: Get language of Analyzer
 * @return {enum.Language} 语言
 */
func (a Analyzer) GetLanguage() language.Type {
	return language.Ruby
}

/**
 * @description: Check if it is a parsable file
 * @param {string} filename file name
 * @return {bool} is a parseable file returns true
 */
func (a Analyzer) CheckFile(filename string) bool {
	return filter.RubyGemfileLock(filename)
}

/**
 * @description: filters the files that the current parser needs to parse
 * @param {*srt.DirTree} dirRoot directory tree node
 * @param {*srt.DepTree} depRoot Dependency tree node
 * @return {[]*srt.FileData} List of files to parse
 */
func (a Analyzer) FilterFile(dirRoot *srt.DirTree, depRoot *srt.DepTree) []*srt.FileData {
	files := []*srt.FileData{}
	// 筛选需要解析的文件
	for _, f := range dirRoot.Files {
		if a.CheckFile(f.Name) {
			files = append(files, f)
		}
	}
	return files
}

/**
 * @description: Parse the file
 * @param {*srt.DirTree} dirRoot directory tree node
 * @param {*srt.DepTree} depRoot Dependency tree node
 * @param {*srt.FileData} file data to parse
 * @return {[]*srt.DepTree} parsed dependency list
 */
func (a Analyzer) ParseFile(dirRoot *srt.DirTree, depRoot *srt.DepTree, file *srt.FileData) (deps []*srt.DepTree) {
	deps = []*srt.DepTree{}
	if filter.RubyGemfileLock(file.Name) {
		return parseGemfileLock(depRoot, file)
	}
	return deps
}
