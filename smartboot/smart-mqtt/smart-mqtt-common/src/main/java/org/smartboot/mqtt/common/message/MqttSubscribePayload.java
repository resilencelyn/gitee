package org.smartboot.mqtt.common.message;

import org.smartboot.mqtt.common.ToString;

import java.util.List;

public final class MqttSubscribePayload extends ToString {

    private List<MqttTopicSubscription> topicSubscriptions;

    public void setTopicSubscriptions(List<MqttTopicSubscription> topicSubscriptions) {
        this.topicSubscriptions = topicSubscriptions;
    }

    public List<MqttTopicSubscription> getTopicSubscriptions() {
        return topicSubscriptions;
    }

}