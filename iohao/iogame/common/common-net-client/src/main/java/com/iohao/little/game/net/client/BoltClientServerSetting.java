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
package com.iohao.little.game.net.client;

import com.iohao.little.game.action.skeleton.core.BarSkeleton;
import com.iohao.little.game.net.client.common.BoltClientProxy;
import com.iohao.little.game.net.client.core.RemoteAddress;
import com.iohao.little.game.net.client.core.ServerSetting;
import com.iohao.little.game.net.message.common.ModuleKey;
import com.iohao.little.game.net.message.common.ModuleMessage;
import lombok.Getter;
import lombok.Setter;
import lombok.experimental.Accessors;

/**
 * 客户端服务器构建配置项
 *
 * @author 洛朱
 * @Date 2021-12-17
 */
@Getter
@Setter
@Accessors(chain = true)
public class BoltClientServerSetting implements ServerSetting {
    /** ip:port */
    final String address;
    /** 模块 key */
    final ModuleKey moduleKey;
    /** 当前客户端的业务框架 */
    final BarSkeleton barSkeleton;
    /** 子模块信息 （子游戏服的信息、逻辑服） */
    final ModuleMessage moduleMessage;
    /** 远程地址 */
    final RemoteAddress remoteAddress;
    /** 客户端服务器代理, 持有一些属性；负责与网关通信 */
    BoltClientProxy boltClientProxy;


    public BoltClientServerSetting(BarSkeleton barSkeleton, ModuleMessage moduleMessage, RemoteAddress remoteAddress) {
        this.remoteAddress = remoteAddress;
        this.address = remoteAddress.ip() + ":" + remoteAddress.port();

        this.barSkeleton = barSkeleton;
        this.moduleMessage = moduleMessage;
        this.moduleKey = moduleMessage.getModuleKey();
    }
}
