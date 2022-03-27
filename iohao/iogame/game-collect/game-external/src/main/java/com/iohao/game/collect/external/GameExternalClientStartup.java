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
package com.iohao.game.collect.external;

import com.iohao.game.collect.common.GameConfig;
import com.iohao.little.game.net.client.BoltClientServer;
import com.iohao.little.game.net.client.core.RemoteAddress;
import com.iohao.little.game.net.external.bolt.AbstractExternalClientStartup;
import com.iohao.little.game.net.external.session.UserSessions;
import com.iohao.little.game.net.external.session.hook.UserHookDefault;
import com.iohao.little.game.net.message.common.ModuleKeyKit;
import com.iohao.little.game.net.message.common.ModuleMessage;
import com.iohao.little.game.net.message.common.ModuleType;

/**
 * 游戏对外服的内部逻辑服
 * <pre>
 *     主要职责：转发用户请求到网关服，在由网关服转到具体的业务逻辑服
 * </pre>
 *
 * @author 洛朱
 * @date 2022-01-11
 */
public class GameExternalClientStartup extends AbstractExternalClientStartup {

    @Override
    public ModuleMessage createModuleMessage() {
        int moduleId = 0;
        var moduleKey = ModuleKeyKit.getModuleKey(moduleId);

        ModuleMessage moduleMessage = new ModuleMessage();
        moduleMessage.setModuleType(ModuleType.EXTERNAL);
        moduleMessage.setModuleKey(moduleKey);
        moduleMessage.setName("对外服务器(external)");
        moduleMessage.setDescription("对接真实的游戏用户");

        return moduleMessage;
    }

    @Override
    public RemoteAddress createRemoteAddress() {
        int port = GameConfig.gatePort;
        String ip = GameConfig.gateIp;
        return new RemoteAddress(ip, port);
    }

    @Override
    public void startupSuccess(BoltClientServer boltClientServer) {
        super.startupSuccess(boltClientServer);

        // 设置 用户钩子接口，用户上线时、下线时会触发
        UserSessions.me().setUserHook(new UserHookDefault());
    }
}
