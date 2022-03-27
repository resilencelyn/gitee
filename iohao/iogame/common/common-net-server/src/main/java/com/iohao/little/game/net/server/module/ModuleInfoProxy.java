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
package com.iohao.little.game.net.server.module;

import com.alipay.remoting.exception.RemotingException;
import com.alipay.remoting.rpc.RpcServer;
import com.iohao.little.game.action.skeleton.protocol.RequestMessage;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.net.common.BoltServer;
import com.iohao.little.game.net.message.common.ModuleMessage;
import lombok.Getter;

/**
 * 模块信息代理
 * <pre>
 *     这里的模块指的是逻辑服信息
 * </pre>
 *
 * @author 洛朱
 * @Date 2021-12-20
 */
public class ModuleInfoProxy {

    @Getter
    final ModuleMessage moduleMessage;
    BoltServer boltServer;
    RpcServer rpcServer;
    final String address;

    public ModuleInfoProxy(ModuleMessage moduleMessage) {
        this.moduleMessage = moduleMessage;
        this.address = moduleMessage.getAddress();
    }

    public Object invokeSync(RequestMessage requestMessage) throws RemotingException, InterruptedException {

        Object result = rpcServer.invokeSync(address, requestMessage, 1000);

        return result;
    }

    public void oneway(Object request) throws RemotingException, InterruptedException {
        rpcServer.oneway(address, request);
    }

    public Object invokeSync(ResponseMessage responseMessage) throws RemotingException, InterruptedException {
        Object result;
        result = rpcServer.invokeSync(address, responseMessage, 1000);

        return result;
    }

    public void setBoltServer(BoltServer boltServer) {
        this.boltServer = boltServer;
        this.rpcServer = boltServer.getRpcServer();
    }
}
