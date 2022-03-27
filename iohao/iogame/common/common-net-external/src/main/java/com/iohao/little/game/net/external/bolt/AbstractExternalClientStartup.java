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
package com.iohao.little.game.net.external.bolt;

import com.alipay.remoting.rpc.RpcClient;
import com.iohao.little.game.action.skeleton.core.BarSkeleton;
import com.iohao.little.game.net.client.BoltClientServer;
import com.iohao.little.game.net.client.core.ClientStartup;
import com.iohao.little.game.net.external.bootstrap.ExternalServerKit;

/**
 * 抽象对外服启动流程
 *
 * @author 洛朱
 * @date 2022-02-04
 */
public abstract class AbstractExternalClientStartup implements ClientStartup {
    @Override
    public BarSkeleton createBarSkeleton() {
        return BarSkeleton
                .newBuilder()
                .build();
    }

    @Override
    public void registerUserProcessor(BoltClientServer boltClientServer) {

        RpcClient rpcClient = boltClientServer.getRpcClient();

        // 注册 广播处理器
        ExternalBroadcastMessageAsyncUserProcessor broadcastMessageAsyncUserProcessor = new ExternalBroadcastMessageAsyncUserProcessor();
        rpcClient.registerUserProcessor(broadcastMessageAsyncUserProcessor);

        // 注册 用户id变更处理
        ExternalSettingUserIdMessageAsyncUserProcessor changeUserIdMessageAsyncUserProcessor = new ExternalSettingUserIdMessageAsyncUserProcessor();
        rpcClient.registerUserProcessor(changeUserIdMessageAsyncUserProcessor);

        // 注册 接收网关消息处理
        ExternalResponseMessageAsyncUserProcessor responseMessageAsyncUserProcessor = new ExternalResponseMessageAsyncUserProcessor();
        rpcClient.registerUserProcessor(responseMessageAsyncUserProcessor);
    }

    @Override
    public void startupSuccess(BoltClientServer boltClientServer) {
        ExternalServerKit.setBoltClientServer(boltClientServer);
    }
}
