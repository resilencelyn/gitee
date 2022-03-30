package com.yunze.config;

import lombok.SneakyThrows;
import org.springframework.amqp.core.Message;
import org.springframework.amqp.core.MessageListener;
import org.springframework.stereotype.Service;

/**
 * 业务执行类
 * @Auther: zhang feng
 * @Date: 2021/11/11/9:02
 * @Description:
 */
@Service
public class OneRunServiceImpl implements MessageListener {


    @SneakyThrows
    @Override
    public void onMessage(Message message) {

        byte[] body = message.getBody();
        String msg = new String(body, "UTF-8");
        System.out.println("======[接收到消息 One ]==============================");
        System.out.println(msg);
    }
}
