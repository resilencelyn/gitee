package com.yunze.quartz.task.yunze.polling;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.BuiltinExchangeType;
import com.yunze.common.config.RabbitMQConfig;
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
 * 定时任务 通道轮序
 *
 * @author root
 */
@Component("cardFlowTask")
public class CardFlowTask {

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

    //卡轮询 路由队列
    String polling_queueName = "polling_card_flow";
    String polling_routingKey = "polling.card.flow";
    String polling_exchangeName = "polling_card";

    //
    String ad_exchangeName = null, ad_queueName = null, ad_routingKey = null,
            ad_del_exchangeName = null, ad_del_queueName = null, ad_del_routingKey = null;


    /**
     * 轮序 用量 指定单个通道创建 消费者 【用来补救 消费者灵活监听时 没有创建 成功 消费者 进行 手动 补救创建 消费者】
     * @param time 多少 分钟 后失效
     */
    public void CardFlowCreateListener(Integer time, String cd_id) {





        Map<String, Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id", cd_id);
        //生产任务
        try {
            rabbitTemplate.convertAndSend("polling_cardCardFlow_exchange",  "admin.CardFlow.queue.One",  JSON.toJSONString(findRouteID_Map), message -> {
                // 设置消息过期时间 time 分钟 过期
                message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                return message;
            });
        } catch (Exception e) {
            System.out.println(e.getMessage().toString());
        }
    }



    /**
     * 轮序 用量 指定单个通道轮询
     *
     * @param time 多少 分钟 后失效
     */
    public void pollingCardFlowOne(Integer time, String cd_id) {

        Map<String, Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id", cd_id);
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);

