package errorx

func EventRecoveryErr() error {
	return New("this event has been recycled")
}
