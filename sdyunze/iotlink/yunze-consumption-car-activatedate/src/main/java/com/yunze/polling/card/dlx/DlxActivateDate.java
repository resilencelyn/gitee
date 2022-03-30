package com.yunze.polling.card.dlx;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.AMQP;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.DefaultConsumer;
import com.rabbitmq.client.Envelope;
import com.yunze.apiCommon.utils.InternalApiRequest;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.redis.RedisCache;
import com.yunze.apiCommon.mapper.YzCardRouteMapper;

import com.yunze.apiCommon.utils.VeDate;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.common.utils.yunze.YzPollingUtil;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.connection.Connection;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * @Auther: zhang feng
 * @Date: 2021/07/09/12:43
 * @Description:
 */
@Slf4j
@Component
public class DlxActivateDate {

    @Resource
    private YzCardMapper yzCardMapper;
    @Resource
    private InternalApiRequest internalApiRequest;
    @Resource
    private RedisCache redisCache;
    @Resource
    private YzCardRouteMapper yzCardRouteMapper;
    @Resource
    private RabbitMQConfig rabbitMQConfig;
    @Resource
    private RabbitTemplate rabbitTemplate;
    @Resource
    private YzPollingUtil yzPollingUtil;

    /**
     * 轮序开始类型监听器 通过该监听器灵活监听
     * @param msg
     * @param channel_1
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "polling_card_activateDate_tow")
    public void pollingStart(String msg, Channel channel_1) throws IOException {
        Map<String, Object> map = JSON.parseObject(msg);
        String FindCd_id = map.get("FindCd_id").toString();
        Map<String, Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id", FindCd_id);
        List<Map<String, Object>> ChannelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        RunEx(null,ChannelArr,false);//执行 公共部分
    }



    /**
     * 创建 消费者进行消费
     */
    public void createListener()  {
        //获取现在数据库中 开启轮序 状态正常的通道进行监听
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
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
        if(ChannelArr!=null && ChannelArr.size()>0){
            ChannelArr = yzPollingUtil.filterNoPollin(Ex_ChannelArr,ChannelArr,bool);//过滤不需要轮询的 通道id
            Connection connection = rabbitMQConfig.getConnection();
            Channel channel = null;
            try {
                channel = connection.createChannel(false);
            }catch (Exception e){
                log.error("createListener  connection.createChannel() Exception  {} "+e.getMessage());
                try {
                    log.info("再次创建连接=====");
                    rabbitMQConfig.connection = null;
                    connection = rabbitMQConfig.getConnection();
                    channel = connection.createChannel(false);
                }catch (Exception e1){
                    log.error("再次创建连接 异常…… Exception  {} "+e1.getMessage());
                }
            }
            //获取redis 中 记录的 已经在监听中的通道名称 避免重复监听
            for (int i = 0; i < ChannelArr.size(); i++) {
                Map<String,Object> ChannelObj = ChannelArr.get(i);
                String cd_id = ChannelObj.get("cd_id").toString();
                Object cd_concurrency = ChannelObj.get("cd_concurrency");//创建消费者数量
                cd_concurrency = cd_concurrency!=null&&cd_concurrency.toString().trim().length()>0?cd_concurrency:"1";
                int Threads = Integer.parseInt(cd_concurrency.toString());
                Object  isExecute = redisCache.getCacheObject("polling-Listener-dlxActivateDate-"+cd_id );
                if(isExecute==null || bool==false){
                    //redis 存储
                    ChannelObj.put("pollingtime", VeDate.getStringDateShort());
                    redisCache.setCacheObject("polling-Listener-dlxActivateDate-"+cd_id, ChannelObj);// 缓存 避免 重复创建 消费 监听
                    try {
                        List(Threads,cd_id,ChannelObj,channel);
                    }catch (Exception e){
                        log.error(">> ActivateDate createListener  失败:{} | {} | {}<<",e.getMessage(),cd_id,ChannelObj.toString());
                    }
                }
            }
        }
    }


