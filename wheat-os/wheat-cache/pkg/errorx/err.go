package errorx

import (
	"errors"
	"fmt"
)

// New TODO 添加链路追踪等 @bandl @lgq
func New(msg string, format ...interface{}) error {
	msg = fmt.Sprintf(msg, format...)
	return errors.New(msg)
}
