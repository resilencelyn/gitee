package com.jd.platform.jlog.client.udp;

import com.jd.platform.jlog.client.Context;
import com.jd.platform.jlog.common.model.RunLogMessage;
import com.jd.platform.jlog.common.model.TracerBean;
import com.jd.platform.jlog.common.model.TracerData;
import com.jd.platform.jlog.common.utils.AsyncPool;
import com.jd.platform.jlog.common.utils.AsyncWorker;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicLong;

/**
 * udp对外发消息
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-16
 */
public class UdpSender {
    /**
     * logger
     */
    private static Logger logger = LoggerFactory.getLogger(UdpSender.class);
    /**
     * 本地队列满了后丢弃的数量
     */
    private static AtomicLong FAIL_OFFER_COUNT = new AtomicLong();
    /**
     * 本地队列，已写入的总数量
     */
    private static AtomicLong SUCCESS_OFFER_COUNT = new AtomicLong();
    /**
     * 本地logger日志队列，已写入的总数量
     */
    private static AtomicLong SUCCESS_LOGGER_OFFER_COUNT = new AtomicLong();
    /**
     * 出入参集中营，最多积压5万个
     */
    private static LinkedBlockingQueue<TracerBean> tracerBeanQueue = new LinkedBlockingQueue<>(50000);
    /**
     * 日志集中营，最多积压5万条
     */
    private static LinkedBlockingQueue<RunLogMessage> logBeanQueue = new LinkedBlockingQueue<>(50000);


    /**
     * 写入队列
     */
    public static void offerBean(TracerBean tracerBean) {
        //容量是否已满
        boolean success = tracerBeanQueue.offer(tracerBean);
        if (!success) {
            long failCount = FAIL_OFFER_COUNT.incrementAndGet();
            if (failCount % 10 == 0) {
                logger.info("用户跟踪队列已满，当前丢弃的数量为: " + failCount);
            }
        } else {
            long successCount = SUCCESS_OFFER_COUNT.incrementAndGet();
            if (successCount % 1000 == 0) {
                logger.info("用户跟踪已产生数量：" + successCount + "，当前队列积压数量：" + tracerBeanQueue.size());
            }
        }
    }

    /**
     * 写入log队列
     */
    public static void offerLogger(RunLogMessage runLogMessage) {
        //容量是否已满
        boolean success = logBeanQueue.offer(runLogMessage);
        if (!success) {
            long failCount = FAIL_OFFER_COUNT.incrementAndGet();
            if (failCount % 10 == 0) {
                logger.info("用户Logger队列已满，当前丢弃的数量为: " + failCount);
            }
        } else {
            long successCount = SUCCESS_LOGGER_OFFER_COUNT.incrementAndGet();
            if (successCount % 10000 == 0) {
                logger.info("用户Logger已产生数量：" + successCount + "，当前队列积压数量：" + tracerBeanQueue.size());
            }
        }
    }


    /**
     * 定时往worker发烧
     */
    public static void uploadToWorker() {
        //filter拦截到的出入参
        AsyncPool.asyncDo(() -> {
            while (true) {
                try {
                    List<TracerBean> tempTracers = new ArrayList<>();
                    TracerBean tracerBean = tracerBeanQueue.take();
                    tempTracers.add(tracerBean);

                    send(tempTracers);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
        //用户中途打的各日志
        AsyncPool.asyncDo(() -> {
            while (true) {
                try {
                    //要么key达到500个，要么达到1秒，就汇总上报给worker一次
                    List<RunLogMessage> tempLogs = new ArrayList<>();
                    AsyncWorker.drain(logBeanQueue, tempLogs, 500, 1, TimeUnit.SECONDS);
                    if (tempLogs.size() == 0) {
                        continue;
                    }

                    List<TracerBean> tempTracers = new ArrayList<>();
                    TracerBean tracerBean = new TracerBean();
                    tracerBean.setTracerId("-1");
                    List<Map<String, Object>> tracerObject = new ArrayList<>();

                    Map<String, Object> map = new HashMap<>();
                    for (RunLogMessage runLogMessage : tempLogs) {
                        map.put(UUID.randomUUID().toString(), runLogMessage);
                    }
                    tracerObject.add(map);
                    tracerBean.setTracerObject(tracerObject);
                    tempTracers.add(tracerBean);

                    send(tempTracers);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * 往worker发traceBean
     */
    private static void send(List<TracerBean> tracerBeans) {
        TracerData tracerData = new TracerData();
        tracerData.setTracerBeanList(tracerBeans);
        Context.CHANNEL.writeAndFlush(tracerData);
    }
}