        Map<String, Object> findMap = new HashMap<>();
        findMap.put("channel_id", cd_id);
        List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCar(findMap);
        if (cardArr != null && cardArr.size() > 0) {
            //插入 通道轮序详情表
            Map<String, Object> pollingPublic_Map = new HashMap<>();
            pollingPublic_Map.put("cd_id", cd_id);
            pollingPublic_Map.put("cd_current", 0);

            //卡状态 用量 轮询
            String polling_id_CardFlow = VeDate.getNo(4);
            //创建 路由 新增轮序详情 生产启动类型消息
            pollingPublic_Map.put("polling_type", "3");
            pollingPublic_Map.put("cd_count", cardArr.size());
            pollingPublic_Map.put("polling_id", polling_id_CardFlow);
            yzPassagewayPollingMapper.add(pollingPublic_Map);//新增 轮序详情表
            //2.卡状态
            //卡号放入路由
            //设置任务 路由器 名称 与队列 名称
            String card_exchangeName = "", card_queueName = "", card_routingKey = "",
                    card_del_exchangeName = "", card_del_queueName = ""  , card_del_routingKey = "";

            try {
                card_exchangeName = "polling_cardCardFlow_exchange";
                card_queueName = "polling_cardCardFlow_queue_" + cd_id;
                card_routingKey = "polling.cardCardFlow.routingKey." + cd_id;
                card_del_exchangeName = "polling_dlxcardCardFlow_exchange";
                card_del_queueName = "polling_dlxcardCardFlow_queue_" + cd_id;
                card_del_routingKey = "polling.dlxcardCardFlow.routingKey." + cd_id;
                rabbitMQConfig.creatExchangeQueue(card_exchangeName, card_queueName, card_routingKey, card_del_exchangeName, card_del_queueName, card_del_routingKey, null);
                System.out.println("pollingCardFlowOne 创建 【" + channelArr.get(0) + "】 成功 ！");
            } catch (Exception e) {
                System.out.println("pollingCardFlowOne 队列 创建 【" + channelArr.get(0) + "】 失败 " + e.getMessage());
            }


            for (int j = 0; j < cardArr.size(); j++) {
                Map<String, Object> card = cardArr.get(j);
                Map<String, Object> Card = new HashMap<>();
                Card.putAll(channelArr.get(0));
                Card.put("iccid", card.get("iccid"));
                Card.put("card_no", card.get("card_no"));
                Card.put("polling_id", polling_id_CardFlow);//轮序任务详情编号
                String msg = JSON.toJSONString(Card);
                //生产任务
                try {
                    rabbitTemplate.convertAndSend(card_exchangeName, card_routingKey, msg, message -> {
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


    /**
     * 轮序 卡状态
     *
     * @param time 多少 分钟 后失效
     */
    public void pollingCardFlow(Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String, Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id", null);
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);
                List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCar(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    //插入 通道轮序详情表
                    Map<String, Object> pollingPublic_Map = new HashMap<>();
                    pollingPublic_Map.put("cd_id", cd_id);
                    pollingPublic_Map.put("cd_current", 0);

                    //卡状态 用量 轮询
                    String polling_id_CardFlow = VeDate.getNo(4);
                    //创建 路由 新增轮序详情 生产启动类型消息
                    boolean CardFlowBool = CardFlow(time, cardArr.size(), polling_id_CardFlow, pollingPublic_Map);
                    String CardFlow_routingKey = "";
                    //2.卡状态
                    //卡号放入路由
                    for (int j = 0; j < cardArr.size(); j++) {
                        Map<String, Object> card = cardArr.get(j);
                        Map<String, Object> Card = new HashMap<>();
                        Card.putAll(channel_obj);
                        Card.put("iccid", card.get("iccid"));
                        Card.put("card_no", card.get("card_no"));
                        Card.put("polling_id", polling_id_CardFlow);//轮序任务详情编号
                        String msg = JSON.toJSONString(Card);
                        //生产任务
                        try {
                            if (j == 0) {
                                //设置任务 路由器 名称 与队列 名称
                                if (CardFlowBool) {
                                    ad_exchangeName = "polling_cardCardFlow_exchange";
                                    ad_queueName = "polling_cardCardFlow_queue_" + cd_id;
                                    CardFlow_routingKey = "polling.cardCardFlow.routingKey." + cd_id;
                                    ad_del_exchangeName = "polling_dlxcardCardFlow_exchange";
                                    ad_del_queueName = "polling_dlxcardCardFlow_queue_" + cd_id;
                                    ad_del_routingKey = "polling.dlxcardCardFlow.routingKey." + cd_id;
                                    rabbitMQConfig.creatExchangeQueue(ad_exchangeName, ad_queueName, CardFlow_routingKey, ad_del_exchangeName, ad_del_queueName, ad_del_routingKey, null);
                                }
                            }
                            if (CardFlowBool) {
                                rabbitTemplate.convertAndSend("polling_cardCardFlow_exchange", CardFlow_routingKey, msg, message -> {
                                    // 设置消息过期时间 time 分钟 过期
                                    message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                                    return message;
                                });
                            }
                            //rabbitMQConfig.send(exchangeName,queueName,routingKey,"direct",msg);
                        } catch (Exception e) {
                            System.out.println(e.getMessage().toString());
                        }
                    }
                }
            }
        }
    }


    /**
     * 创建 卡状态 轮询 生产者路由队列 及 数据库 插入 轮询信息
     *
     * @param time
     * @param size
     * @param polling_id
     * @param pollingPublic_Map
     * @return
     */
    public boolean CardFlow(Integer time, Integer size, String polling_id, Map<String, Object> pollingPublic_Map) {
        boolean bool = true;
        //卡状态  轮询
        try {
            rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null, BuiltinExchangeType.FANOUT);
            Map<String, Object> start_type = new HashMap<>();
            start_type.put("type", "CardFlow");
            rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                // 设置消息过期时间 time 分钟 过期
                message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                return message;
            });
            pollingPublic_Map.put("polling_type", "3");
            pollingPublic_Map.put("cd_count", size);
            pollingPublic_Map.put("polling_id", polling_id);
            yzPassagewayPollingMapper.add(pollingPublic_Map);//新增 轮序详情表
        } catch (Exception e) {
            bool = false;
            System.out.println("生产 轮序 [CardFlow] 启动类型 失败 " + e.getMessage().toString());
        }
        return bool;
    }


    /**
     * 直接再次启动
     *
     * @param time
     * @return
     */
    public boolean StartCardFlow(Integer time) {
        boolean bool = true;
        //卡状态  轮询
        try {
            rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null, BuiltinExchangeType.FANOUT);
            Map<String, Object> start_type = new HashMap<>();
            start_type.put("type", "CardFlow");
            rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                // 设置消息过期时间 time 分钟 过期
                message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                return message;
            });
        } catch (Exception e) {
            bool = false;
            System.out.println("生产 轮序 [StartCardFlow] 启动类型 失败 " + e.getMessage().toString());
        }
        return bool;
    }


}
