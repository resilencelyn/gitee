package com.yunze.polling.card;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.AMQP;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.DefaultConsumer;
import com.rabbitmq.client.Envelope;
import com.yunze.apiCommon.utils.InternalApiRequest;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.redis.RedisCache;
import com.yunze.apiCommon.mapper.YzCardRouteMapper;
import com.yunze.common.utils.yunze.CardFlowSyn;
import com.yunze.apiCommon.utils.VeDate;
import com.yunze.common.utils.yunze.YzPollingUtil;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.connection.Connection;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * 卡用量轮序消费 者
 */
@Slf4j
@Component
public class CardFlow {


    @Resource
    private InternalApiRequest internalApiRequest;
    @Resource
    private RedisCache redisCache;
    @Resource
    private YzCardRouteMapper yzCardRouteMapper;
    @Resource
    private RabbitMQConfig rabbitMQConfig;
    @Resource
    private CardFlowSyn cardFlowSyn;
    @Resource
    private YzPollingUtil yzPollingUtil;


    /**
     * 轮序开始类型监听器 通过该监听器灵活监听
     * @param msg
     * @param channel_1
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "polling_card_flow_one")
    public void pollingStart(String msg, Channel channel_1) throws IOException {

        Map<String, Object> map = JSON.parseObject(msg);
        String FindCd_id = map.get("FindCd_id").toString();
        Map<String, Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id", FindCd_id);
        List<Map<String, Object>> ChannelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        RunEx(null,ChannelArr,false);//执行 公共部分
    }






    /**
     * 创建 监听
     */

    public void createListener() {
        //获取现在数据库中 开启轮序 状态正常的通道进行监听
        Map<String, Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id", null);
        List<Map<String, Object>> ChannelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        List<String> Ex_ChannelArr = yzCardRouteMapper.findCardChannelID();//查询 卡表里已经划分 的 通道信息

        RunEx(Ex_ChannelArr,ChannelArr,true);//执行 公共部分

    }


    /**
     * 执行 公共部分
     * @param Ex_ChannelArr
     * @param ChannelArr
     * @param bool 是否过滤 不需要监听的
     */
    public void RunEx(List<String> Ex_ChannelArr,List<Map<String, Object>> ChannelArr,boolean bool){
        if (ChannelArr != null && ChannelArr.size() > 0) {

            ChannelArr = yzPollingUtil.filterNoPollin(Ex_ChannelArr,ChannelArr,bool);//过滤不需要轮询的 通道id
            // 创建的连接
            Connection connection = rabbitMQConfig.getConnection();
            Channel channel = null;
            try {
                channel = connection.createChannel(false);
            } catch (Exception e) {
                log.error("createListener  connection.createChannel() Exception  {} " + e.getMessage());
                try {
                    log.info("再次创建连接=====");
                    rabbitMQConfig.connection = null;
                    connection = rabbitMQConfig.getConnection();
                    channel = connection.createChannel(false);
                } catch (Exception e1) {
                    log.error("再次创建连接 异常…… Exception  {} " + e1.getMessage());
                }
            }

            try {
                //获取redis 中 记录的 已经在监听中的通道名称 避免重复监听
                for (int i = 0; i < ChannelArr.size(); i++) {
                    Map<String, Object> ChannelObj = ChannelArr.get(i);
                    String cd_id = ChannelObj.get("cd_id").toString();
                    Object cd_concurrency = ChannelObj.get("cd_concurrency");//创建消费者数量
                    cd_concurrency = cd_concurrency != null && cd_concurrency.toString().trim().length() > 0 ? cd_concurrency : "1";
                    int Threads = Integer.parseInt(cd_concurrency.toString());
                    Object isExecute = redisCache.getCacheObject("polling-Listener-CardFlow-" + cd_id);
                    if (isExecute == null || bool==false) {
                        //redis 存储
                        ChannelObj.put("pollingtime", VeDate.getStringDateShort());
                        redisCache.setCacheObject("polling-Listener-CardFlow-" + cd_id, ChannelObj);// 缓存 避免 重复创建 消费 监听
                        try {
                            List(Threads, cd_id, ChannelObj, channel);
                        } catch (Exception e) {
                            log.error(">> CardFlow activateConsumer  失败:{} | {} | {}<<", e.getMessage(), cd_id, ChannelObj.toString());
                        }
                    }
                }
            } catch (Exception e) {
                log.error("业务逻辑异常 ：  {} " + e.getMessage());
            }
        }
    }












