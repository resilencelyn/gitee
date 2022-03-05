package middlemsg

import (
	"time"
)

const (
	PulginsInfosName = "plugins-infos-context"
)

type PulginsInfo struct {
	Version string
	Desc    string
	Name    string
	Statux  string
	Time    time.Duration
}

type PulginsInfos struct {
	Infos []*PulginsInfo
}
