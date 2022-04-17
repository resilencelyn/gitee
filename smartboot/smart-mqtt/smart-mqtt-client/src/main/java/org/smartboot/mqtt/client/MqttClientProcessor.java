package org.smartboot.mqtt.client;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.smartboot.mqtt.client.processor.ConnAckProcessor;
import org.smartboot.mqtt.client.processor.MqttAckProcessor;
import org.smartboot.mqtt.client.processor.MqttPingRespProcessor;
import org.smartboot.mqtt.client.processor.MqttProcessor;
import org.smartboot.mqtt.client.processor.PublishProcessor;
import org.smartboot.mqtt.common.message.MqttConnAckMessage;
import org.smartboot.mqtt.common.message.MqttMessage;
import org.smartboot.mqtt.common.message.MqttPingRespMessage;
import org.smartboot.mqtt.common.message.MqttPubAckMessage;
import org.smartboot.mqtt.common.message.MqttPubCompMessage;
import org.smartboot.mqtt.common.message.MqttPubRecMessage;
import org.smartboot.mqtt.common.message.MqttPubRelMessage;
import org.smartboot.mqtt.common.message.MqttPublishMessage;
import org.smartboot.mqtt.common.message.MqttSubAckMessage;
import org.smartboot.socket.StateMachineEnum;
import org.smartboot.socket.extension.processor.AbstractMessageProcessor;
import org.smartboot.socket.transport.AioSession;

import java.util.HashMap;
import java.util.Map;

/**
 * @author 三刀
 * @version V1.0 , 2018/4/24
 */
public class MqttClientProcessor extends AbstractMessageProcessor<MqttMessage> {
    private static final Logger LOGGER = LoggerFactory.getLogger(MqttClientProcessor.class);
    private final MqttClient mqttClient;
    private final Map<Class<? extends MqttMessage>, MqttProcessor<? extends MqttMessage>> processorMap = new HashMap<>();

    public MqttClientProcessor(MqttClient mqttClient) {
        this.mqttClient = mqttClient;
        processorMap.put(MqttConnAckMessage.class, new ConnAckProcessor());
        processorMap.put(MqttPubAckMessage.class, new MqttAckProcessor<MqttPubAckMessage>());
        processorMap.put(MqttPublishMessage.class, new PublishProcessor());
        processorMap.put(MqttPubRecMessage.class, new MqttAckProcessor<MqttPubRecMessage>());
        processorMap.put(MqttPubCompMessage.class, new MqttAckProcessor<MqttPubCompMessage>());
        processorMap.put(MqttPubRelMessage.class, new MqttAckProcessor<MqttPubRelMessage>());
        processorMap.put(MqttSubAckMessage.class, new MqttAckProcessor<MqttPubRelMessage>());
        processorMap.put(MqttPingRespMessage.class, new MqttPingRespProcessor());
    }

    @Override
    public void process0(AioSession session, MqttMessage msg) {
        mqttClient.getListeners().forEach(listener -> listener.onMessageReceived(mqttClient, msg));
        MqttProcessor processor = processorMap.get(msg.getClass());
        LOGGER.info("receive msg:{}", msg);
        if (processor != null) {
            processor.process(mqttClient, msg);
        } else {
            LOGGER.error("unknown msg:{}", msg);
        }
    }

    @Override
    public void stateEvent0(AioSession session, StateMachineEnum stateMachineEnum, Throwable throwable) {
        System.out.println(stateMachineEnum);
        if (throwable != null) {
            throwable.printStackTrace();
        }
    }
}