    public void List(int threadCount, String cd_id, Map<String, Object> ChannelObj, Channel channel) {
        ThreadPoolExecutor executor = null;
        try {
            //创建和线程同等数量的线程池
            executor = (ThreadPoolExecutor) Executors.newFixedThreadPool(threadCount);
            //阻塞，同步线程
            CountDownLatch countDownLatch = new CountDownLatch(threadCount);
            for (int i = 0; i < threadCount; i++) {
                log.info("=====启动 {}=====", Thread.currentThread().getName());
                //开始分配线程处理
                SynTask synTask = new SynTask(cd_id, ChannelObj, channel);

                executor.execute(synTask);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            executor.shutdown();
            while (true) {
                if (executor.isTerminated()) {
                    break;
                }
            }
        }
    }

    @Async
    class SynTask implements Runnable {
        private String cd_id;
        private Map<String, Object> ChannelObj;
        private Channel channel;


        public SynTask(String Pcd_id, Map<String, Object> PChannelObj, Channel Pchannel) {
            this.cd_id = Pcd_id;
            this.ChannelObj = PChannelObj;
            this.channel = Pchannel;
            //activateConsumer(Pcd_id,PChannelObj,Pchannel);
        }


        /**
         * 创建 消费者 卡用量
         *
         * @param cd_id
         */
        public void activateConsumer(String cd_id, Map<String, Object> ChannelObj, Channel channel) {
            System.out.println(channel);
            Connection connection = null;
            try {
                // 创建的连接
                //connection = rabbitMQConfig.getConnection();
                // connection = connection != null ? connection : rabbitMQConfig.getConnection();
                // 创建通道
                //Channel channel = connection.createChannel(false);
                String QUEUE_NAME = null, EXCHANGE_NAME = null, ROUTINGKEY = null;
                EXCHANGE_NAME = "polling_cardCardFlow_exchange";
                QUEUE_NAME = "polling_cardCardFlow_queue_" + cd_id;
                ROUTINGKEY = "polling.cardCardFlow.routingKey." + cd_id;
                try {
                    channel.queueBind(QUEUE_NAME, EXCHANGE_NAME, ROUTINGKEY);
                    DefaultConsumer defaultConsumer = new DefaultConsumer(channel) {
                        @Override
                        public void handleDelivery(String consumerTag, Envelope envelope, AMQP.BasicProperties properties, byte[] body) throws IOException {
                            String msg = new String(body, "UTF-8");
                            //System.out.println("polling_cardCardFlow_queue_"+cd_id+" 消费者获取消息:" + msg);
                            synCardFlow(msg, channel);//同步卡用量
                        }
                    };
                    // 开始监听消息 自动签收
                    channel.basicConsume(QUEUE_NAME, true, defaultConsumer);
                } catch (Exception e) {
                    try {
                        redisCache.redisTemplate.delete("polling-Listener-CardFlow-" + cd_id);
                        log.error(">> 创建 消费者失败 > 删除 redis记录Key polling-Listener-CardFlow-{} | {}<<", cd_id);
                    } catch (Exception e1) {
                        log.error(">> 创建 消费者失败 > 删除 redis记录Key polling-Listener-CardFlow-{} 失败 | {}<<", cd_id, e1.getMessage());
                    }
                }


            } catch (Exception e) {
                log.error(">>DB - 卡用量 创建 轮序消费者 同步卡用量失败:{} | {}<<", e.getMessage(), ChannelObj.toString());
                try {
                    redisCache.redisTemplate.delete("polling-Listener-CardFlow-" + cd_id);
                    log.error(">> 创建 消费者失败 > 删除 redis记录Key polling-Listener-CardFlow-{} | {}<<", cd_id);
                } catch (Exception e1) {
                    log.error(">> 创建 消费者失败 > 删除 redis记录Key polling-Listener-CardFlow-{} 失败 | {}<<", cd_id, e1.getMessage());
                }
            }
        }


        /**
         * 同步卡用量
         *
         * @param msg
         * @throws IOException
         */
        public void synCardFlow(String msg, Channel channel) {
            try {
                if (StringUtils.isEmpty(msg)) {
                    return;
                }
                Map<String, Object> map = JSON.parseObject(msg);
                String iccid = map.get("iccid").toString();
                String polling_id = map.get("polling_id").toString();//轮询任务编号
                String prefix = "polling_cardCardFlow_queue";
                //执行前判断 redis 是否存在 执行数据 存在时 不执行
                Object isExecute = redisCache.getCacheObject(prefix + ":" + iccid);
                if (isExecute == null) {
                    //System.out.println("SUCCESS");
                    redisCache.setCacheObject(prefix + ":" + iccid, msg, 3, TimeUnit.MINUTES);//3 分钟缓存 避免 重复消费
                    redisCache.setCacheObject(polling_id + ":" + iccid, msg, 4, TimeUnit.HOURS);//4 小时缓存 用来统计轮序进度

                    Map<String, Object> Parammap = new HashMap<>();
                    Parammap.put("iccid", iccid);
                    Map<String, Object> Rmap = internalApiRequest.queryFlow(Parammap, map);
                    String code = Rmap.get("code") != null ? Rmap.get("code").toString() : "500";
                    if (code.equals("200")) {
                        //获取 卡用量 开卡日期 更新 card info
                        if (Rmap.get("Use") != null && Rmap.get("Use") != "" && Rmap.get("Use").toString().trim().length() > 0) {
                            Double Use = Double.parseDouble(Rmap.get("Use").toString());
                            if (Use >= 0) {
                                try {
                                    Map<String, Object> RMap = cardFlowSyn.CalculationFlow(iccid, Use);
                                    log.info(">>cardFlowSyn - 卡用量轮序消费者 同步卡用量返回:{} | {} | {}<<", polling_id, iccid, JSON.toJSON(RMap));
                                } catch (Exception e) {
                                    log.error(">>cardFlowSyn - 卡用量轮序消费者 同步卡用量失败:{} | {} | {}<<", polling_id, iccid, e.getMessage().toString());
                                }
                            } else {
                                log.info(">>API - 卡用量轮序消费者 未获取到卡用量 {} |  statusCode = 0 :{} | {}<<", polling_id, iccid, Rmap);
                            }
                        }
                    } else {
                        log.info(">>API - 卡用量轮序消费者 未获取到卡用量:{} | {} | {}<<", polling_id, iccid, Rmap);
                        // System.out.println(map.get("iccid")+" 未获取到卡用量 ！");
                    }
                }
            } catch (Exception e) {
//            // 记录该消息日志形式  存放数据库db中、后期通过定时任务实现消息补偿、人工实现补偿
                log.error(">>错误 - 卡用量轮序消费者:{}<<", e.getMessage().toString());
//            //将该消息存放到死信队列中，单独写一个死信消费者实现消费。
            }
        }


        @Override
        public void run() {
            activateConsumer(cd_id, ChannelObj, channel);
        }


    }


}
