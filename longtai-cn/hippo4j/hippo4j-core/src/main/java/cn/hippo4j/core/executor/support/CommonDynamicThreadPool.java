package cn.hippo4j.core.executor.support;

import cn.hippo4j.core.executor.DynamicThreadPoolExecutor;

import java.util.concurrent.TimeUnit;

/**
 * Common dynamic threadPool.
 *
 * @author chen.ma
 * @date 2021/6/16 22:35
 */
public class CommonDynamicThreadPool {

    /**
     * Get instance.
     *
     * @param threadPoolId
     * @return
     */
    public static DynamicThreadPoolExecutor getInstance(String threadPoolId) {
        DynamicThreadPoolExecutor poolExecutor = (DynamicThreadPoolExecutor) ThreadPoolBuilder.builder()
                .dynamicPool()
                .threadFactory(threadPoolId)
                .poolThreadSize(2, 4)
                .keepAliveTime(60L, TimeUnit.SECONDS)
                .workQueue(QueueTypeEnum.RESIZABLE_LINKED_BLOCKING_QUEUE, 1024)
                .build();
        return poolExecutor;
    }

}
