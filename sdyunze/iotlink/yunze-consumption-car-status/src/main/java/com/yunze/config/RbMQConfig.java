package com.yunze.config;

import org.springframework.amqp.core.AcknowledgeMode;
import org.springframework.amqp.rabbit.connection.ConnectionFactory;
import org.springframework.amqp.rabbit.listener.SimpleMessageListenerContainer;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import javax.annotation.Resource;

/**
 * @Auther: zhang feng
 * @Date: 2021/11/11/9:03
 * @Description:
 *
 *
 *
 */
/*@Configuration*/
public class RbMQConfig {

   /* @Resource
    private OneRunServiceImpl oneRunServiceImpl;
    @Resource
    private TowRunServiceImpl towRunServiceImpl;

    @Bean
    public SimpleMessageListenerContainer simpleMessageListenerContainer(ConnectionFactory connectionFactory) {
        SimpleMessageListenerContainer container = new SimpleMessageListenerContainer(connectionFactory);
        container.setDefaultRequeueRejected(false);// 是否重回队列
        container.setAcknowledgeMode(AcknowledgeMode.AUTO);// 签收模式 自动
        container.setMessageListener(oneRunServiceImpl);//设置监听类
        container.start();
        return container;
    }*/




}
