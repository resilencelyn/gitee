package com.jd.platform.jlog.worker.store;

import org.springframework.stereotype.Component;

/**
 * 客户端发来消息暂存于此
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-12
 */
@Component
public class TracerBeanStore {
//    /**
//     * 队列长度1千万
//     * <p>
//     * 抛出异常	    立即返回true、false	       阻塞	       超时
//     * 插入	add(e)	    offer(e)	           put(e)	   offer(e, time, unit)
//     * 移除	remove()	poll()	               take()	   poll(time, unit)
//     * 检查	element()	peek()	               不可用	   不可用
//     */
//    private LinkedBlockingQueue<byte[]> traceQueue;
//    /**
//     * 已提供到队列的总数量
//     */
//    private final LongAdder totalOfferCount = new LongAdder();
//    /**
//     * 已消费完毕的总数量
//     */
//    private final LongAdder totalDealCount = new LongAdder();
//    /**
//     * 来不及消费，写入队列的数量
//     */
//    private final LongAdder totalSendToMqCount = new LongAdder();
//    /**
//     * 消费者线程池
//     */
//    private ExecutorService threadPoolExecutor = Executors.newCachedThreadPool();
//    /**
//     * 消费者线程数量
//     */
//    @Value("${thread.count}")
//    private int threadCount;
//    /**
//     * 往待入库队列写
//     */
//    @Resource
//    private TracerToDbStore tracerToDbStore;
//    /**
//     * logger
//     */
//    private Logger logger = LoggerFactory.getLogger(getClass());
//
//    /**
//     * 初始化队列容量
//     */
//    public void initQueue(int size) {
//        traceQueue = new LinkedBlockingQueue<>(size);
//    }

    /**
     * 写入队列
     */
//    public void offer(byte[] body) {
//        //容量是否已满
//        boolean success = traceQueue.offer(body);
//        totalOfferCount.increment();
//
//        long totalReceive = totalOfferCount.longValue();
//        if (totalReceive % 10000 == 0) {
//            logger.info("已接收：" + totalReceive + ", 当前Queue里元素数量为" + traceQueue.size());
//        }
//
//        if (!success) {
//            //TODO 队列满了，就尝试写MQ
//            totalSendToMqCount.increment();
//            logger.info("队列已满");
//        }
//    }

    /**
     * 开启消费
     */
//    public void beginConsume() {
//        for (int i = 0; i < threadCount; i++) {
//            threadPoolExecutor.submit(this::consume);
//        }
//    }

    /**
     * 单线程开始消费
     */
//    private void consume() {
//        while (true) {
//            try {
//                //压缩后的字节数组
//                byte[] body = traceQueue.take();
//                byte[] decompressBytes = ZstdUtils.decompressBytes(body);
//
//                TracerData tracerData = ProtostuffUtils.deserialize(decompressBytes, TracerData.class);
//                //包含了多个tracer对象
//                List<TracerBean> tracerBeanList = tracerData.getTracerBeanList();
//                buildTracerModel(tracerBeanList);
//
//                //处理完毕，将数量加1
//                totalDealCount.increment();
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
//        }
//    }

    /**
     * 构建要入库的对象
     */
//    private void buildTracerModel(List<TracerBean> tracerBeanList) {
//        //遍历传过来的
//        for (TracerBean tracerBean : tracerBeanList) {
//            Map<String, Object> map = new HashMap<>();
//
//            List<Map<String, Object>> mapList = tracerBean.getTracerObject();
//
//            Map<String, Object> requestMap = mapList.get(0);
//            Map<String, Object> responseMap = mapList.get(mapList.size() - 1);
//            byte[] responseBytes = (byte[]) responseMap.get("response");
//
//            map.put("requestContent", FastJsonUtils.collectToString(requestMap));
//            map.put("responseContent", new String(responseBytes));
//            map.put("createTime", DateUtil.formatDateTime(new Date(tracerBean.getCreateTime())));
//            map.put("costTime", tracerBean.getCostTime());
//
//            long tracerId = requestMap.get("tracerId") == null ? 0 : Long.valueOf(requestMap.get("tracerId").toString());
//            map.put("tracerId", tracerId);
//
//            String pin = requestMap.get("pin") == null ? null : requestMap.get("pin").toString();
//            map.put("pin", pin);
//            String uuid = requestMap.get("uuid") == null ? null : requestMap.get("uuid").toString();
//            map.put("uuid", uuid);
//
//            String client = requestMap.get("client") == null ? null : requestMap.get("client").toString();
//            int clientType = 0;
//            if ("apple".equals(client)) {
//                clientType = 2;
//            } else if ("android".equals(client)) {
//                clientType = 1;
//            }
//            map.put("clientType", clientType);
//            String clientVersion = requestMap.get("clientVersion") == null ? null : requestMap.get("clientVersion").toString();
//            map.put("clientVersion", clientVersion);
//
//            String userIp = requestMap.get("ip") == null ? null : requestMap.get("ip").toString();
//            map.put("userIp", userIp);
//            String serverIp = requestMap.get("serverIp") == null ? null : requestMap.get("serverIp").toString();
//            map.put("serverIp", serverIp);
//
//            map.put("intoDbTime", DateUtil.formatDateTime(new Date(tracerBean.getCreateTime())));
//
//            tracerToDbStore.offer(map);
//        }
//    }

//    /**
//     * 构建要入库的对象
//     */
//    private void buildTracerModel(List<TracerBean> tracerBeanList) {
//        //遍历传过来的
//        for (TracerBean tracerBean : tracerBeanList) {
//            TracerModel tracerModel = new TracerModel();
//
//            List<Map<String, Object>> mapList = tracerBean.getTracerObject();
//
//            Map<String, Object> requestMap = mapList.get(0);
//            Map<String, Object> responseMap = mapList.get(mapList.size() - 1);
//            byte[] responseBytes = (byte[]) responseMap.get("response");
//
//            tracerModel.setRequestContent(FastJsonUtils.collectToString(requestMap));
//            tracerModel.setResponseContent(new String(responseBytes));
//            tracerModel.setCreateTime(tracerBean.getCreateTime());
//            tracerModel.setCostTime(tracerBean.getCostTime());
//
//            long tracerId = requestMap.get("tracerId") == null ? 0 : Long.valueOf(requestMap.get("tracerId").toString());
//            tracerModel.setTracerId(tracerId);
//
//            String pin = requestMap.get("pin") == null ? null : requestMap.get("pin").toString();
//            tracerModel.setPin(pin);
//            String uuid = requestMap.get("uuid") == null ? null : requestMap.get("uuid").toString();
//            tracerModel.setUuid(uuid);
//
//            String client = requestMap.get("client") == null ? null : requestMap.get("client").toString();
//            int clientType = 0;
//            if ("apple".equals(client)) {
//                clientType = 2;
//            } else if ("android".equals(client)) {
//                clientType = 1;
//            }
//            tracerModel.setClientType(clientType);
//            String clientVersion = requestMap.get("clientVersion") == null ? null : requestMap.get("clientVersion").toString();
//            tracerModel.setClientVersion(clientVersion);
//
//            String userIp = requestMap.get("ip") == null ? null : requestMap.get("ip").toString();
//            tracerModel.setUserIp(userIp);
//            String serverIp = requestMap.get("serverIp") == null ? null : requestMap.get("serverIp").toString();
//            tracerModel.setServerIp(serverIp);
//
//            tracerModel.setIntoDbTime(System.currentTimeMillis());
//
//            tracerToDbStore.offer(tracerModel);
//        }
//    }


}
