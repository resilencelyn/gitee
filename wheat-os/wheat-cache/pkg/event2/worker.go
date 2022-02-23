package event2

func (d *driver) awaitWorker() {
	for i := 0; i < awaitThread; i++ {
		go func() {
			for {
				awaitFunc := <-d.waitQueue
				e, res, err := awaitFunc()
				e.execWorker(res, err)
			}
		}()
	}
}
