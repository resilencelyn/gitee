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
package com.iohao.little.game.net.server.processor;

import com.alipay.remoting.AsyncContext;
import com.alipay.remoting.BizContext;
import com.alipay.remoting.exception.RemotingException;
import com.alipay.remoting.rpc.RpcServer;
import com.alipay.remoting.rpc.protocol.AsyncUserProcessor;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.net.common.BoltServer;
import com.iohao.little.game.net.server.module.ModuleInfoManager;
import com.iohao.little.game.net.server.module.ModuleInfoProxy;
import lombok.extern.slf4j.Slf4j;

/**
 * 把逻辑服的响应转发到对外服
 *
 * @author 洛朱
 * @date 2022-01-18
 */
@Slf4j
public class GateResponseMessageAsyncUserProcessor extends AsyncUserProcessor<ResponseMessage> {

    final BoltServer boltServer;

    public GateResponseMessageAsyncUserProcessor(BoltServer boltServer) {
        this.boltServer = boltServer;
    }

    @Override
    public void handleRequest(BizContext bizCtx, AsyncContext asyncCtx, ResponseMessage responseMessage) {
        log.debug("把逻辑服的响应转发到对外服 {}", responseMessage);

        // 转发 给 对外服务器
        ModuleInfoProxy externalModuleInfo = ModuleInfoManager.me().getExternalModuleInfo();
        String address = externalModuleInfo.getModuleMessage().getAddress();

        try {
            RpcServer rpcServer = boltServer.getRpcServer();
            rpcServer.oneway(address, responseMessage);
        } catch (RemotingException | InterruptedException e) {
            e.printStackTrace();
        }

    }

    @Override
    public String interest() {
        return ResponseMessage.class.getName();
    }
}
