package com.yunze.polling.card;

import org.springframework.amqp.core.BindingBuilder;
import org.springframework.amqp.core.FanoutExchange;
import org.springframework.amqp.core.Queue;
import org.springframework.amqp.rabbit.core.RabbitAdmin;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.rabbit.listener.SimpleMessageListenerContainer;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;

/**
 * @Auther: zhang feng
 * @Date: 2021/11/11/9:01
 * @Description:
 */
/*@Service*/
public class AutoListener {

    /*@Resource
    private SimpleMessageListenerContainer messageListenerContainer;

    public void A(){

        // 删除已经创建的 名称与绑定
        *//*Queue queue = new Queue("polling_dlxcardCardStatus_queue_30");
        FanoutExchange exchange = new FanoutExchange("polling_cardCardStatus_exchange");

       rabbitAdmin.declareQueue(queue);
        rabbitAdmin.declareExchange(exchange);
        rabbitAdmin.declareBinding(BindingBuilder.bind(queue).to(exchange));*//*
        //创建 监听
        messageListenerContainer.addQueueNames("polling_dlxcardCardStatus_queue_1");
        messageListenerContainer.addQueueNames("polling_dlxcardCardStatus_queue_12");
        messageListenerContainer.start();
        System.out.println("A====");
    }
*/




}
