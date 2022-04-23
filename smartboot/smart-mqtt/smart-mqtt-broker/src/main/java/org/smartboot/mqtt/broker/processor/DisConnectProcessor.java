package org.smartboot.mqtt.broker.processor;

import org.smartboot.mqtt.broker.BrokerContext;
import org.smartboot.mqtt.broker.MqttSession;
import org.smartboot.mqtt.common.message.MqttDisconnectMessage;

/**
 * DISCONNECT 报文是客户端发给服务端的最后一个控制报文。表示客户端正常断开连接。
 *
 * @author 三刀（zhengjunweimail@163.com）
 * @version V1.0 , 2022/3/27
 */
public class DisConnectProcessor extends AuthorizedMqttProcessor<MqttDisconnectMessage> {
    @Override
    public void process0(BrokerContext context, MqttSession session, MqttDisconnectMessage message) {
        //服务端在收到 DISCONNECT 报文时：
        // 1. 必须丢弃任何与当前连接关联的未发布的遗嘱消息，具体描述见 3.1.2.5 节 [MQTT-3.14.4-3]。
        // 2. 应该关闭网络连接，如果客户端 还没有这么做。
        session.setWillMessage(null);
        session.disconnect();
    }
}
