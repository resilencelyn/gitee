package com.yunze.polling.card;

import com.alibaba.fastjson.JSON;
import com.yunze.apiCommon.utils.InternalApiRequest;
import com.yunze.common.config.MyDictionary;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.card.CardChange;
import com.yunze.common.core.redis.RedisCache;

import com.yunze.common.utils.yunze.CardFlowSyn;
import com.yunze.apiCommon.utils.VeDate;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.connection.Connection;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Component;
import com.rabbitmq.client.AMQP;
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.DefaultConsumer;
import com.rabbitmq.client.Envelope;
import javax.annotation.Resource;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * 达量停机 轮序消费 者
 */
@Slf4j
@Component
public class CardStop {


    @Resource
    private InternalApiRequest internalApiRequest;
    @Resource
    private RedisCache redisCache;
    @Resource
    private RabbitMQConfig rabbitMQConfig;
    @Resource
    private CardFlowSyn cardFlowSyn;
    @Resource
    private MyDictionary myDictionary;
    @Resource
    private CardChange cardChange;


    /**
     * 轮序开始类型监听器 通过该监听器灵活监听
     * @param msg
     * @param channel_1
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "polling_card_Stop_one")
    public void pollingStart(String msg, Channel channel_1) throws IOException {
        Map<String, Object> map = JSON.parseObject(msg);
        String  Listener = map.get("Listener").toString();
        String  cd_id = map.get("cd_id").toString();
        RunEx(Listener,1,cd_id,false);//执行 公共部分
    }



    /**
     * 创建 监听
     * @param Threads 消费者创建 数量
     */
    public void createListener(int Threads,Map<String,Object> Pmap)  {
        //获取现在数据库中 开启轮序 状态正常的通道进行监听
        try {
            //获取 key 值对应的 监听 生产 队列
            String  Listener = Pmap.get("Listener").toString();
            String  cd_id = Pmap.get("cd_id").toString();
            RunEx(Listener,Threads,cd_id,true);
        }catch (Exception e){
            log.error(">>createListener - 未订购资费停机轮询 创建 失败:{} | {}<<",e.getMessage());
        }
    }


    /**
     * 执行 公共部分
     * @param Listener
     * @param Threads
     * @param cd_id
     * @param bool 是否过滤 不需要监听的
     */
    public void RunEx(String Listener,int Threads,String cd_id, boolean bool){

        //获取现在数据库中 开启轮序 状态正常的通道进行监听
        try {
            //获取 key 值对应的 监听 生产 队列
            Object  isExecute = redisCache.getCacheObject(Listener);
            if(isExecute==null || bool==false){
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
                //redis 存储
                Map<String,Object> oMap = new HashMap<>();
                oMap.put("pollingtime", VeDate.getStringDateShort());
                redisCache.setCacheObject(Listener, oMap);// 缓存 避免 重复创建 消费 监听
                List(Threads,cd_id,channel);
            }
        }catch (Exception e){
            log.error(">> {} - 未订购资费停机轮询 创建 失败:cd_id {} | {}<<",Listener,cd_id,e.getMessage());
        }
    }





