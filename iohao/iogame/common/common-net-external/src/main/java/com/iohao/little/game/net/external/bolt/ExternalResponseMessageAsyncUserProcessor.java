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
import com.iohao.little.game.action.skeleton.core.exception.ActionErrorEnum;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.net.external.bootstrap.ExternalKit;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import com.iohao.little.game.net.external.config.ExternalOtherConfig;
import com.iohao.little.game.net.external.session.UserChannelId;
import com.iohao.little.game.net.external.session.UserSession;
import com.iohao.little.game.net.external.session.UserSessions;
import io.netty.channel.Channel;
import lombok.extern.slf4j.Slf4j;

import java.util.Objects;


/**
 * 接收来自网关的响应
 * <pre>
 *     把响应 write 到客户端
 * </pre>
 *
 * @author 洛朱
 * @date 2022-01-18
 */
@Slf4j
public class ExternalResponseMessageAsyncUserProcessor extends AsyncUserProcessor<ResponseMessage> {
    @Override
    public void handleRequest(BizContext bizCtx, AsyncContext asyncCtx, ResponseMessage responseMessage) {
        if (log.isDebugEnabled()) {
            log.debug("接收来自网关的响应 {}", responseMessage);
        }

        ExternalMessage message = ExternalKit.convertExternalMessage(responseMessage);

        UserSession userSession = null;

        try {
            // true 表示请求业务方法需要先登录
            if (ExternalOtherConfig.verifyIdentity) {
                long userId = responseMessage.getUserId();
                userSession = UserSessions.me().getUserSession(userId);
            } else {
                String channelId = responseMessage.getUserChannelId();
                userSession = UserSessions.me().getUserSession(new UserChannelId(channelId));
            }
        } catch (RuntimeException e) {
            e.printStackTrace();
            message.setResponseStatus(ActionErrorEnum.verifyIdentity.getCode());
            message.setValidMsg(e.getMessage());
        }

        if (Objects.isNull(userSession)) {
            return;
        }

        // 响应结果给用户
        Channel channel = userSession.getChannel();
        channel.writeAndFlush(message);
    }

    /**
     * 指定感兴趣的请求数据类型，该 UserProcessor 只对感兴趣的请求类型的数据进行处理；
     * 假设 除了需要处理 MyRequest 类型的数据，还要处理 java.lang.String 类型，有两种方式：
     * 1、再提供一个 UserProcessor 实现类，其 interest() 返回 java.lang.String.class.getName()
     * 2、使用 MultiInterestUserProcessor 实现类，可以为一个 UserProcessor 指定 List<String> multiInterest()
     */
    @Override
    public String interest() {
        return ResponseMessage.class.getName();
    }
}
