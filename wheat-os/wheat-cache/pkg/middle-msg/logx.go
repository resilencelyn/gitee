package middlemsg

import "time"

var (
	LogContextName = "log-context"
)

type LogContext struct {
	Level string
	Data  time.Time
	Msg   string
	Route string
}
