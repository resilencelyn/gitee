package com.yunze.polling;

import com.alibaba.fastjson.JSON;
import com.rabbitmq.client.Channel;
import com.yunze.polling.card.CardStop;
import com.yunze.polling.card.dlx.DlxCardStop;
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
    private CardStop cardStop;

    @Autowired
    private DlxCardStop dlxCardStop;



    /**
     * 轮序开始类型监听器 通过该监听器灵活监听
     * @param msg
     * @param message
     * @param channel
     * @throws IOException
     */
    @RabbitHandler
    @RabbitListener(queues = "polling_card_stop")
    public void pollingStart(String msg, Message message, Channel channel) throws IOException {

        Map<String,Object> map = JSON.parseObject(msg);
        String type = map.get("type").toString();
        //激活时间轮序
        if(type.equals("CardStop")){

            String  Listener = map.get("Listener").toString();
            if(Listener.indexOf("polling_cardCardStop_queue_")!=-1){
                cardStop.createListener(2,map);
            }else if(Listener.indexOf("dlx_polling_cardCardStop_queue_")!=-1){
                dlxCardStop.createListener(2,map);
            }else {
                System.out.println("=================【未找到队列！"+Listener+"】=================");
            }
        }
    }


}