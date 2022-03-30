package com.yunze.polling;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.Channel;
import com.yunze.polling.card.ActivateDate;
import com.yunze.polling.card.dlx.DlxActivateDate;
import lombok.extern.slf4j.Slf4j;
import org.springframework.amqp.core.Message;
import org.springframework.amqp.rabbit.annotation.RabbitHandler;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.io.IOException;
import java.util.Map;

/**
 * 激活时间轮序消费者
 */
@Component
@Slf4j
public class CardPollingConsumer {


    @Autowired
    private ActivateDate activateDate;

    @Autowired
    private DlxActivateDate dlxActivateDate;

    /**
     * 监听队列回调的方法
     *
     * @param msg
     */
    public void orderConsumer(String msg) {
        log.info(">>正常订单消费者消息 1111 MSG:{}<<", msg);
    }





    /**
     * 轮序开始类型监听器 通过该监听器灵活监听
     * @param msg
     * @param message
     * @param channel
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "polling_card_activatedate")
    public void pollingStart(String msg, Message message, Channel channel) throws IOException {

        Map<String,Object> map = JSON.parseObject(msg);
        String type = map.get("type").toString();
        //激活时间轮序
        if(type.equals("ActivateDate")){
            activateDate.createListener();
            dlxActivateDate.createListener();
        }/*else if(type.equals("CardStatus")){
            cardStatus.createListener(1);
            dlxCardStatus.createListener(1);
        }else if(type.equals("CardFlow")){
            cardFlow.createListener(2);
            dlxCardFlow.createListener(2);
        }*/
    }

}