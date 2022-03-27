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
package com.iohao.little.game.net.server.core;

import com.alipay.remoting.ConnectionEventType;
import com.iohao.little.game.net.common.BoltServer;
import com.iohao.little.game.net.server.processor.*;
import lombok.experimental.UtilityClass;

/**
 * 提供给 网关 一些默认配置
 *
 * @author 洛朱
 * @date 2022-01-12
 */
@UtilityClass
class CommonServerStartup {

    void connectionEventProcessor(BoltServer boltServer) {
        GateConnectionEventProcessor serverConnectProcessor = new GateConnectionEventProcessor();
        GateCloseConnectionEventProcessor serverDisConnectProcessor = new GateCloseConnectionEventProcessor();

        serverConnectProcessor.setServer(boltServer);
        boltServer.addConnectionEventProcessor(ConnectionEventType.CONNECT, serverConnectProcessor);
        boltServer.addConnectionEventProcessor(ConnectionEventType.CLOSE, serverDisConnectProcessor);
    }

    void registerUserProcessor(BoltServer boltServer) {
        // 处理 - 广播
        var gateBroadcastMessageAsyncUserProcess = new GateBroadcastMessageAsyncUserProcessor(boltServer);
        boltServer.registerUserProcessor(gateBroadcastMessageAsyncUserProcess);

        // 处理 - 模块注册
        var registerModuleProcessor = new GateRegisterModuleMessageAsyncUserProcessor(boltServer);
        boltServer.registerUserProcessor(registerModuleProcessor);

        // 处理 - 内部模块消息的转发
        var innerModuleMessageAsyncUserProcess = new GateInnerModuleMessageAsyncUserProcessor();
        boltServer.registerUserProcessor(innerModuleMessageAsyncUserProcess);

        // 处理 - 改变用户 id -- external server
        var gateChangeUserIdMessageAsyncUserProcess = new GateChangeUserIdMessageAsyncUserProcessor(boltServer);
        boltServer.registerUserProcessor(gateChangeUserIdMessageAsyncUserProcess);

        // 处理 - (接收真实用户的请求) 把对外服的请求转发到逻辑服
        var externalMessageProcessor = new GateExternalRequestMessageAsyncUserProcessor(boltServer);
        boltServer.registerUserProcessor(externalMessageProcessor);

        // 处理 - （响应真实用户的请求）把逻辑服的响应转发到对外服
        var responseMessageAsyncUserProcessor = new GateResponseMessageAsyncUserProcessor(boltServer);
        boltServer.registerUserProcessor(responseMessageAsyncUserProcessor);
    }

}
