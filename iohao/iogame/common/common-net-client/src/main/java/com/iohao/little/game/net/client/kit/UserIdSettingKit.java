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
package com.iohao.little.game.net.client.kit;

import com.alipay.remoting.exception.RemotingException;
import com.iohao.little.game.action.skeleton.core.ServerContext;
import com.iohao.little.game.action.skeleton.core.flow.attr.FlowAttr;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.net.client.common.BoltClientProxy;
import com.iohao.little.game.net.message.common.SettingUserIdMessage;
import com.iohao.little.game.net.message.common.SettingUserIdMessageResponse;
import lombok.experimental.UtilityClass;
import lombok.extern.slf4j.Slf4j;

import java.util.Objects;

/**
 * 变更用户 id
 * <pre>
 *     用户连接登录编写 文档
 *     https://www.yuque.com/iohao/game/tywkqv
 * </pre>
 *
 * @author 洛朱
 * @date 2022-01-19
 */
@Slf4j
@UtilityClass
public class UserIdSettingKit {
    /**
     * 设置用户的 userId
     * <pre>
     *     玩家真正的登录，只有设置了用户的 id， 才算是验证通过
     * </pre>
     *
     * @param flowContext 业务框架 flow上下文
     * @param userId      一般从数据库中获取
     * @return true 变更成功
     */
    public boolean settingUserId(FlowContext flowContext, long userId) {
        // 这个 userId 一般是首次建立连接时，系统随机分配的临时 id
        String userChannelId = flowContext.getRequest().getUserChannelId();

        SettingUserIdMessage userIdMessage = new SettingUserIdMessage()
                .setUserId(userId)
                .setUserChannelId(userChannelId);

        log.debug("1 逻辑服 {}", userIdMessage);

        try {
            ServerContext serverContext = flowContext.option(FlowAttr.serverContext);
            BoltClientProxy boltClientProxy = (BoltClientProxy) serverContext;
            // 请求网关 （实际上是请求对外服，让对外服保存 userId）
            SettingUserIdMessageResponse settingUserIdMessageResponse = (SettingUserIdMessageResponse) boltClientProxy
                    .invokeSync(userIdMessage);

            log.debug("5 逻辑服 {}", settingUserIdMessageResponse);

            if (Objects.isNull(settingUserIdMessageResponse) || !settingUserIdMessageResponse.isSuccess()) {
                return false;
            }

        } catch (RemotingException | InterruptedException e) {
            log.error(e.getMessage(), e);
        }

        ResponseMessage response = flowContext.getResponse();
        response.setUserId(userId);

        return true;
    }
}
