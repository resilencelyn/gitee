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
package com.iohao.little.game.net.server.core;

import com.alipay.remoting.config.Configs;
import com.iohao.little.game.net.common.BoltServer;
import com.iohao.little.game.net.server.GateKit;

/**
 * 服务端启动配置
 * <pre>
 *     一般指网关服
 * </pre>
 *
 * @author 洛朱
 * @date 2021-12-20
 */
public interface ServerStartup {

    /**
     * 创建 BoltServer
     *
     * @return BoltServer
     */
    BoltServer createBoltServer();

    /**
     * 添加连接处理器
     * <pre>
     *     see:
     *     {@link com.alipay.remoting.ConnectionEventType#CLOSE}
     *     {@link com.alipay.remoting.ConnectionEventType#CONNECT}
     * </pre>
     *
     * @param boltServer boltServer
     */
    default void connectionEventProcessor(BoltServer boltServer) {
        CommonServerStartup.connectionEventProcessor(boltServer);
    }

    /**
     * 注册用户处理器
     *
     * @param boltServer boltServer
     */
    default void registerUserProcessor(BoltServer boltServer) {
        CommonServerStartup.registerUserProcessor(boltServer);
    }

    /**
     * 服务器启动前的钩子方法
     *
     * @param boltServer boltServer
     */
    default void startBefore(BoltServer boltServer) {

    }

    /**
     * 启动
     * <pre>
     *     模板方法模式
     * </pre>
     */
    default void startup() {
        // 开启 bolt 重连
        System.setProperty(Configs.CONN_MONITOR_SWITCH, "true");
        System.setProperty(Configs.CONN_RECONNECT_SWITCH, "true");

        // 创建 BoltServer
        BoltServer boltServer = createBoltServer();
        GateKit.setBoltServer(boltServer);

        // 添加连接处理器
        connectionEventProcessor(boltServer);

        // 注册用户处理器
        registerUserProcessor(boltServer);

        // 服务器启动前的钩子方法
        startBefore(boltServer);

        // 启动服务端
        boltServer.startup();
    }
}
