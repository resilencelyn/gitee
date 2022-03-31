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
package com.iohao.little.game.net.client.core;

import com.alipay.remoting.rpc.RpcClient;
import com.iohao.little.game.action.skeleton.core.ActionCommandManager;
import com.iohao.little.game.action.skeleton.core.BarSkeleton;
import com.iohao.little.game.net.client.BoltClientServer;
import com.iohao.little.game.net.client.BoltClientServerSetting;
import com.iohao.little.game.net.client.common.BoltClientProxy;
import com.iohao.little.game.net.client.kit.BoltClientProxyKit;
import com.iohao.little.game.net.message.common.ModuleMessage;

import java.util.Objects;

/**
 * 逻辑服 启动流程
 *
 * @author 洛朱
 * @date 2021-12-17
 */
public interface ClientStartup {

    /**
     * 初始化 业务框架
     *
     * @return 业务框架
     */
    BarSkeleton createBarSkeleton();

    /**
     * 初始化 模块信息
     *
     * @return 模块信息
     */
    ModuleMessage createModuleMessage();

    /**
     * 初始化 远程连接地址
     *
     * @return 远程连接地址
     */
    RemoteAddress createRemoteAddress();

    /**
     * 构建配置项
     *
     * @param setting 客户端服务器构建配置项
     */
    default void serverSetting(BoltClientServerSetting setting) {

    }

    /**
     * 添加连接处理器
     * <pre>
     *     see:
     *     {@link com.alipay.remoting.ConnectionEventType#CLOSE}
     *     {@link com.alipay.remoting.ConnectionEventType#CONNECT}
     * </pre>
     *
     * @param boltClientServer boltClientServer
     */
    default void connectionEventProcessor(BoltClientServer boltClientServer) {
        CommonClientStartup.connectionEventProcessor(boltClientServer);
    }

    /**
     * 注册用户处理器
     *
     * @param boltClientServer boltClientServer
     */
    default void registerUserProcessor(BoltClientServer boltClientServer) {
        CommonClientStartup.registerUserProcessor(boltClientServer);
    }

    /**
     * 服务器启动后的钩子方法
     *
     * @param boltClientServer BoltClientServer
     */
    default void startupSuccess(BoltClientServer boltClientServer) {

    }

    /**
     * 启动
     * <pre>
     *     模板方法模式
     * </pre>
     */
    default void startup() {
        int[] cmdMergeArray;

        // 业务框架
        BarSkeleton barSkeleton = this.createBarSkeleton();
        if (Objects.nonNull(barSkeleton)) {
            // 设置模块包含的 cmd 列表
            ActionCommandManager actionCommandManager = barSkeleton.getActionCommandManager();
            cmdMergeArray = actionCommandManager.arrayCmdMerge();
        } else {
            cmdMergeArray = new int[0];
        }

        // 创建模块信息
        ModuleMessage moduleMessage = this.createModuleMessage();
        moduleMessage.setCmdMergeArray(cmdMergeArray);

        // 远程连接地址
        RemoteAddress remoteAddress = this.createRemoteAddress();

        // 服务器设置
        BoltClientServerSetting setting = new BoltClientServerSetting(barSkeleton, moduleMessage, remoteAddress);
        // 构建配置项
        this.serverSetting(setting);

        // 构建客户端服务器
        BoltClientServer boltClientServer = new BoltClientServer(setting);

        // 添加连接处理器
        this.connectionEventProcessor(boltClientServer);
        // 注册用户处理器
        this.registerUserProcessor(boltClientServer);

        // 初始化 客户端服务器
        boltClientServer.init();

        // 客户端服务器代理，
        BoltClientProxy boltClientProxy = setting.getBoltClientProxy();

        // 启动 rpc client
        RpcClient rpcClient = boltClientServer.getRpcClient();
        rpcClient.startup();

        // 客户端服务器注册到网关服
        boltClientServer.registerModuleToGate();

        // save BoltClientServer
        BoltClientProxyKit.put(moduleMessage.getModuleKey(), boltClientProxy);

        // 服务器启动后的钩子方法
        this.startupSuccess(boltClientServer);

    }
}
