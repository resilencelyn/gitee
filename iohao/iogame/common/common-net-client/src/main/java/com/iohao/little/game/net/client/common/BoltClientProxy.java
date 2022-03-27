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
package com.iohao.little.game.net.client.common;

import com.alipay.remoting.Connection;
import com.alipay.remoting.exception.RemotingException;
import com.alipay.remoting.rpc.RpcClient;
import com.iohao.little.game.action.skeleton.core.BarSkeleton;
import com.iohao.little.game.action.skeleton.core.ServerContext;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import com.iohao.little.game.action.skeleton.protocol.RequestMessage;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.broadcast.Broadcast;
import com.iohao.little.game.net.client.BoltClientServer;
import com.iohao.little.game.net.message.common.InnerModuleMessage;
import lombok.Getter;
import lombok.Setter;
import lombok.experimental.Accessors;
import lombok.extern.slf4j.Slf4j;

import java.util.Collection;

/**
 * 客户端服务器代理, 持有一些属性
 * <pre>
 *     负责与网关通信
 * </pre>
 *
 * @author 洛朱
 * @Date 2021-12-17
 * @see BoltClientServer#init()
 */
@Getter
@Setter
@Accessors(chain = true)
@Slf4j
public class BoltClientProxy implements ServerContext {

    Connection connection;

    /**
     * rpcClient
     * <pre>
     *     {@link BoltClientServer#init()}
     * </pre>
     */
    RpcClient rpcClient;

    /**
     * 业务框架
     * <pre>
     *     {@link BoltClientServer#init()}
     * </pre>
     */
    BarSkeleton barSkeleton;

    /** 消息发送超时时间 */
    int timeoutMillis = 1000;

    /** 广播 */
    final Broadcast broadcast = new Broadcast(this);

    public Object invokeSync(final Object request, final int timeoutMillis) throws RemotingException, InterruptedException {
        return rpcClient.invokeSync(connection, request, timeoutMillis);
    }

    public Object invokeSync(final Object request) throws RemotingException, InterruptedException {
        return invokeSync(request, timeoutMillis);
    }

    public void oneway(final Object request) throws RemotingException {
        this.rpcClient.oneway(connection, request);
    }

    void invokeWithCallback(Object request) throws RemotingException {
        this.rpcClient.invokeWithCallback(connection, request, null, timeoutMillis);
    }

    @Override
    public ResponseMessage invokeModuleMessage(RequestMessage requestMessage) {

        InnerModuleMessage moduleMessage = new InnerModuleMessage();
        moduleMessage.setRequestMessage(requestMessage);

        ResponseMessage o = null;

        try {
            o = (ResponseMessage) this.invokeSync(moduleMessage);
        } catch (RemotingException | InterruptedException e) {
            e.printStackTrace();
        }

        return o;
    }

    @Override
    public void sendResponse(Object request) {
        try {
            rpcClient.oneway(connection, request);
        } catch (RemotingException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void broadcast(FlowContext flowContext, Collection<Long> userIdList) {
        this.broadcast.broadcast(flowContext, userIdList);
    }
}
