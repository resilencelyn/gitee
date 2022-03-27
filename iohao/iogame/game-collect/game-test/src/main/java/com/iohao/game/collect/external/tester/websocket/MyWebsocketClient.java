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
package com.iohao.game.collect.external.tester.websocket;

import cn.hutool.core.util.StrUtil;
import com.iohao.game.collect.common.GameConfig;
import com.iohao.game.collect.external.tester.module.LoginVerifyOnMessage;
import com.iohao.game.collect.external.tester.module.OnMessage;
import com.iohao.game.collect.external.tester.module.tank.TankEnterRoomOnMessage;
import com.iohao.game.collect.external.tester.module.tank.TankMoveOnMessage;
import com.iohao.little.game.action.skeleton.core.CmdKit;
import com.iohao.little.game.common.kit.ProtoKit;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import lombok.extern.slf4j.Slf4j;
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.drafts.Draft_6455;
import org.java_websocket.handshake.ServerHandshake;

import java.net.URI;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * @author 洛朱
 * @date 2022-01-13
 */
@Slf4j
public class MyWebsocketClient {
    WebSocketClient webSocketClient;

    final Map<Integer, OnMessage> onMessageMap = new HashMap<>();

    public static void main(String[] args) throws Exception {
        MyWebsocketClient websocketClient = MyWebsocketClient.me();
        websocketClient.start();
    }

    private MyWebsocketClient() {
        put(LoginVerifyOnMessage.me());

        put(TankEnterRoomOnMessage.me());

        put(TankMoveOnMessage.me());
    }

    public void start() throws Exception {
        this.init();
        webSocketClient.connect();
        log.info("start ws://127.0.0.1:10088/websocket");
    }

    public void send(ExternalMessage request) {
        byte[] bytes = ProtoKit.toBytes(request);
        webSocketClient.send(bytes);
    }

    public void send(OnMessage onMessage) {
        Object requestData = onMessage.requestData();

        log.info("requestData : {}", requestData);
        ExternalMessage externalMessage = onMessage.createExternalMessage();
//        externalMessage.setCmdMerge(0);
//        externalMessage.dataContent = null;

        byte[] bytes = ProtoKit.toBytes(externalMessage);
        webSocketClient.send(bytes);
    }

    void init() throws Exception {
        var url = "ws://{}:{}" + GameConfig.websocketPath;

        var wsUrl = StrUtil.format(url, GameConfig.externalIp, GameConfig.externalPort);
        log.info("ws url : {}", wsUrl);
        webSocketClient = new WebSocketClient(new URI(wsUrl), new Draft_6455()) {

            @Override
            public void onOpen(ServerHandshake serverHandshake) {
                // 建立连接后 执行的方法
                MyWebsocketClient.me().send(LoginVerifyOnMessage.me());
            }

            @Override
            public void onMessage(ByteBuffer byteBuffer) {
                // 接收消息
                byte[] dataContent = byteBuffer.array();

                ExternalMessage message = ProtoKit.parseProtoByte(dataContent, ExternalMessage.class);
                int cmdMerge = message.getCmdMerge();
                byte[] data = message.getDataContent();

                OnMessage onMessage = onMessageMap.get(cmdMerge);

                log.info("client 收到消息 {}", message);

                if (Objects.nonNull(onMessage)) {
                    Object bizData = onMessage.process(message, data);
                    int cmd = CmdKit.getCmd(cmdMerge);
                    int subCmd = CmdKit.getSubCmd(cmdMerge);
                    String onMessageName = onMessage.getClass().getSimpleName();
                    log.info("{}-{} {}  {}", cmd, subCmd, onMessageName, bizData);
                } else {
                    log.info("不存在处理类 onMessage: ");
                }
            }

            @Override
            public void onMessage(String msg) {
                // 接收消息
                log.info("收到 String 消息========== {}", msg);
            }

            @Override
            public void onClose(int i, String s, boolean b) {
                log.info("链接已关闭: [{}] - [{}] - [{}]", i, s, b);
            }

            @Override
            public void onError(Exception e) {
                log.error(e.getMessage(), e);
                e.printStackTrace();
                System.out.println("发生错误已关闭");
            }
        };

    }



    private void put(OnMessage onMessage) {
        onMessageMap.put(onMessage.getCmdMerge(), onMessage);
    }

    public static MyWebsocketClient me() {
        return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final MyWebsocketClient ME = new MyWebsocketClient();
    }
}
