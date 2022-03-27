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
package com.iohao.little.game.net.client.core;

import com.alipay.remoting.ConnectionEventType;
import com.alipay.remoting.rpc.RpcClient;
import com.iohao.little.game.net.client.BoltClientServer;
import com.iohao.little.game.net.client.BoltClientServerSetting;
import com.iohao.little.game.net.client.common.ClientConnectEventProcessor;
import com.iohao.little.game.net.client.common.ClientDisConnectEventProcessor;
import com.iohao.little.game.net.client.common.ClientRequestMessageAsyncUserProcessor;
import lombok.experimental.UtilityClass;

/**
 * @author 洛朱
 * @date 2022-01-12
 */
@UtilityClass
class CommonClientStartup {
    void registerUserProcessor(BoltClientServer boltClientServer) {
        RpcClient rpcClient = boltClientServer.getRpcClient();

        BoltClientServerSetting boltClientServerSetting = boltClientServer.getBoltClientServerSetting();
        ClientRequestMessageAsyncUserProcessor processor = new ClientRequestMessageAsyncUserProcessor();
        processor.setBoltClientServerSetting(boltClientServerSetting);

        rpcClient.registerUserProcessor(processor);
    }

    void connectionEventProcessor(BoltClientServer boltClientServer) {

        ClientConnectEventProcessor clientConnectEventProcessor = new ClientConnectEventProcessor();
        ClientDisConnectEventProcessor clientDisConnectProcessor = new ClientDisConnectEventProcessor();

        BoltClientServerSetting boltClientServerSetting = boltClientServer.getBoltClientServerSetting();
        var moduleKey = boltClientServerSetting.getModuleKey();
        clientConnectEventProcessor.setModuleKey(moduleKey);

        RpcClient rpcClient = boltClientServer.getRpcClient();
        rpcClient.addConnectionEventProcessor(ConnectionEventType.CONNECT, clientConnectEventProcessor);
        rpcClient.addConnectionEventProcessor(ConnectionEventType.CLOSE, clientDisConnectProcessor);
    }

}
