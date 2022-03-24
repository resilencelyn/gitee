package com.jd.platform.jlog.worker.disruptor;

import com.lmax.disruptor.RingBuffer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.concurrent.atomic.LongAdder;

/**
 * Producer
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-24
 */
public class Producer {
    /**
     * 已提供到队列的总数量
     */
    private final LongAdder totalOfferCount = new LongAdder();
    /**
     * ringBuffer
     */
    private RingBuffer<OneTracer> ringBuffer;
    /**
     * logger
     */
    private Logger logger = LoggerFactory.getLogger(getClass());

    public Producer(RingBuffer<OneTracer> ringBuffer) {
        this.ringBuffer = ringBuffer;
    }

    /**
     * 发送数据
     */
    public void sendData(byte[] bytes) {
        if (bytes == null) {
            return;
        }

        long sequence = ringBuffer.next();
        totalOfferCount.increment();

        long totalReceive = totalOfferCount.longValue();
        if (totalReceive % 10000 == 0) {
            logger.info("生产消费队列，已接收：" + totalReceive);
        }
        try {
            OneTracer oneTracer = ringBuffer.get(sequence);
            oneTracer.setBytes(bytes);
        } finally {
            ringBuffer.publish(sequence);
        }
    }
}
