package errorx

func LruNotWorkFuncEventErr() error {
	return New("the event haven't work of function")
}

func KeyBaseIsNilErr() error {
	return New("key base can't be nil")
}
