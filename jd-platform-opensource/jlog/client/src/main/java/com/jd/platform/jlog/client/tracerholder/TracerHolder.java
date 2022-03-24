package com.jd.platform.jlog.client.tracerholder;

import com.alibaba.ttl.TransmittableThreadLocal;

/**
 * 用于在线程间传递tracerId用
 * @author wuweifeng
 * @version 1.0
 * @date 2021-09-09
 */
public class TracerHolder {
    /**
     * 用于在线程池间也能透传tracerId
     */
    private static TransmittableThreadLocal<Long> context = new TransmittableThreadLocal<>();

    /**
     * 设置tracerId到线程里
     */
    public static void setTracerId(long tracerId) {
        context.set(tracerId);
    }

    /**
     * 如果没有id，说明filter没设置上，则给他设置一个
     */
    public static long getTracerId() {
        try {
            return context.get();
        } catch (Exception e) {
            return 0L;
        }
    }

}
