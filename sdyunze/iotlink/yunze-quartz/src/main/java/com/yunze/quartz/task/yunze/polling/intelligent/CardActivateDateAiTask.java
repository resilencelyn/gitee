package com.yunze.quartz.task.yunze.polling.intelligent;

import com.alibaba.fastjson.JSON;
import com.yunze.apiCommon.mapper.YzCardRouteMapper;
import com.yunze.apiCommon.utils.VeDate;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.mapper.yunze.YzCardMapper;
import com.yunze.common.mapper.yunze.YzPassagewayPollingMapper;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 定时任务 通道轮询
 *
 * @author root
 */
@Component("cardActivateDateAiTask")
public class CardActivateDateAiTask {

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


    String ad_exchangeName = null, ad_queueName = null, ad_routingKey = null,
           ad_del_exchangeName = null,ad_del_queueName = null, ad_del_routingKey = null;







    /**
     * 轮询 激活时间
     * @param time
     * @param polling_type 轮询类型 【激活时间-智能-有用量 14 】【激活时间-智能-已发货 15 】
     * @param usedIsNull 用量是否大于 0
     * @param deliver_dateIsNull 发货时间是否为空
     */
    public void pollingActivateDateIntelligent(Integer time,String polling_type,String usedIsNull,String deliver_dateIsNull) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        findRouteID_Map.put("cd_algorithm", "2");//高频轮询
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);
                findMap.put("activate_date", "Null");//激活时间 为空时才获取
                findMap.put("used", usedIsNull);//用量
                findMap.put("deliver_date", deliver_dateIsNull);//发货时间

                List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCar(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    String polling_id = VeDate.getNo(4);
                    //插入 通道轮询详情表
                    Map<String, Object> pollingPublic_Map = new HashMap<>();
                    pollingPublic_Map.put("cd_id", cd_id);
                    pollingPublic_Map.put("cd_current", 0);
                    pollingPublic_Map.put("polling_type", polling_type);
                    //创建 路由 新增轮询详情 生产启动类型消息
                    ActivateDateRun(cardArr,channel_obj,polling_id,cd_id,time,pollingPublic_Map);
                }

            }
        }
    }




    /**
     * 轮询 激活时间 [激活时间-智能-未发货-已到沉默期]
     * @param time
     */
    public void pollingActivateDateIntelligent(Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        findRouteID_Map.put("cd_algorithm", "2");//高频轮询
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);

                List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCarSilencePeriod(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    String polling_id = VeDate.getNo(4);
                    //插入 通道轮询详情表
                    Map<String, Object> pollingPublic_Map = new HashMap<>();
                    pollingPublic_Map.put("cd_id", cd_id);
                    pollingPublic_Map.put("cd_current", 0);
                    pollingPublic_Map.put("polling_type", "16");
                    //创建 路由 新增轮询详情 生产启动类型消息
                    ActivateDateRun(cardArr,channel_obj,polling_id,cd_id,time,pollingPublic_Map);
                }

            }
        }
    }





    /**
     * 激活时间 任务发送消息
     * @param ActivateArr
     * @param channel_obj
     * @param polling_id
     * @param cd_id
     * @param time
     */
    public void ActivateDateRun(List<Map<String, Object>> ActivateArr, Map<String, Object> channel_obj, String polling_id,String cd_id,int time,Map<String, Object> pollingPublic_Map) {

          try {
              //设置任务 路由器 名称 与队列 名称
              ad_exchangeName = "polling_cardActivateDate_exchange";
              ad_queueName = "polling_cardActivateDate_queue";
              ad_routingKey = "polling.cardActivateDate.routingKey";
              ad_del_exchangeName = "polling_dlxcardActivateDate_exchange";
              ad_del_queueName = "polling_dlxcardActivateDate_queue" ;
              ad_del_routingKey = "polling.dlxcardActivateDate.routingKey";
              //rabbitMQConfig.creatExchangeQueue(ad_exchangeName, ad_queueName, ad_routingKey, ad_del_exchangeName, ad_del_queueName, ad_del_routingKey,null);

          } catch (Exception e) {
            System.out.println(e.getMessage().toString());
        }

        pollingPublic_Map.put("cd_count", ActivateArr.size());
        pollingPublic_Map.put("polling_id", polling_id);
        yzPassagewayPollingMapper.add(pollingPublic_Map);//新增 轮询详情表


        for (int j = 0; j < ActivateArr.size(); j++) {
            Map<String, Object> card = ActivateArr.get(j);
            Map<String, Object> Card = new HashMap<>();
            Card.putAll(channel_obj);
            Card.put("iccid", card.get("iccid"));
            Card.put("card_no", card.get("card_no"));
            Card.put("polling_id", polling_id);//轮询任务详情编号
            String msg = JSON.toJSONString(Card);
            //1. 更新激活时间 [生产任务]
            try {
                rabbitTemplate.convertAndSend(ad_exchangeName, ad_routingKey, msg, message -> {
                    // 设置消息过期时间 time 分钟 过期
                    message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                    return message;
                });
            } catch (Exception e) {
                System.out.println(e.getMessage().toString());
            }
        }
    }







}
