package util

import (
	"regexp"
	"strconv"
	"strings"

	"gitee.com/wheat-os/wheatCache/pkg/errorx"
)

// ParseSizeToBit
// 支持MB, GB, KB, 格式 "5KB" 或者 "5kb"等等
func ParseSizeToBit(size string) (int64, error) {
	sizes := regexp.MustCompile("^\\d+")
	sizeRes := sizes.FindAllString(size, 1)
	unit := strings.Split(size, sizeRes[0])
	Res, _ := strconv.ParseInt(sizeRes[0], 10, 64)
	sizeType := strings.ToUpper(unit[1])
	switch {
	case sizeType == "BIT" || sizeType == "B":
		return Res, nil
	case sizeType == "KB":
		return Res * 1024, nil
	case sizeType == "MB":
		return Res * 1024 * 1024, nil
	case sizeType == "GB":
		return Res * 1024 * 1024 * 1024, nil
	default:
		return 0, errorx.New("your size is wrong")
	}
}
