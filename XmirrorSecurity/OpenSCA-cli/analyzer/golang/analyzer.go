/*
 * @Description: golang analyzer
 * @Date: 2022-02-10 16:08:00
 */

package golang

import (
	"sort"
	"util/enum/language"
	"util/filter"
	"util/model"
)

// golang Analyzer
type Analyzer struct{}

// New create golang Analyzer
func New() Analyzer {
	return Analyzer{}
}

// GetLanguage Get language of Analyzer
func (Analyzer) GetLanguage() language.Type {
	return language.Golang
}

// CheckFile Check if it is a parsable file
func (Analyzer) CheckFile(filename string) bool {
	return filter.GoMod(filename) || filter.GoSum(filename)
}

// FilterFile filters the files that the current parser needs to parse
func (a Analyzer) FilterFile(dirRoot *model.DirTree, depRoot *model.DepTree) []*model.FileData {
	files := []*model.FileData{}
	for _, file := range dirRoot.Files {
		if a.CheckFile(file.Name) {
			files = append(files, file)
		}
	}
	sort.Slice(files, func(i, j int) bool {
		return filter.GoSum(files[i].Name) && !filter.GoSum(files[j].Name)
	})
	return files
}

// ParseFile Parse the file
func (Analyzer) ParseFile(dirRoot *model.DirTree, depRoot *model.DepTree, file *model.FileData) []*model.DepTree {
	if filter.GoMod(file.Name) {
		return parseGomod(depRoot, file)
	} else if filter.GoSum(file.Name) {
		return parseGosum(depRoot, file)
	}
	return []*model.DepTree{}
}
