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

import com.alipay.remoting.AsyncContext;
import com.alipay.remoting.BizContext;
import com.alipay.remoting.rpc.protocol.AsyncUserProcessor;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.net.external.bootstrap.ExternalKit;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import com.iohao.little.game.net.external.session.UserSessions;
import com.iohao.little.game.net.message.common.BroadcastMessage;
import lombok.extern.slf4j.Slf4j;

import java.util.Objects;

/**
 * 接收并处理 来自网关的广播消息
 *
 * @author 洛朱
 * @date 2022-01-16
 */
@Slf4j
public class ExternalBroadcastMessageAsyncUserProcessor extends AsyncUserProcessor<BroadcastMessage> {
    @Override
    public void handleRequest(BizContext bizCtx, AsyncContext asyncCtx, BroadcastMessage message) {
//        log.info("对外服接收网关的数据：{}", message);
        ResponseMessage responseMessage = message.getResponseMessage();
        ExternalMessage externalMessage = ExternalKit.convertExternalMessage(responseMessage);

        // 推送消息到真实用户
        if (message.isBroadcastAll()) {
            // 给全体推送
            // TODO: 2022/1/18 这里可以使用领域事件
            UserSessions.me().broadcast(externalMessage);
            return;
        }

        // 给用户列表推送
        if (Objects.nonNull(message.getUserIdList())) {

            for (Long userId : message.getUserIdList()) {
                ExternalKit.writeAndFlush(userId, externalMessage);
            }

            return;
        }

        // 推送给单个用户
        long userId = responseMessage.getUserId();
        ExternalKit.writeAndFlush(userId, externalMessage);
    }

    /**
     * 指定感兴趣的请求数据类型，该 UserProcessor 只对感兴趣的请求类型的数据进行处理；
     * 假设 除了需要处理 MyRequest 类型的数据，还要处理 java.lang.String 类型，有两种方式：
     * 1、再提供一个 UserProcessor 实现类，其 interest() 返回 java.lang.String.class.getName()
     * 2、使用 MultiInterestUserProcessor 实现类，可以为一个 UserProcessor 指定 List<String> multiInterest()
     */
    @Override
    public String interest() {
        return BroadcastMessage.class.getName();
    }
}
