package com.jd.platform.jlog.worker.disruptor;

import com.jd.platform.jlog.worker.store.TracerLogToDbStore;
import com.jd.platform.jlog.worker.store.TracerModelToDbStore;
import com.lmax.disruptor.BlockingWaitStrategy;
import com.lmax.disruptor.EventFactory;
import com.lmax.disruptor.RingBuffer;
import com.lmax.disruptor.dsl.Disruptor;
import com.lmax.disruptor.dsl.ProducerType;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import javax.annotation.Resource;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadFactory;

/**
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-24
 */
@Configuration
public class DisruptorStarter {

    /**
     * 消费者线程数量
     */
    @Value("${thread.count}")
    private int threadCount;
    /**
     * TracerToDbStore
     */
    @Resource
    private TracerModelToDbStore tracerModelToDbStore;
    /**
     * tracerLogToDbStore
     */
    @Resource
    private TracerLogToDbStore tracerLogToDbStore;
    /**
     * 队列最大容量，1024*16，也就是RingBuffer大小，必须是2的N次方
     */
    @Value("${queue.maxSize}")
    private int maxSize;

    @Bean
    public Producer init() {
        ThreadFactory producerFactory = Executors.defaultThreadFactory();

        EventFactory<OneTracer> eventFactory = OneTracer::new;
        Disruptor<OneTracer> disruptor = new Disruptor<>(eventFactory, maxSize, producerFactory,
                ProducerType.SINGLE, new BlockingWaitStrategy());

        // 创建10个消费者来处理同一个生产者发的消息(这10个消费者不重复消费消息)
        TracerConsumer[] consumers = new TracerConsumer[threadCount];
        for (int i = 0; i < consumers.length; i++) {
            consumers[i] = new TracerConsumer(tracerModelToDbStore, tracerLogToDbStore);
        }
        disruptor.handleEventsWithWorkerPool(consumers);

        RingBuffer<OneTracer> ringBuffer = disruptor.start();

        return new Producer(ringBuffer);
    }
}
