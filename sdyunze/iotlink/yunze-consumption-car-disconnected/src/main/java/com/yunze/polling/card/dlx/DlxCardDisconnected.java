package com.yunze.polling.card.dlx;

import com.alibaba.fastjson.JSON;
import com.yunze.apiCommon.utils.InternalApiRequest;
import com.yunze.common.config.MyDictionary;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.redis.RedisCache;
import com.yunze.common.mapper.yunze.YzCardInfoChangeMapper;
import com.yunze.common.mapper.yunze.YzCardMapper;

import com.yunze.common.utils.yunze.CardFlowSyn;
import com.yunze.apiCommon.utils.VeDate;
import com.yunze.common.utils.yunze.GetShowStatIdArr;
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
 * 未订购资费停机轮询 轮询消费 者
 */
@Slf4j
@Component
public class DlxCardDisconnected {


    @Resource
    private InternalApiRequest internalApiRequest;
    @Resource
    private YzCardMapper yzCardMapper;
    @Resource
    private RedisCache redisCache;
    @Resource
    private RabbitMQConfig rabbitMQConfig;
    @Resource
    private CardFlowSyn cardFlowSyn;
    @Resource
    private YzCardInfoChangeMapper yzCardInfoChangeMapper;
    @Resource
    private MyDictionary myDictionary;
    @Resource
    private GetShowStatIdArr getShowStatIdArr;

    /**
     * 监听器
     * @param msg
     * @param channel_1
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "polling_dlxcardCardDisconnected_queue", containerFactory = "customContainerFactory")
    public void pollingStart(String msg, Channel channel_1) throws IOException {
        if (msg != null && msg.length() > 0) {
            synCardDisconnected(msg, channel_1);
        }
    }



    /**
     * 同步未订购资费停机轮询
     *
     * @param msg
     * @param channel
     * @throws IOException
     */
    public void synCardDisconnected(String msg, Channel channel) throws IOException {
        try {
            if (StringUtils.isEmpty(msg)) {
                return;
            }

            Map<String, Object> map = JSON.parseObject(msg);
            String iccid = map.get("iccid").toString();


            String polling_id = map.get("polling_id").toString();//轮询任务编号
            String prefix = "polling_dlxcardCardDisconnected_queue";
            //执行前判断 redis 是否存在 执行数据 存在时 不执行
            //Object isExecute = redisCache.getCacheObject(prefix + ":" + iccid);
            //if (isExecute == null) {
                //System.out.println("SUCCESS");
                //redisCache.setCacheObject(prefix + ":" + iccid, msg, 3, TimeUnit.MINUTES);//3 分钟缓存 避免 重复消费
                redisCache.setCacheObject(polling_id + ":" + iccid, msg, 1, TimeUnit.HOURS);//1 小时缓存 用来统计轮询进度

                Map<String, Object> Parammap = new HashMap<>();
                Parammap.put("iccid", iccid);
                Parammap.put("Is_Stop", "off");
                Map<String, Object> Rmap = internalApiRequest.queryFlow(Parammap, map);
                String code = Rmap.get("code") != null ? Rmap.get("code").toString() : "500";
                if (code.equals("200")) {
                    //获取 卡用量 开卡日期 更新 card info
                    if (Rmap.get("Use") != null && Rmap.get("Use") != "" && Rmap.get("Use").toString().trim().length() > 0) {
                        Double Use = Double.parseDouble(Rmap.get("Use").toString());
                        if (Use >= 0) {
                            try {
                                Map<String, Object> cardFlowSynRMap = cardFlowSyn.CalculationFlow(iccid, Use);
                                log.info("Dlx - " + ">>cardFlowSyn - 未订购资费停机轮询-卡用量轮询消费者 同步卡用量返回:{} | {}<<", iccid, JSON.toJSON(cardFlowSynRMap));
                                Double remaining = Double.parseDouble(cardFlowSynRMap.get("remaining").toString());
                                if (remaining <= 0 && remaining != -1) {
                                    String status_id = map.get("status_id") != null && map.get("status_id") != "" && map.get("status_id").toString().length() > 0 ? map.get("status_id").toString() : "未知";
                                    Disconnected(iccid, Parammap, map, status_id);
                                } else {
                                    log.error("Dlx - >> 数据 同步卡用量失败:{} | {}<<", iccid, JSON.toJSON(cardFlowSynRMap));
                                }
                            } catch (Exception e) {
                                log.error("Dlx - >>cardFlowSyn - 未订购资费停机轮询-卡用量轮询消费者 同步卡用量失败:{} | {}<<", iccid, e.getMessage().toString());
                            }
                        } else {
                            log.info("Dlx - " + ">>API - 未订购资费停机轮询-卡用量轮询消费者 未获取到卡用量 statusCode = 0 :{} | {}<<", iccid, Rmap);
                        }
                    }
                } else {
                    log.info("Dlx - " + ">>API - 未订购资费停机轮询-卡用量轮询消费者 未获取到卡用量:{} | {}<<", iccid, Rmap);
                }
            //}
        } catch (Exception e) {
            log.error("Dlx - >>错误 - 未订购资费停机轮询轮询消费者:{}<<", e.getMessage().toString());
        }
    }







    /**
     * 停机
     * @param iccid
     * @param Parammap
     * @param map
     */
    public  void Disconnected (String iccid,Map<String,Object> Parammap,Map<String,Object> map,String status_id){
        Map<String,Object> Rmap =  internalApiRequest.changeCardStatus(Parammap,map);
        String code =  Rmap.get("code")!=null?Rmap.get("code").toString():"500";
        if(code.equals("200")){
            //获取 未订购资费停机轮询 开卡日期 更新 card info
            Map<String,Object> Upd_Map = new HashMap<>();
            Upd_Map.put("status_id","2");
            Upd_Map.put("iccid",iccid);
            String status_ShowId = getShowStatIdArr.GetShowStatId(status_id);
            Upd_Map.put("status_ShowId",status_ShowId);
            boolean bool = yzCardMapper.updStatusId(Upd_Map)>0;//变更未订购资费停机轮询
            if(bool){
                Map<String,Object> Add_Map = new HashMap<>();
                Add_Map.put("iccid",iccid);
                Add_Map.put("ctype","1");//生命周期
                Add_Map.put("cbefore",myDictionary.getdictLabel("yunze_card_status_ShowId",status_id));
                Add_Map.put("cafterward","停机");
                Add_Map.put("remark","");
                Add_Map.put("source_type","2");// 来源 类型 ：2 未订购轮询停机
                Add_Map.put("execution_status","1");
                boolean addBool =  yzCardInfoChangeMapper.save(Add_Map)>0;
                log.info("Dlx - "+map.get("iccid")+" 修改未订购资费停机轮询 bool ： "+bool+" 新增卡信息变更记录 addBool :"+addBool);
            }else{
                log.error("Dlx - >>DB - 未订购资费停机轮询轮询消费者 同步未订购资费停机轮询失败:{} | {}<<",iccid,Upd_Map);
            }
        }else{
            log.info("Dlx - "+">>API - 未订购资费停机轮询轮询消费者 未获取到未订购资费停机轮询:{} | {}<<",iccid,Rmap);
        }
    }



}





