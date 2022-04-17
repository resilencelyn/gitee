package org.smartboot.mqtt.common.message;

import org.smartboot.mqtt.common.util.ValidateUtils;
import org.smartboot.socket.transport.WriteBuffer;
import org.smartboot.socket.util.BufferUtils;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

/**
 * 连接服务端，客户端到服务端的网络连接建立后，客户端发送给服务端的第一个报文必须是 CONNECT 报文。
 *
 * @author 三刀
 * @version V1.0 , 2018/4/22
 */
public class MqttConnectMessage extends MqttMessage {
    /**
     * 可变报头
     */
    private MqttConnectVariableHeader mqttConnectVariableHeader;
    /**
     * 有效载荷
     */
    private MqttConnectPayload mqttConnectPayload;

    public MqttConnectMessage(MqttFixedHeader mqttFixedHeader) {
        super(mqttFixedHeader);
    }

    public MqttConnectMessage(MqttConnectVariableHeader mqttConnectVariableHeader, MqttConnectPayload mqttConnectPayload) {
        super(MqttFixedHeader.CONNECT_HEADER);
        this.mqttConnectVariableHeader = mqttConnectVariableHeader;
        this.mqttConnectPayload = mqttConnectPayload;
    }

    @Override
    public void decodeVariableHeader(ByteBuffer buffer) {
        //协议名
        //协议名是表示协议名 MQTT 的 UTF-8 编码的字符串。
        //MQTT 规范的后续版本不会改变这个字符串的偏移和长度。
        //如果协议名不正确服务端可以断开客户端的连接，也可以按照某些其它规范继续处理 CONNECT 报文。
        //对于后一种情况，按照本规范，服务端不能继续处理 CONNECT 报文
        final String protocolName = decodeString(buffer);

        //协议级别，8位无符号值
        final byte protocolLevel = buffer.get();

        //连接标志
        final int b1 = BufferUtils.readUnsignedByte(buffer);

        //保持连接
        final int keepAlive = decodeMsbLsb(buffer);

        mqttConnectVariableHeader = new MqttConnectVariableHeader(
                protocolName,
                protocolLevel,
                b1,
                keepAlive);
    }

    @Override
    public void decodePlayLoad(ByteBuffer buffer) {
        //客户端标识符
        // 客户端标识符 (ClientId) 必须存在而且必须是 CONNECT 报文有效载荷的第一个字段
        final String decodedClientId = decodeString(buffer);

        String decodedWillTopic = null;
        byte[] decodedWillMessage = null;
        //如果遗嘱标志被设置为 1，有效载荷的下一个字段是遗嘱主题（Will Topic）。
        // 遗嘱主题必须是 1.5.3 节定义的 UTF-8 编码字符串
        if (mqttConnectVariableHeader.isWillFlag()) {
            decodedWillTopic = decodeString(buffer, 0, 32767);
            decodedWillMessage = decodeByteArray(buffer);
        }
        String decodedUserName = null;
        byte[] decodedPassword = null;
        // 如果用户名（User Name）标志被设置为 1，有效载荷的下一个字段就是它。
        // 用户名必须是 1.5.3 节定义的 UTF-8 编码字符串 [MQTT-3.1.3-11]。
        // 服务端可以将它用于身份验证和授权。
        if (mqttConnectVariableHeader.hasUserName()) {
            decodedUserName = decodeString(buffer);
        }
        // 密码字段包含一个两字节的长度字段，
        // 长度表示二进制数据的字节数（不包含长度字段本身占用的两个字节），
        // 后面跟着 0 到 65535 字节的二进制数据。
        if (mqttConnectVariableHeader.hasPassword()) {
            decodedPassword = decodeByteArray(buffer);
        }

        mqttConnectPayload = new MqttConnectPayload(decodedClientId, decodedWillTopic, decodedWillMessage, decodedUserName, decodedPassword);
    }

    @Override
    public void writeTo(WriteBuffer writeBuffer) throws IOException {
        //VariableHeader
        byte[] clientIdBytes = encodeUTF8(mqttConnectPayload.clientIdentifier());
        //剩余长度等于可变报头的长度（10 字节）加上有效载荷的长度。
        int remainingLength = 10 + clientIdBytes.length;

        //遗嘱
        byte[] willTopicBytes = null;
        if (mqttConnectPayload.willTopic() != null) {
            willTopicBytes = encodeUTF8(mqttConnectPayload.willTopic());
            remainingLength += willTopicBytes.length + 2 + mqttConnectPayload.willMessageInBytes().length;
        }

        //用户名
        byte[] userNameBytes = null;
        if (mqttConnectPayload.userName() != null) {
            userNameBytes = encodeUTF8(mqttConnectPayload.userName());
            remainingLength += userNameBytes.length;
        }
        //密码
        if (mqttConnectPayload.passwordInBytes() != null) {
            remainingLength += 2 + mqttConnectPayload.passwordInBytes().length;
        }


        //第一部分：固定报头
        writeBuffer.writeByte(getFixedHeaderByte1(mqttFixedHeader));
        writeBuffer.write(encodeMBI(remainingLength));


        //第二部分：可变报头，10字节
        //协议名
        byte[] nameBytes = mqttConnectVariableHeader.protocolName().getBytes(StandardCharsets.UTF_8);
        ValidateUtils.isTrue(nameBytes.length == 4, "invalid protocol name");
        byte versionByte = mqttConnectVariableHeader.getProtocolLevel();
        writeBuffer.writeShort((short) nameBytes.length);
        writeBuffer.write(nameBytes);
        //协议级别
        writeBuffer.writeByte(versionByte);
        //连接标志
        byte connectFlag = 0x00;
        if (mqttConnectVariableHeader.hasUserName()) {
            connectFlag = (byte) 0x80;
        }
        if (mqttConnectVariableHeader.hasPassword()) {
            connectFlag |= 0x40;
        }
        if (mqttConnectVariableHeader.isWillFlag()) {
            connectFlag |= 0x04;
            connectFlag |= mqttConnectVariableHeader.willQos() << 3;
            if (mqttConnectVariableHeader.isWillRetain()) {
                connectFlag |= 0x20;
            }
        }
        if (mqttConnectVariableHeader.isCleanSession()) {
            connectFlag |= 0x02;
        }
        writeBuffer.writeByte(connectFlag);
        //保持连接
        writeBuffer.writeShort((short) mqttConnectVariableHeader.keepAliveTimeSeconds());

        //第三部分：有效载荷
        //客户端标识符 (ClientId) 必须存在而且必须是 CONNECT 报文有效载荷的第一个字段
        writeBuffer.write(clientIdBytes);

        //遗嘱
        if (willTopicBytes != null) {
            writeBuffer.write(willTopicBytes);
            writeBuffer.writeShort((short) mqttConnectPayload.willMessageInBytes().length);
            writeBuffer.write(mqttConnectPayload.willMessageInBytes());
        }
        //用户名
        if (userNameBytes != null) {
            writeBuffer.write(userNameBytes);
        }
        //密码
        if (mqttConnectPayload.passwordInBytes() != null) {
            writeBuffer.writeShort((short) mqttConnectPayload.passwordInBytes().length);
            writeBuffer.write(mqttConnectPayload.passwordInBytes());
        }
    }

    public MqttConnectPayload getPayload() {
        return mqttConnectPayload;
    }

    public MqttConnectVariableHeader getVariableHeader() {
        return mqttConnectVariableHeader;
    }
}
