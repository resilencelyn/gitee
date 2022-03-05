package errorx

func TimeOutErr() error {
	return New("time out err")
}
