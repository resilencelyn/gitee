package com.jd.platform.jlog.worker.store;

import com.jd.platform.jlog.common.utils.AsyncPool;
import com.jd.platform.jlog.common.utils.AsyncWorker;
import com.jd.platform.jlog.common.utils.CollectionUtil;
import com.jd.platform.jlog.worker.db.Db;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.LongAdder;

/**
 * 解析好的数据暂存和入库
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-21
 */
@Component
public class TracerModelToDbStore {
    /**
     * 待入库的数据
     */
    private LinkedBlockingQueue<Map<String, Object>> modelQueue;
    /**
     * logger
     */
    private Logger logger = LoggerFactory.getLogger(getClass());
    /**
     * db
     */
    @Resource
    private Db db;
    /**
     * 已入库总数量
     */
    private final LongAdder totalInsertCount = new LongAdder();
    /**
     * 每批往ck写多少条
     */
    @Value("${clickhouse.batchSize}")
    private String batchSize;
    /**
     * 几个线程去入库
     */
    @Value("${clickhouse.poolSize}")
    private String poolSize;
    /**
     * 间隔几秒入库
     */
    @Value("${clickhouse.insertInterval}")
    private int interval;
    /**
     * 待入库队列长度
     */
    @Value("${queue.preDbSize}")
    private int preDbSize;
    /**
     * 写入队列
     */
    public void offer(Map<String, Object> map) {
        boolean success = modelQueue.offer(map);
        //如果队列已满，则做其他处理
        if (!success) {

        }
    }

    /**
     * 入库
     */
    public void beginIntoDb() {
        //初始化队列长度
        modelQueue = new LinkedBlockingQueue<>(preDbSize);
        int pool = Integer.valueOf(poolSize);

        for (int i = 0; i < pool; i++) {
            AsyncPool.asyncDo(() -> {
                try {
                    Thread.sleep(new Random().nextInt(8000));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                while (true) {
                    try {
                        List<Map<String, Object>> tempModels = new ArrayList<>();
                        //每1s入库一次
                        AsyncWorker.drain(modelQueue, tempModels, Integer.valueOf(batchSize), interval, TimeUnit.SECONDS);
                        if (CollectionUtil.isEmpty(tempModels)) {
                            continue;
                        }

                        //批量插入
                        int successCount = db.insertAll("tracer_model", tempModels);
                        totalInsertCount.add(successCount);
                        logger.info("model成功入库 " + tempModels.size() + " 条, 累计已入库 " + totalInsertCount.longValue() + ", 待入库队列size " + modelQueue.size());

                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            });
        }

    }


}
