package com.jd.platform.jlog.worker.disruptor;

import com.alibaba.fastjson.JSON;
import com.jd.platform.jlog.common.model.RunLogMessage;
import com.jd.platform.jlog.common.model.TracerBean;
import com.jd.platform.jlog.common.model.TracerData;
import com.jd.platform.jlog.common.utils.FastJsonUtils;
import com.jd.platform.jlog.common.utils.ProtostuffUtils;
import com.jd.platform.jlog.common.utils.ZstdUtils;
import com.jd.platform.jlog.worker.store.TracerLogToDbStore;
import com.jd.platform.jlog.worker.store.TracerModelToDbStore;
import com.lmax.disruptor.WorkHandler;
import io.netty.util.internal.StringUtil;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.concurrent.atomic.LongAdder;

import static com.jd.platform.jlog.common.constant.Constant.DEFAULT_BYTE;
import static com.jd.platform.jlog.common.constant.Constant.REQ;
import static com.jd.platform.jlog.common.constant.Constant.RESP;

/**
 * TracerConsumer
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-24
 */
public class TracerConsumer implements WorkHandler<OneTracer> {
    /**
     * 已消费完毕的总数量
     */
    private static final LongAdder totalDealCount = new LongAdder();
    /**
     * logger
     */
    private Logger logger = LoggerFactory.getLogger(getClass());
    /**
     * 待入库队列，出入参model
     */
    private TracerModelToDbStore tracerModelToDbStore;
    /**
     * 待入库队列，普通日志
     */
    private TracerLogToDbStore tracerLogToDbStore;

    private static final DateTimeFormatter DEFAULT_FORMATTER = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");


    public TracerConsumer(TracerModelToDbStore tracerModelToDbStore, TracerLogToDbStore tracerLogToDbStore) {
        this.tracerModelToDbStore = tracerModelToDbStore;
        this.tracerLogToDbStore = tracerLogToDbStore;
    }

    @Override
    public void onEvent(OneTracer oneTracer) {
        try {
            long totalConsume = totalDealCount.longValue();
            boolean needInfo = totalConsume % 1000 == 0;

            //压缩后的字节数组
            byte[] decompressBytes = ZstdUtils.decompressBytes(oneTracer.getBytes());

            TracerData tracerData = ProtostuffUtils.deserialize(decompressBytes, TracerData.class);

            //包含了多个tracer对象
            List<TracerBean> tracerBeanList = tracerData.getTracerBeanList();
            buildTracerModel(tracerBeanList);

            //处理完毕，将数量加1
            totalDealCount.increment();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 构建要入库的对象
     */
    private void buildTracerModel(List<TracerBean> tracerBeanList) {
        //遍历传过来的
        for (TracerBean tracerBean : tracerBeanList) {
            //普通日志
            if ("-1".equals(tracerBean.getTracerId())) {
                dealTracerLog(tracerBean);
            } else {
                dealFilterModel(tracerBean);
            }

        }
    }

    /**
     * 处理中途日志
     */
    private void dealTracerLog(TracerBean tracerBean) {
        List<Map<String, Object>> mapList = tracerBean.getTracerObject();
        Map<String, Object> objectMap = mapList.get(0);
        //遍历value集合，里面每个都是一个RunLogMessage对象
        for (Object object :objectMap.values()) {
            Map<String, Object> map = new HashMap<>(12);

            RunLogMessage runLogMessage = (RunLogMessage) object;
            map.put("tracerId", runLogMessage.getTracerId());
            map.put("className", runLogMessage.getClassName());
            map.put("threadName", runLogMessage.getThreadName());
            map.put("methodName", runLogMessage.getMethodName());
            map.put("logLevel", runLogMessage.getLogLevel());
            map.put("createTime",  formatLongTime(runLogMessage.getCreateTime()));
            map.put("content", runLogMessage.getContent());
            map.putAll(runLogMessage.getTagMap());
            tracerLogToDbStore.offer(map);
        }

    }

    /**
     * 处理filter里处理的出入参
     */
    private void dealFilterModel(TracerBean tracerBean) {

        List<Map<String, Object>> mapList = tracerBean.getTracerObject();
        Map<String, Object> requestMap = mapList.get(0);

        Object req = requestMap.get(REQ);
        if(req == null){
            req = DEFAULT_BYTE;
        }
        requestMap.remove(REQ);

        Map<String, Object> map = new HashMap<>(requestMap);
        long tracerId = Long.parseLong(tracerBean.getTracerId());
        //filter的出入参
        Map<String, Object> responseMap = mapList.get(mapList.size() - 1);

        Object resp = responseMap.get(RESP);
        if(resp == null){
            resp = DEFAULT_BYTE;
        }

        map.put("requestContent", req);
        map.put("responseContent", resp);
        map.put("costTime", tracerBean.getCostTime());
        map.put("tracerId", tracerId);
        map.put("createTime", formatLongTime(tracerBean.getCreateTime()));
        responseMap.remove("response");
        map.putAll(responseMap);
        tracerModelToDbStore.offer(map);
    }

    private static String formatLongTime(long time) {
        return DEFAULT_FORMATTER.format(LocalDateTime.ofInstant(Instant.ofEpochMilli(time),ZoneId.systemDefault()));
    }

}
