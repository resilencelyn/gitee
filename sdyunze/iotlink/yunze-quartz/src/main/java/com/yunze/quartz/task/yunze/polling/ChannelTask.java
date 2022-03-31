package com.yunze.quartz.task.yunze.polling;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.BuiltinExchangeType;
import com.yunze.apiCommon.mapper.YzCardRouteMapper;
import com.yunze.common.config.RabbitMQConfig;
import com.yunze.common.mapper.yunze.YzCardMapper;
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
 * 定时任务 通道轮询
 *
 * @author root
 */
@Component("channelTask")
public class ChannelTask {

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
    String polling_queueName = "polling_start_type";
    String polling_routingKey = "polling.start.type";
    String polling_exchangeName = "polling_card";

    //
    String ad_exchangeName = null, ad_queueName = null, ad_routingKey = null,
           ad_del_exchangeName = null,ad_del_queueName = null, ad_del_routingKey = null;


    /**
     * 轮询 还没有用量的数据  超出用量的 的生产者
     * type Slight .还没有用量的数据; 2.超出用量的 Serious
     *
     * @param type
     * @param time 多少 分钟 后失效
     */
    public void channelType(String type, Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);

        if (channelArr != null && channelArr.size() > 0) {
            //2.获取 通道下卡号
            boolean bool = false;
            bool = type.equalsIgnoreCase("Slight") ? true : false;// 轮询 还没有用量的数据 是 开启 激活时间 任务生产
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);
                findMap.put("type", type);
                List<Map<String, Object>> cardArr = yzCardMapper.channelCardSeriousAndSlight(findMap);
                if (cardArr != null && cardArr.size() > 0) {

                    //插入 通道轮询详情表
                    Map<String, Object> pollingPublic_Map = new HashMap<>();
                    pollingPublic_Map.put("cd_id", cd_id);
                    pollingPublic_Map.put("cd_current", 0);

                    //激活时间 任务生产
                    if (bool) {
                        findMap.put("Arr", cardArr);
                        List<Map<String, Object>> ActivateArr = yzCardMapper.activate_dateNull(findMap);// 获取激活时间为空 iccid
                        if (ActivateArr != null && ActivateArr.size() > 0) {
                            String polling_id = VeDate.getNo(4);
                            //创建 路由 新增轮询详情 生产启动类型消息
                            if(ActivateDate(time,ActivateArr.size(),polling_id,pollingPublic_Map)){
                                ActivateDateRun(ActivateArr,channel_obj,polling_id,cd_id,time);
                            }
                        }
                    }




                }

            }
        }
    }





    /**
     * 轮询 激活时间
     * type Slight .还没有用量的数据; 2.超出用量的 Serious
     * @param type
     * @param time 多少 分钟 后失效
     */
    public void pollingActivateDate(String type, Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            //2.获取 通道下卡号
            boolean bool = false;
            bool = type.equalsIgnoreCase("Slight") ? true : false;// 轮询 还没有用量的数据 是 开启 激活时间 任务生产
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);
                findMap.put("type", type);
                List<Map<String, Object>> cardArr = yzCardMapper.channelCardSeriousAndSlight(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    //插入 通道轮询详情表
                    Map<String, Object> pollingPublic_Map = new HashMap<>();
                    pollingPublic_Map.put("cd_id", cd_id);
                    pollingPublic_Map.put("cd_current", 0);
                    //激活时间 任务生产
                    // if (bool) {
                        findMap.put("Arr", cardArr);
                        List<Map<String, Object>> ActivateArr = yzCardMapper.activate_dateNull(findMap);// 获取激活时间为空 iccid
                        if (ActivateArr != null && ActivateArr.size() > 0) {
                            String polling_id = VeDate.getNo(4);
                            //创建 路由 新增轮询详情 生产启动类型消息
                            if(ActivateDate(time,ActivateArr.size(),polling_id,pollingPublic_Map)){
                                ActivateDateRun(ActivateArr,channel_obj,polling_id,cd_id,time);
                            }
                        }
                    //  }

                }
            }
        }
    }



    /**
     * 轮询 卡状态
     * @param time 多少 分钟 后失效
     */
    public void pollingCardStatus(Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);
        if (channelArr != null && channelArr.size() > 0) {
            String CardStatus_routingKey = "";
            try {
                //设置任务 路由器 名称 与队列 名称
                ad_exchangeName = "polling_cardCardStatus_exchange";
                ad_queueName = "polling_cardCardStatus_queue" ;
                CardStatus_routingKey = "polling.cardCardStatus.routingKey";
                ad_del_exchangeName = "polling_dlxcardCardStatus_exchange";
                ad_del_queueName = "polling_dlxcardCardStatus_queue";
                ad_del_routingKey = "polling.dlxcardCardStatus.routingKey";
                rabbitMQConfig.creatExchangeQueue(ad_exchangeName, ad_queueName, CardStatus_routingKey, ad_del_exchangeName, ad_del_queueName, ad_del_routingKey,null);
            } catch (Exception e) {
                System.out.println(e.getMessage().toString());
            }

            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                String cd_id = channel_obj.get("cd_id").toString();
                findMap.put("channel_id", cd_id);
                List<Map<String, Object>> cardArr = yzCardMapper.findChannelIdCar(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    //插入 通道轮询详情表
                    Map<String, Object> pollingPublic_Map = new HashMap<>();
                    pollingPublic_Map.put("cd_id", cd_id);
                    pollingPublic_Map.put("cd_current", 0);

                    //卡状态 用量 轮询
                    String polling_id_CardStatus = VeDate.getNo(4);
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
                        } catch (Exception e) {
                            System.out.println(e.getMessage().toString());
                        }
                    }
                }
            }
        }
    }





















    //  通道 轮询 3.正常使用的卡号数据
    public void channel(Integer time) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);

        if (channelArr != null && channelArr.size() > 0) {
            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                findMap.put("channel_id", channel_obj.get("cd_id"));
                String cd_id = channel_obj.get("cd_id").toString();

                List<Map<String, Object>> cardArr = yzCardMapper.channelCardCommonly(findMap);
                if (cardArr != null && cardArr.size() > 0) {



                    //插入 通道轮询详情表
                    Map<String, Object> pollingPublic_Map = new HashMap<>();
                    pollingPublic_Map.put("cd_id", cd_id);
                    pollingPublic_Map.put("cd_current", 0);

                    //卡状态 用量 轮询
                    String polling_id_CardStatus = VeDate.getNo(4);
                    //创建 路由 新增轮询详情 生产启动类型消息
                    //boolean CardStatusBool =  CardStatus(time, cardArr.size(), polling_id_CardStatus,pollingPublic_Map);

                    String CardStatus_routingKey = "";
                    String CardFlow_routingKey = "";

                    try {
                            ad_exchangeName = "polling_cardCardStatus_exchange";
                            ad_queueName = "polling_cardCardStatus_queue";
                            CardStatus_routingKey = "polling.cardCardStatus.routingKey";
                            ad_del_exchangeName = "polling_dlxcardCardStatus_exchange";
                            ad_del_queueName = "polling_dlxcardCardStatus_queue";
                            ad_del_routingKey = "polling.dlxcardCardStatus.routingKey";
                            rabbitMQConfig.creatExchangeQueue(ad_exchangeName, ad_queueName, CardStatus_routingKey, ad_del_exchangeName, ad_del_queueName, ad_del_routingKey, null);
                    }catch (Exception e) {
                        System.out.println(e.getMessage());
                    }

                    //卡号放入路由
                    for (int j = 0; j < cardArr.size(); j++) {
                        Map<String, Object> card = cardArr.get(j);
                        Map<String, Object> Card = new HashMap<>();
                        Card.putAll(channel_obj);
                        Card.put("iccid", card.get("iccid"));
                        Card.put("card_no", card.get("card_no"));
                        String msg = JSON.toJSONString(Card);
                        //生产任务
                        //2.卡状态
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
        }
    }


    //  通道 轮询 2.超过百分之多少的数据进行轮询生产
    public void channelFindPercentage(String percentage) {
        //1.状态 正常 轮询开启 时 获取  每个 通道下卡号 加入队列
        Map<String,Object> findRouteID_Map = new HashMap<>();
        findRouteID_Map.put("FindCd_id",null);
        List<Map<String, Object>> channelArr = yzCardRouteMapper.findRouteID(findRouteID_Map);

        if (channelArr != null && channelArr.size() > 0) {
            //2.获取 通道下卡号
            for (int i = 0; i < channelArr.size(); i++) {
                Map<String, Object> channel_obj = channelArr.get(i);
                Map<String, Object> findMap = new HashMap<>();
                findMap.put("channel_id", channel_obj.get("cd_id"));
                findMap.put("percentage", percentage);
                List<Map<String, Object>> cardArr = yzCardMapper.findPercentage(findMap);
                if (cardArr != null && cardArr.size() > 0) {
                    //卡号放入路由
                    for (int j = 0; j < cardArr.size(); j++) {
                        Map<String, Object> card = cardArr.get(j);
                        Map<String, Object> Card = new HashMap<>();
                        Card.putAll(channel_obj);
                        Card.put("iccid", card.get("iccid"));
                        Card.put("card_no", card.get("card_no"));
                        String msg = JSON.toJSONString(Card);


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
     * @return
     */
    public boolean CardStatus(int time, int size, String polling_id,Map<String, Object> pollingPublic_Map) {
        boolean bool = true;
        //卡状态  轮询
        try {
            rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null, BuiltinExchangeType.FANOUT);
            Map<String, Object> start_type = new HashMap<>();
            start_type.put("type", "CardStatus");
            rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                // 设置消息过期时间 time 分钟 过期
                message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                return message;
            });
            pollingPublic_Map.put("polling_type", "2");
            pollingPublic_Map.put("cd_count", size);
            pollingPublic_Map.put("polling_id", polling_id);
            yzPassagewayPollingMapper.add(pollingPublic_Map);//新增 轮询详情表

        } catch (Exception e) {
            bool = false;
            System.out.println("生产 轮询 [CardStatus] 启动类型 失败 " + e.getMessage().toString());
        }
        return bool;
    }

    /**
     * 创建 激活时间 轮询 生产者路由队列 及 数据库 插入 轮询信息
     * @param time
     * @param size
     * @param polling_id
     * @param pollingPublic_Map
     * @return
     */
    public boolean ActivateDate(int time, int size, String polling_id,Map<String, Object> pollingPublic_Map) {
        boolean bool = true;
        try {
            rabbitMQConfig.creatExchangeQueue(polling_exchangeName, polling_queueName, polling_routingKey, null, null, null,BuiltinExchangeType.FANOUT);
            Map<String, Object> start_type = new HashMap<>();
            start_type.put("type", "ActivateDate");
            rabbitTemplate.convertAndSend(polling_exchangeName, polling_routingKey, JSON.toJSONString(start_type), message -> {
                // 设置消息过期时间 time 分钟 过期
                message.getMessageProperties().setExpiration("" + (time * 1000 * 60));
                return message;
            });
            pollingPublic_Map.put("polling_type", "1");
            pollingPublic_Map.put("cd_count", size);
            pollingPublic_Map.put("polling_id", polling_id);
            yzPassagewayPollingMapper.add(pollingPublic_Map);//新增 轮询详情表
        } catch (Exception e) {
            System.out.println("生产 轮询 [ActivateDate] 启动类型 失败 " + e.getMessage().toString());
        }
        return bool;
    }


    /**
     * 激活时间 任务发送消息
     * @param ActivateArr
     * @param channel_obj
     * @param polling_id
     * @param cd_id
     * @param time
     */
    public void ActivateDateRun(List<Map<String, Object>> ActivateArr, Map<String, Object> channel_obj, String polling_id,String cd_id,int time) {
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
                if (j == 0) {
                    //设置任务 路由器 名称 与队列 名称
                    ad_exchangeName = "polling_cardActivateDate_exchange";
                    ad_queueName = "polling_cardActivateDate_queue";
                    ad_routingKey = "polling.cardActivateDate.routingKey";
                    ad_del_exchangeName = "polling_dlxcardActivateDate_exchange";
                    ad_del_queueName = "polling_dlxcardActivateDate_queue";
                    ad_del_routingKey = "polling.dlxcardActivateDate.routingKey";
                    rabbitMQConfig.creatExchangeQueue(ad_exchangeName, ad_queueName, ad_routingKey, ad_del_exchangeName, ad_del_queueName, ad_del_routingKey,null);
                }
                //rabbitMQConfig.send(exchangeName,queueName,routingKey,"direct",msg);
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
