/*
 * # iohao.com . 渔民小镇
 * Copyright (C) 2021 - 2022 double joker （262610965@qq.com） . All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License..
 */
package com.iohao.little.game.net.external.bootstrap.handler.codec;

import cn.hutool.core.util.ArrayUtil;
import com.iohao.little.game.action.skeleton.core.CmdKit;
import com.iohao.little.game.common.kit.ProtoKit;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessageCmdCode;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToMessageCodec;
import io.netty.handler.codec.http.websocketx.BinaryWebSocketFrame;
import lombok.extern.slf4j.Slf4j;

import java.util.List;
import java.util.Objects;

/**
 * websocket  编解码 for C#
 *
 * @author 洛朱
 * @date 2022-01-22
 */
@Slf4j
@ChannelHandler.Sharable
public class ExternalCodecWebsocketProtoForCSharp extends MessageToMessageCodec<BinaryWebSocketFrame, ExternalMessage> {
    @Override
    protected void encode(ChannelHandlerContext ctx, ExternalMessage msg, List<Object> out) throws Exception {
        if (Objects.isNull(msg)) {
            throw new Exception("The encode ExternalMessage is null");
        }

        // 编码器 - ExternalMessage ---> 字节数组
        byte[] bytes = ProtoKit.toBytes(msg);
        log.info("=== array 发送 客户端消息=== {}", ArrayUtil.toString(bytes));
        log.info("=== array 发送 客户端消息=== {}", msg);
        ByteBuf byteBuf = Unpooled.wrappedBuffer(bytes);

        BinaryWebSocketFrame socketFrame = new BinaryWebSocketFrame(byteBuf);

        out.add(socketFrame);
    }

    @Override
    protected void decode(ChannelHandlerContext ctx, BinaryWebSocketFrame binary, List<Object> out) {
        // 解码器 - 字节数组 ---> ExternalMessage
        ByteBuf content = binary.content();
        byte[] msgBytes = new byte[content.readableBytes()];
        content.readBytes(msgBytes);

        ExternalMessage message = ProtoKit.parseProtoByte(msgBytes, ExternalMessage.class);
        log.info("=== array 接收 客户端消息=== {}", ArrayUtil.toString(msgBytes));
        log.info("=== array 接收 客户端消息=== {}", message);
        out.add(message);
    }

    public static void main(String[] args) {
        /*
        {"cmdCode":1,"protocolSwitch":0,"cmdMerge":65537,"responseStatus":0,"validMsg":null,"dataContent":null}
[8, 1, 24, 129, 128, 4]
         */
        ExternalMessage externalMessage = new ExternalMessage();
        externalMessage.setCmdCode(ExternalMessageCmdCode.biz);
        externalMessage.setProtocolSwitch(0);
        externalMessage.setCmdMerge(CmdKit.merge(1,1));
        externalMessage.setResponseStatus(0);

//        externalMessage.setCmdMerge(0);

        byte[] bytes = ProtoKit.toBytes(externalMessage);
        System.out.println(externalMessage);
        System.out.println(ArrayUtil.toString(bytes));


        // 1 ExternalMessage(cmdCode=1, protocolSwitch=0, cmdMerge=65537, responseStatus=0, validMsg=null, dataContent=null)
        // 1 [8, 2, 16, 0, 24, -126, -128, 8, 32, 0]

        // 2 ExternalMessage(cmdCode=1, protocolSwitch=0, cmdMerge=0, responseStatus=0, validMsg=null, dataContent=null)
        // 2 [8, 2, 16, 0, 24, 0, 32, 0]
    }

    public static void main1(String[] args) {
//        ByteBuf byteBuf =

        byte[] javaBytes = new byte[]{8, 2, 16, 0, 24, -126, -128, 8, 32, 0, 50, 6, 10, 4, 116, 101, 115, 116};
        int len = javaBytes.length;
        ByteBuf byteBuf = Unpooled.wrappedBuffer(javaBytes);
        ByteBuf bufferSharp = Unpooled.buffer(javaBytes.length);

        log.info("java: {}, {}", byteBuf.readableBytes(), javaBytes.length);

        for (int i = 0; i < len; i++) {
            short i1 = byteBuf.readUnsignedByte();
            // c# [8, 1, 24, 129, 128, 4, 50, 6, 10, 4, 116, 101, 115, 116]
            System.out.print(i1 + ", ");
            // [8, 2, 16, 0, 24, 130, 128, 8, 32, 0, 50, 6, 10, 4, 116, 101, 115, 116]

            bufferSharp.writeByte(i1);
        }
        System.out.println();

        byte[] sharpBytes = new byte[len];
        bufferSharp.readBytes(sharpBytes);
        // 期望值 [8, 2, 16, 0, 24, -126, -128, 8, 32, 0, 50, 6, 10, 4, 116, 101, 115, 116]
        log.info("bufferSharp {}", ArrayUtil.toString(sharpBytes));

        // java 8, 2, 16, 0, 24, 130, 128, 8, 32, 0, 50, 6, 10, 4, 116, 101, 115, 116



//        [8, 1, 24, 129, 128, 4, 50, 6, 10, 4, 116, 101, 115, 116]


    }

}
