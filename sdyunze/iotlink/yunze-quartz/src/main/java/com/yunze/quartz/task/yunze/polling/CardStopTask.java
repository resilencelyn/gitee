package com.yunze.quartz.task.yunze.polling;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.BuiltinExchangeType;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.core.redis.RedisCache;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.apiCommon.mapper.YzCardRouteMapper;
import com.yunze.common.mapper.yunze.YzPassagewayPollingMapper;
import com.yunze.apiCommon.utils.VeDate;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 定时任务 通道轮序 达量停机
 *
 * @author root
 */
@Component("cardStopTask")
public class CardStopTask {

    @Resource
    private YzCardRouteMapper yzCardRouteMapper;
    @Resource
    private YzCardMapper yzCardMapper;
    @Resource
    private RabbitTemplate rabbitTemplate;

    @Resource
    private YzPassagewayPollingMapper yzPassagewayPollingMapper;

    @Resource
    private RabbitMQConfig rabbitMQConfig;
    @Resource
    private RedisCache redisCache;
    //卡轮询 路由队列
    String polling_queueName = "polling_card_stop";
    String polling_routingKey = "polling.card.stop";
    String polling_exchangeName = "polling_card";

    //
    String ad_exchangeName = null, ad_queueName = null, ad_routingKey = null,
           ad_del_exchangeName = null,ad_del_queueName = null, ad_del_routingKey = null;





    /**
     * 轮序 卡状态
     * @param time 多少 分钟 后失效
     */
    public void pollingCardStop(Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);
                List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCarStop(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    //插入 通道轮序详情表
                    Map<String, Object> pollingPublic_Map = new HashMap<>();
                    pollingPublic_Map.put("cd_id", cd_id);
                    pollingPublic_Map.put("cd_current", 0);

                    //卡状态 用量 轮询
                    String polling_id_CardStop = VeDate.getNo(4);
                    //创建 路由 新增轮序详情 生产启动类型消息
                    Map<String, Object> start_type = new HashMap<String, Object>();
                    start_type.put("Listener","polling_cardCardStop_queue_" + cd_id);
                    start_type.put("cd_id", cd_id);
                    boolean CardStopBool =  CardStop(time, cardArr.size(), polling_id_CardStop,pollingPublic_Map,start_type);
                    String CardStop_routingKey = "";
                    //String CardFlow_routingKey = "";
                    //2.卡状态
                    //卡号放入路由
                    for (int j = 0; j < cardArr.size(); j++) {
                        Map<String, Object> card = cardArr.get(j);
                        Map<String, Object> Card = new HashMap<>();
                        Card.putAll(channel_obj);
                        Card.put("iccid", card.get("iccid"));
                        Card.put("card_no", card.get("card_no"));
                        Card.put("status_id", card.get("status_id"));

                        Card.put("polling_id", polling_id_CardStop);//轮序任务详情编号
                        Object remind_ratio =  card.get("remind_ratio");
                        Object used =  card.get("used");
                        boolean is_sel = true;//是否需要查询
                        String YyyyAndMm[] = VeDate.getYyyyAndMm();
                        String day = YyyyAndMm[2];
                        //默认赋值停机值
                        if(remind_ratio!=null && remind_ratio.toString().length()>0){
                            Card.put("Max", card.get("remind_ratio"));
                        }
                        //月初一号  或 27号 通用出账日 、 联通出账日 需要更新最新用量
                        if(Integer.parseInt(day)!=1 && Integer.parseInt(day)!=27){
                            //并且同步时间等于今天
                            Object syn_Time = card.get("syn_Time");
                            if(syn_Time!=null){
                                String Ymd = YyyyAndMm[0]+"-"+YyyyAndMm[1]+"-"+YyyyAndMm[2];
                                if(syn_Time.toString().equals(Ymd)){
                                    if(remind_ratio!=null && used!=null && remind_ratio.toString().trim().length()>0 && used.toString().trim().length()>0){
                                        Double d_Max  =  Double.parseDouble(remind_ratio.toString().trim());
                                        Double d_used  =  Double.parseDouble(used.toString().trim());
                                        is_sel = d_used>d_Max?false:true;//用量大于 最大值 时 不需要查询直接停机
                                        Card.put("Max", d_Max);
                                    }
                                }
                            }
                        }

                        Card.put("is_sel", is_sel);//是否需要接口同步用量
                        String msg = JSON.toJSONString(Card);
                        //生产任务
                        try {
                            if (j == 0) {
                                //设置任务 路由器 名称 与队列 名称
                                if(CardStopBool){ //卡状态
                                    ad_exchangeName = "polling_cardCardStop_exchange";
                                    ad_queueName = "polling_cardCardStop_queue_" + cd_id;
                                    CardStop_routingKey = "polling.cardCardStop.routingKey." + cd_id;
                                    ad_del_exchangeName = "polling_dlxcardCardStop_exchange";
                                    ad_del_queueName = "polling_dlxcardCardStop_queue_" + cd_id;
                                    ad_del_routingKey = "polling.dlxcardCardStop.routingKey." + cd_id;
                                    rabbitMQConfig.creatExchangeQueue(ad_exchangeName, ad_queueName, CardStop_routingKey, ad_del_exchangeName, ad_del_queueName, ad_del_routingKey,null);

                                    redisCache.setCacheObject("polling-Listener-CardStop-"+cd_id, ad_queueName+","+ad_del_queueName);// 缓存 避免 重复创建 消费 监听
                                }
                            }
                            if(CardStopBool){ //卡状态
                                rabbitTemplate.convertAndSend("polling_cardCardStop_exchange", CardStop_routingKey, msg, message -> {
                                    // 设置消息过期时间 time 分钟 过期
                                    message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                                    return message;
                                });
                            }
                            //rabbitMQConfig.send(exchangeName,queueName,routingKey,"direct",msg);
                        } catch (Exception e) {
                            System.out.println(e.getMessage());
                        }
                    }
                }
            }
        }
    }


    /**
     * 创建 卡状态 轮询 生产者路由队列 及 数据库 插入 轮询信息
     * @param time
     * @param size
     * @param polling_id
     * @param pollingPublic_Map
     * @param start_type
     * @return
     */
    public boolean CardStop(int time, int size, String polling_id,Map<String, Object> pollingPublic_Map,Map<String, Object> start_type) {
        boolean bool = true;
        //卡状态  轮询
        try {
            rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null, BuiltinExchangeType.FANOUT);
            start_type.put("type", "CardStop");
            rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                // 设置消息过期时间 time 分钟 过期
                message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                return message;
            });
            pollingPublic_Map.put("polling_type", "4");
            pollingPublic_Map.put("cd_count", size);
            pollingPublic_Map.put("polling_id", polling_id);
            yzPassagewayPollingMapper.add(pollingPublic_Map);//新增 轮序详情表

        } catch (Exception e) {
            bool = false;
            System.out.println("生产 轮序 [CardStop] 启动类型 失败 " + e.getMessage().toString());
        }
        return bool;
    }












}
