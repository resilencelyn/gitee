package errorx

func DaoTypeErr(typ string) error {
	return New("the type is not: %s", typ)
}

func NotKeyErr(key string) error {
	return New("the key is not exist, key:%s", key)
}
