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
package com.iohao.game.collect.external.tester.websocket;

import com.iohao.game.collect.common.GameConfig;
import com.iohao.little.game.net.external.ExternalServer;
import com.iohao.little.game.net.external.ExternalServerBuilder;
import com.iohao.little.game.net.external.bootstrap.ExternalJoinEnum;

/**
 * @author 洛朱
 * @date 2022-01-13
 */
public class TestExternalServerWebsocket {


    public static void main(String[] args) {
        new TestExternalServerWebsocket().init();
        System.out.println("对外服务器启动 ok!");
    }

    public void init() {

        // 端口
        int port = GameConfig.externalPort;
        // 对外服务器 - 构建器
        ExternalServerBuilder builder = ExternalServer.newBuilder(port)
                // websocket 方式连接
                .externalJoinEnum(ExternalJoinEnum.WEBSOCKET)
                // websocket 业务处理器
                .registerChannelHandler("TestExternalBizHandler", new TestExternalBizHandler());
//        builder.registerChannelHandler("TestExternalBizHandler", new ExternalTextHandler());

        // 构建对外服务器
        ExternalServer externalServer = builder.build();
        // 启动对外服务器
        externalServer.startup();
    }
}
