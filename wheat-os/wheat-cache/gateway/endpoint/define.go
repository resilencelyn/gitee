package endpoint

type EndpointInterface interface {
	GetTargetAddr(...string) (string, error)
	IsEmpty() bool
	AddTarget(targets ...string)
}

const (
	HashReplicasDefault = 3
)
