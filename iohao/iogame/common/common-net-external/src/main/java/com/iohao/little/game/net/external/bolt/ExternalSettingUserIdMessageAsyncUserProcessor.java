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
import com.iohao.little.game.net.external.session.UserChannelId;
import com.iohao.little.game.net.external.session.UserSessions;
import com.iohao.little.game.net.message.common.SettingUserIdMessage;
import com.iohao.little.game.net.message.common.SettingUserIdMessageResponse;
import lombok.extern.slf4j.Slf4j;

/**
 * 设置 userId 业务
 *
 * @author 洛朱
 * @date 2022-01-18
 */
@Slf4j
public class ExternalSettingUserIdMessageAsyncUserProcessor extends AsyncUserProcessor<SettingUserIdMessage> {
    @Override
    public void handleRequest(BizContext bizCtx, AsyncContext asyncCtx, SettingUserIdMessage request) {

        long userId = request.getUserId();
        String channelId = request.getUserChannelId();

        UserChannelId userChannelId = new UserChannelId(channelId);

        boolean result = UserSessions.me().settingUserId(userChannelId, userId);

        SettingUserIdMessageResponse response = new SettingUserIdMessageResponse();
        response.setSuccess(result);
        response.setUserId(userId);

        asyncCtx.sendResponse(response);

        log.debug("3 对外服设置用户id, userChannelId:{}, 真实userId:{}", userChannelId, userId);
    }

    @Override
    public String interest() {
        return SettingUserIdMessage.class.getName();
    }
}