    public void List(int threadCount,String cd_id, Map<String, Object> ChannelObj, Channel channel){
        ThreadPoolExecutor executor=null;
        try {
            //创建和线程同等数量的线程池
            executor= (ThreadPoolExecutor) Executors.newFixedThreadPool(threadCount);
            //阻塞，同步线程
            CountDownLatch countDownLatch=new CountDownLatch(threadCount);
            for(int i=0;i<threadCount;i++){
                log.info("=====启动 {}=====",Thread.currentThread().getName());
                //开始分配线程处理
                SynTask synTask=new SynTask(cd_id,ChannelObj,channel);
                executor.execute(synTask);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }finally {
            executor.shutdown();
            while(true){
                if(executor.isTerminated()){
                    break;
                }
            }
        }
    }



    @Async
    class SynTask implements Runnable {
        private String cd_id;
        private Map<String,Object> ChannelObj;
        private Channel channel;


        public SynTask(String Pcd_id, Map<String,Object> PChannelObj,Channel Pchannel) {
            this.cd_id  = Pcd_id ;
            this.ChannelObj = PChannelObj;
            this.channel = Pchannel;
        }

        /**
         * 消费者 激活时间 消费者
         * @param cd_id
         */
        public void activateConsumer(String cd_id,Map<String,Object> ChannelObj,Channel channel){
            try {
                String QUEUE_NAME = null,EXCHANGE_NAME = null,ROUTINGKEY = null;

                EXCHANGE_NAME = "polling_dlxcardActivateDate_exchange";
                QUEUE_NAME = "polling_dlxcardActivateDate_queue_"+cd_id;
                ROUTINGKEY = "polling.dlxcardActivateDate.routingKey."+cd_id;
                
                // 关联队列消费者关联队列
                channel.queueBind(QUEUE_NAME, EXCHANGE_NAME, ROUTINGKEY);
                DefaultConsumer defaultConsumer = new DefaultConsumer(channel) {
                    @Override
                    public void handleDelivery(String consumerTag, Envelope envelope, AMQP.BasicProperties properties, byte[] body) throws IOException {
                        String msg = new String(body, "UTF-8");
                        //System.out.println("polling_dlxcardActivateDate_queue_"+cd_id+" 消费者获取消息:" + msg);
                        synActivateDate(msg,channel);//同步激活时间
                    }
                };
                // 开始监听消息 自动签收
                channel.basicConsume(QUEUE_NAME, true, defaultConsumer);
            }catch (Exception e){
                log.error(">>DB - 激活时间 创建 轮序消费者 同步激活时间失败:{} | {}<<",e.getMessage().toString(),ChannelObj.toString());
                try {
                    redisCache.redisTemplate.delete("polling-Listener-dlxActivateDate-"+cd_id);
                    log.error(">> 创建 消费者失败 > 删除 redis记录Key polling-Listener-dlxActivateDate-{} | {}<<", cd_id);
                }catch (Exception e1){
                    log.error(">> 创建 消费者失败 > 删除 redis记录Key polling-Listener-dlxActivateDate-{} 失败 | {}<<", cd_id, e1.getMessage());
                }
            }
        }







        public void synActivateDate(String msg, Channel channel) throws IOException {
            try {
                if (StringUtils.isEmpty(msg)) {
                    return;
                }

                Map<String,Object> map = JSON.parseObject(msg);
                String iccid = map.get("iccid").toString();
                String polling_id = map.get("polling_id").toString();//轮询任务编号
                String prefix = "polling_cardActivateDate_queue";
                //执行前判断 redis 是否存在 执行数据 存在时 不执行
                Object  isExecute = redisCache.getCacheObject(prefix+":"+ iccid);
                if(isExecute==null){
                    //System.out.println("SUCCESS");
                    redisCache.setCacheObject(prefix+":"+ iccid, msg, 3, TimeUnit.MINUTES);//3 分钟缓存 避免 重复消费
                    redisCache.setCacheObject(polling_id+":"+ iccid, msg, 4, TimeUnit.HOURS);//4 小时缓存 用来统计轮序进度


                    Map<String,Object> Parammap=new HashMap<>();
                    Parammap.put("iccid",iccid);
                    Map<String,Object> Rmap =  internalApiRequest.queryCardActiveTime(Parammap,map);
                    String code =  Rmap.get("code")!=null?Rmap.get("code").toString():"500";
                    if(code.equals("200")){
                        //获取 激活日期 开卡日期 更新 card info
                        if(Rmap.get("activateDate")!=null &&  Rmap.get("activateDate")!="" &&  Rmap.get("activateDate").toString().trim().length()>0){
                            String activateDate = Rmap.get("activateDate").toString().trim();
                            Object openDate = Rmap.get("openDate")!=null &&  Rmap.get("openDate")!="" &&  Rmap.get("openDate").toString().trim().length()>0?
                                    Rmap.get("openDate").toString().trim():null;
                            Map<String,Object> Upd_Map = new HashMap<>();
                            Upd_Map.put("activate_date",activateDate);
                            if(openDate!=null){
                                Upd_Map.put("open_date",openDate);
                            }
                            Upd_Map.put("iccid",iccid);
                            //1.创建路由 绑定 生产队列 发送消息
                            String card_exchangeName = "admin_card_exchange", card_queueName = "admin_CardUpd_queue", card_routingKey = "admin.CardUpd.queue",
                                    card_del_exchangeName = "dlx_"+card_exchangeName,card_del_queueName = "dlx_"+card_queueName, card_del_routingKey = "dlx_"+card_routingKey;
                            try {
                                //rabbitMQConfig.creatExchangeQueue(card_exchangeName, card_queueName, card_routingKey, card_del_exchangeName, card_del_queueName, card_del_routingKey,null);
                                /*Upd_Map.put("queueTypeName","admin_CardUpdActivate_queue");
                                rabbitTemplate.convertAndSend(card_exchangeName, card_routingKey, JSON.toJSONString(Upd_Map), message -> {
                                    // 设置消息过期时间 30 分钟 过期
                                    message.getMessageProperties().setExpiration("" + (30 * 1000 * 60));
                                    return message;
                                });*/
                                int upd = yzCardMapper.updActivate(Upd_Map);
                                log.info(">>发送 dlx变更激活时间 指令成功 ！ iccid:{} | upd:{} | Upd_Map {}<<", iccid,upd, Upd_Map);
                            } catch (Exception e) {
                                System.out.println("变更激活时间 队列 创建 失败 " + e.getMessage().toString());
                            }
                        }
                    }else{
                        log.info(">>API - 激活时间轮序消费者 未获取到激活日期:{} | {} | Parammap {} | map {}<<",iccid,Rmap,Parammap,map);
                        // System.out.println(map.get("iccid")+" 未获取到激活日期 ！");
                    }
                }
            } catch (Exception e) {
//            // 记录该消息日志形式  存放数据库db中、后期通过定时任务实现消息补偿、人工实现补偿
                log.error(">>错误 - 激活时间轮序消费者:{}<<", e.getMessage().toString());
//            //将该消息存放到死信队列中，单独写一个死信消费者实现消费。
            }
        }
        @Override
        public void run() {
            activateConsumer(cd_id,ChannelObj,channel);
        }
    }


}