    public void List(int threadCount,String cd_id,Channel channel){
        ThreadPoolExecutor executor=null;
        try {
            //创建和线程同等数量的线程池
            executor= (ThreadPoolExecutor) Executors.newFixedThreadPool(threadCount);
            //阻塞，同步线程
            CountDownLatch countDownLatch=new CountDownLatch(threadCount);
            for(int i=0;i<threadCount;i++){
                log.info("=====启动 {}=====",Thread.currentThread().getName());
                //开始分配线程处理
                SynTask synTask=new SynTask(cd_id,channel);
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
        private Channel channel;


        public SynTask(String Pcd_id,Channel Pchannel) {
            this.cd_id  = Pcd_id ;
            this.channel = Pchannel;
        }






    /**
     * 创建 消费者 达量停机
     * @param cd_id
     */
    public void activateConsumer(String cd_id,Channel channel){
        try {
            String QUEUE_NAME = null,EXCHANGE_NAME = null,ROUTINGKEY = null;

            EXCHANGE_NAME = "polling_cardCardStop_exchange";
            QUEUE_NAME = "polling_cardCardStop_queue_"+cd_id;
            ROUTINGKEY = "polling.cardCardStop.routingKey."+cd_id;

            // 关联队列消费者关联队列
            channel.queueBind(QUEUE_NAME, EXCHANGE_NAME, ROUTINGKEY);
            DefaultConsumer defaultConsumer = new DefaultConsumer(channel) {
                @Override
                public void handleDelivery(String consumerTag, Envelope envelope, AMQP.BasicProperties properties, byte[] body) throws IOException {
                    String msg = new String(body, "UTF-8");
                    //System.out.println("polling_cardCardStop_queue_"+cd_id+" 消费者获取消息:" + msg);
                    synCardStop(msg,channel);//同步达量停机
                }
            };
            // 开始监听消息 自动签收
            channel.basicConsume(QUEUE_NAME, true, defaultConsumer);
        }catch (Exception e){
            log.error(">>DB - 达量停机 创建 轮序消费者 同步达量停机失败:{} | {}<<",e.getMessage().toString());
        }
    }


    /**
     * 同步达量停机
     * @param msg
     * @param channel
     * @throws IOException
     */
    public void synCardStop(String msg, Channel channel) throws IOException {
        try {
            if (StringUtils.isEmpty(msg)) {
                return;
            }

            Map<String,Object> map = JSON.parseObject(msg);
            String iccid = map.get("iccid").toString();
            boolean is_sel = (boolean) map.get("is_sel");

            Double Max =  Double.parseDouble(map.get("Max").toString());



            String polling_id = map.get("polling_id").toString();//轮询任务编号
            String prefix = "polling_cardCardStop_queue";
            //执行前判断 redis 是否存在 执行数据 存在时 不执行
            Object  isExecute = redisCache.getCacheObject(prefix+":"+ iccid);
            if(isExecute==null){
                //System.out.println("SUCCESS");
                redisCache.setCacheObject(prefix+":"+ iccid, msg, 3, TimeUnit.MINUTES);//3 分钟缓存 避免 重复消费
                redisCache.setCacheObject(polling_id+":"+ iccid, msg, 4, TimeUnit.HOURS);//4 小时缓存 用来统计轮序进度

                Map<String,Object> Parammap=new HashMap<>();
                Parammap.put("iccid",iccid);
                Parammap.put("Is_Stop","off");
                //不需要查询的 已经超出的直接执行停机
                if(is_sel){
                    Map<String,Object> Rmap =  internalApiRequest.queryFlow(Parammap,map);
                    String code =  Rmap.get("code")!=null?Rmap.get("code").toString():"500";
                    if(code.equals("200")){
                        //获取 卡用量 开卡日期 更新 card info
                        if(Rmap.get("Use")!=null &&  Rmap.get("Use")!="" &&  Rmap.get("Use").toString().trim().length()>0){
                            Double Use = Double.parseDouble(Rmap.get("Use").toString());
                            if(Use>=0){
                                try {
                                    Map<String,Object> RMap = cardFlowSyn.CalculationFlow(iccid,Use);
                                    log.info(">>cardFlowSyn - 达量停机-卡用量轮序消费者 同步卡用量返回:{} | {}<<",iccid,JSON.toJSON(RMap));
                                    Double used =  Double.parseDouble(RMap.get("used").toString());
                                    log.info(">>cardFlowSyn - {} | used {} | Max {}  | used>=Max {} <<",iccid,used,Max,used>=Max);
                                    if(used>=Max){
                                        stop(iccid,Parammap,map,map.get("status_id").toString());
                                    }
                                }catch (Exception e){
                                    log.error(">>cardFlowSyn - 达量停机-卡用量轮序消费者 同步卡用量失败:{} | {}<<",iccid,e.getMessage().toString());
                                }
                            }else{
                                log.info(">>API - 达量停机-卡用量轮序消费者 未获取到卡用量 statusCode = 0 :{} | {}<<",iccid,Rmap);
                            }
                        }
                    }else{
                        log.info(">>API - 达量停机-卡用量轮序消费者 未获取到卡用量:{} | {}<<",iccid,Rmap);
                    }
                }else{
                    String status_id = map.get("status_id")!=null?map.get("status_id").toString():"8";//未知
                    stop(iccid,Parammap,map,status_id);
                }
            }
        } catch (Exception e) {
            log.error(">>错误 - 达量停机轮序消费者:{} | {} <<", e.getMessage(),msg);
        }
    }


    /**
     * 停机
     * @param iccid
     * @param Parammap
     * @param map
     */
    public  void stop (String iccid,Map<String,Object> Parammap,Map<String,Object> map,String status_id){
        Map<String,Object> Add_Map = new HashMap<>();
        Add_Map.put("iccid",iccid);
        Add_Map.put("ctype","2");//生命周期
        Add_Map.put("cbefore",myDictionary.getdictLabel("yunze_card_status_ShowId",status_id));
        Add_Map.put("cafterward","停机");
        Add_Map.put("source_type","1");// 来源 类型 ：1 达量停机

        Map<String,Object> Upd_Map = new HashMap<>();
        Upd_Map.put("status_id","2");
        Upd_Map.put("iccid",iccid);
        cardChange.OnOff(Parammap,map,Add_Map,Upd_Map);


    }

        @Override
        public void run() {
            activateConsumer(cd_id,channel);
        }



    }




}
