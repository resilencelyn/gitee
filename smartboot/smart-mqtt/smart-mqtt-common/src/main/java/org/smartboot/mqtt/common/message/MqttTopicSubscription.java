package org.smartboot.mqtt.common.message;

import org.smartboot.mqtt.common.enums.MqttQoS;

public final class MqttTopicSubscription {
    /**
     * 主题过滤器
     */
    private final String topicFilter;
    private final MqttQoS qualityOfService;

    public MqttTopicSubscription(String topicFilter, MqttQoS qualityOfService) {
        this.topicFilter = topicFilter;
        this.qualityOfService = qualityOfService;
    }

    public String topicFilter() {
        return topicFilter;
    }

    public MqttQoS qualityOfService() {
        return qualityOfService;
    }
}