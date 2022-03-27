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
package com.iohao.little.game.net.external.bootstrap;

import cn.hutool.core.util.IdUtil;
import com.alipay.remoting.rpc.RpcCommandType;
import com.iohao.little.game.action.skeleton.protocol.RequestMessage;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessageCmdCode;
import com.iohao.little.game.net.external.session.UserSession;
import com.iohao.little.game.net.external.session.UserSessions;
import io.netty.channel.Channel;
import lombok.experimental.UtilityClass;

/**
 * @author 洛朱
 * @date 2022-01-18
 */
@UtilityClass
public class ExternalKit {
    public RequestMessage convertRequestMessage(ExternalMessage message) {
        RequestMessage requestMessage = new RequestMessage();
        requestMessage.setCmdMerge(message.getCmdMerge());
        requestMessage.setRpcCommandType(RpcCommandType.REQUEST_ONEWAY);
        requestMessage.setDataContent(message.getDataContent());

        return requestMessage;
    }

    public ExternalMessage convertExternalMessage(ResponseMessage responseMessage) {

        ExternalMessage externalMessage = new ExternalMessage();
        externalMessage.setCmdCode(ExternalMessageCmdCode.biz);
        externalMessage.setCmdMerge(responseMessage.getCmdMerge());
        externalMessage.setResponseStatus((short) responseMessage.getResponseStatus());
        externalMessage.setData(responseMessage.getDataContent());

        return externalMessage;
    }

    public void writeAndFlush(long userId, ExternalMessage message) {

        UserSession userSession = UserSessions.me().getUserSession(userId);

        Channel channel = userSession.getChannel();

        channel.writeAndFlush(message);
    }

    public long newId() {
        return IdUtil.getSnowflake().nextId();
    }

}
