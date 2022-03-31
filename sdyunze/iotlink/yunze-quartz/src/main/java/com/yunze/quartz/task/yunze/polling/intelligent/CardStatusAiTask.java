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
@Component("cardStatusAiTask")
public class CardStatusAiTask {

    @Resource
    private YzCardRouteMapper yzCardRouteMapper;
    @Resource
    private YzCardMapper yzCardMapper;
    @Resource
    private RabbitTemplate rabbitTemplate;

    @Resource
    private YzPassagewayPollingMapper yzPassagewayPollingMapper;




    //
    String ad_exchangeName = null, ad_queueName = null, ad_routingKey = null,
           ad_del_exchangeName = null,ad_del_queueName = null, ad_del_routingKey = null;






    /**
     * 轮询 卡状态 [卡状态-智能-已激活 ][卡状态-智能-未激活-有用量] [卡状态-智能-未激活-已发货]
     * @param time 多少 分钟 后失效
     * @param activate_dateIsNull 激活时间是否为空
     * @param polling_type 轮询类型 【卡状态-智能-已激活 10 】【卡状态-智能-未激活-有用量 11 】【卡状态-智能-未激活-已发货 13】
     * @param usedIsNull 用量是否大于 0
     * @param deliver_dateIsNull 发货时间是否为空
     */
    public void pollingCardStatus(Integer time,String activate_dateIsNull,String polling_type,String usedIsNull,String deliver_dateIsNull) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        findRouteID_Map.put("cd_algorithm", "2");//智能轮询

        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            String CardStatus_routingKey = "polling.cardCardStatus.routingKey";

           /* try {
                //设置任务 路由器 名称 与队列 名称
                ad_exchangeName = "polling_cardCardStatus_exchange";
                ad_queueName = "polling_cardCardStatus_queue" ;

                ad_del_exchangeName = "polling_dlxcardCardStatus_exchange";
                ad_del_queueName = "polling_dlxcardCardStatus_queue";
                ad_del_routingKey = "polling.dlxcardCardStatus.routingKey";
                // rabbitMQConfig.creatExchangeQueue(ad_exchangeName, ad_queueName, CardStatus_routingKey, ad_del_exchangeName, ad_del_queueName, ad_del_routingKey,null);
            }catch (Exception e){
                System.out.println(e.getMessage().toString());
            }*/

            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);
                findMap.put("activate_date", activate_dateIsNull);//激活时间
                findMap.put("used", usedIsNull);//用量
                findMap.put("deliver_date", deliver_dateIsNull);//发货时间

                List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCar(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    pollingExecute(cd_id,cardArr,channel_obj,CardStatus_routingKey,time,polling_type);
                }
            }
        }
    }






    /**
     * 轮询 卡状态 [卡状态-智能-未激活-未发货-已到沉默期]
     * @param time 多少 分钟 后失效
     */
    public void pollingCardStatus(Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        findRouteID_Map.put("cd_algorithm", "2");//智能轮询

        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            String CardStatus_routingKey = "polling.cardCardStatus.routingKey";


            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);


                List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCarSilencePeriod(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    pollingExecute(cd_id,cardArr,channel_obj,CardStatus_routingKey,time,"12");
                }
            }
        }
    }


    /**
     * 轮询 卡状态 状态未知获取 数据
     * @param time
     */
    public void pollingCardStatusNull(Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);

        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            String  CardStatus_routingKey = "polling.cardCardStatus.routingKey";


            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);
                findMap.put("status_ShowId", "8");// 状态 未知
                List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCarStatusShowId(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    pollingExecute(cd_id,cardArr,channel_obj,CardStatus_routingKey,time,"17");
                }
                findMap.put("status_ShowId", "Null");// 状态 为空
                List<Map<String, Object>> cardArr_1 = yzCardMapper.findChannelIdCarStatusShowId(findMap);
                if (cardArr_1 != null && cardArr_1.size() > 0) {
                    pollingExecute(cd_id,cardArr_1,channel_obj,CardStatus_routingKey,time,"17");
                }
            }
        }
    }























    /**
     * 公共执行
     * @param cd_id
     * @param cardArr
     * @param channel_obj
     * @param CardStatus_routingKey
     * @param time
     * @param polling_type
     */
    public void pollingExecute(String cd_id,List<Map<String, Object>> cardArr,Map<String, Object> channel_obj,String CardStatus_routingKey,Integer time,String polling_type){
        //插入 通道轮询详情表
        Map<String, Object> pollingPublic_Map = new HashMap<>();
        pollingPublic_Map.put("cd_id", cd_id);
        pollingPublic_Map.put("cd_current", 0);

        //卡状态 用量 轮询
        String polling_id_CardStatus = VeDate.getNo(4);

        pollingPublic_Map.put("polling_type", polling_type);
        pollingPublic_Map.put("cd_count", cardArr.size());
        pollingPublic_Map.put("polling_id", polling_id_CardStatus);
        yzPassagewayPollingMapper.add(pollingPublic_Map);//新增 轮询详情表

        //创建 路由 新增轮询详情 生产启动类型消息

        //2.卡状态
        //卡号放入路由
        for (int j = 0; j < cardArr.size(); j++) {
            Map<String, Object> card = cardArr.get(j);
            Map<String, Object> Card = new HashMap<>();
            Card.putAll(channel_obj);
            Card.put("iccid", card.get("iccid"));
            Card.put("card_no", card.get("card_no"));
            Card.put("polling_id", polling_id_CardStatus);//轮询任务详情编号
            String msg = JSON.toJSONString(Card);
            //生产任务
            try {
                rabbitTemplate.convertAndSend("polling_cardCardStatus_exchange", CardStatus_routingKey, msg, message -> {
                    // 设置消息过期时间 time 分钟 过期
                    message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                    return message;
                });
                //rabbitMQConfig.send(exchangeName,queueName,routingKey,"direct",msg);
            } catch (Exception e) {
                System.out.println(e.getMessage().toString());
            }
        }
    }





}
