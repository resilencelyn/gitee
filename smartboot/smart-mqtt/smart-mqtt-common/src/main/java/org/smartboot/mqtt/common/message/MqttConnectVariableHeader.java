/*
 * Copyright 2014 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package org.smartboot.mqtt.common.message;


import org.apache.commons.lang.builder.ToStringBuilder;
import org.apache.commons.lang.builder.ToStringStyle;
import org.smartboot.mqtt.common.enums.MqttVersion;

/**
 * CONNECT 报文的可变报头按下列次序包含四个字段：协议名（Protocol Name），协议级别（Protocol
 * Level），连接标志（Connect Flags）和保持连接（Keep Alive）。
 */
public final class MqttConnectVariableHeader {

    /**
     * 协议名
     */
    private final String protocolName;
    /**
     * 协议级别
     */
    private final byte protocolLevel;
    private final boolean hasUserName;
    private final boolean hasPassword;
    private final boolean isWillRetain;
    private final int willQos;
    private final boolean isWillFlag;
    private final boolean isCleanSession;
    private final int reserved;
    private final int keepAliveTimeSeconds;

    public MqttConnectVariableHeader(
            String name,
            byte protocolLevel,
            int connectFlag,
            int keepAliveTimeSeconds) {
        this.protocolName = name;
        this.protocolLevel = protocolLevel;
        this.hasUserName = (connectFlag & 0x80) == 0x80;
        this.hasPassword = (connectFlag & 0x40) == 0x40;
        this.isWillRetain = (connectFlag & 0x20) == 0x20;
        this.willQos = (connectFlag & 0x18) >> 3;
        this.isWillFlag = (connectFlag & 0x04) == 0x04;
        this.isCleanSession = (connectFlag & 0x02) == 0x02;
        this.reserved = (connectFlag & 0x01);
        this.keepAliveTimeSeconds = keepAliveTimeSeconds;
    }

    public MqttConnectVariableHeader(MqttVersion mqttVersion, boolean hasUserName, boolean hasPassword, WillMessage willMessage, boolean isCleanSession, int keepAliveTimeSeconds) {
        this.protocolName = mqttVersion.protocolName();
        this.protocolLevel = mqttVersion.protocolLevel();
        this.hasUserName = hasUserName;
        this.hasPassword = hasPassword;
        this.isWillFlag = willMessage != null;
        this.isWillRetain = isWillFlag && willMessage.isWillRetain();
        this.willQos = isWillFlag ? willMessage.getWillQos().value() : 0;
        this.isCleanSession = isCleanSession;
        //服务端必须验证 CONNECT 控制报文的保留标志位（第 0 位）是否为 0，如果不为 0 必须断开客户端连接
        this.reserved = 0;
        this.keepAliveTimeSeconds = keepAliveTimeSeconds;
    }

    public String protocolName() {
        return protocolName;
    }

    public byte getProtocolLevel() {
        return protocolLevel;
    }

    public boolean hasUserName() {
        return hasUserName;
    }

    public boolean hasPassword() {
        return hasPassword;
    }

    public boolean isWillRetain() {
        return isWillRetain;
    }

    public int willQos() {
        return willQos;
    }

    public boolean isWillFlag() {
        return isWillFlag;
    }

    public boolean isCleanSession() {
        return isCleanSession;
    }

    public int keepAliveTimeSeconds() {
        return keepAliveTimeSeconds;
    }

    public int getReserved() {
        return reserved;
    }

    @Override
    public String toString() {
        return ToStringBuilder.reflectionToString(this, ToStringStyle.SHORT_PREFIX_STYLE);
    }
}
