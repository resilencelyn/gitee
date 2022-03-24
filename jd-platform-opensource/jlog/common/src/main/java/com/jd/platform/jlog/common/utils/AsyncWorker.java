package com.jd.platform.jlog.common.utils;

import java.util.Collection;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName AsyncWorker.java
 * @Description TODO
 * @createTime 2022年02月21日 17:10:00
 */
public class AsyncWorker {

    public static <E> int drain(BlockingQueue<E> q, Collection<? super E> buffer, int numElements, long timeout, TimeUnit unit) throws Exception {
        if(buffer == null){
            throw new Exception("[Assertion failed] - the buffer argument cannot be null");
        }
        long deadline = System.nanoTime() + unit.toNanos(timeout);
        int added = 0;

        while(added < numElements) {
            added += q.drainTo(buffer, numElements - added);
            if (added < numElements) {
                E e = q.poll(deadline - System.nanoTime(), TimeUnit.NANOSECONDS);
                if (e == null) {
                    break;
                }
                buffer.add(e);
                ++added;
            }
        }
        return added;
    }

}
