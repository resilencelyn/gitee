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
package com.iohao.little.game.net.external;

import com.iohao.little.game.common.kit.ExecutorKit;
import com.iohao.little.game.net.external.bolt.AbstractExternalClientStartup;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelFuture;
import lombok.AccessLevel;
import lombok.experimental.FieldDefaults;
import lombok.extern.slf4j.Slf4j;

import java.net.InetSocketAddress;
import java.util.concurrent.ScheduledExecutorService;

/**
 * 对外的服务器
 *
 * @author 洛朱
 * @date 2022-01-09
 */
@Slf4j
@FieldDefaults(level = AccessLevel.PRIVATE)
public class ExternalServer {
    final ScheduledExecutorService singleScheduled = ExecutorKit.newSingleScheduled("iohao.com:ExternalServer-Gateway");
    /** netty 服务器，与真实用户对接 */
    final ServerBootstrap bootstrap;
    /** 内部逻辑服 连接网关服务器，与网关通信 */
    final AbstractExternalClientStartup externalClientStartup;
    /** ip */
    final String ip;
    /** 对外服端口 */
    final int port;

    ExternalServer(ExternalServerBuilder builder) {
        this.port = builder.port;
        this.ip = builder.ip;
        this.bootstrap = builder.bootstrap;
        this.externalClientStartup = builder.externalClientStartup;
    }

    /**
     * 启动对外服
     *
     * @throws InterruptedException e
     */
    private void doStart() throws InterruptedException {
        // channelFuture
        ChannelFuture channelFuture = bootstrap.bind(new InetSocketAddress(ip, port)).sync();
        System.out.println("external 启动游戏对外服 ! port: " + port);
        channelFuture.channel().closeFuture().sync();
    }

    /**
     * 启动内部逻辑服 连接网关服务器，与网关通信
     */
    private void startupExternalClientStartup() {
        singleScheduled.execute(this.externalClientStartup::startup);
        System.out.println("external 启动内部逻辑服, 用于连接网关服务器");
    }

    /**
     * 启动对外服
     */
    public void startup() {
        // 启动内部逻辑服, 用于连接网关服务器
        this.startupExternalClientStartup();

        try {
            // 启动对外服
            this.doStart();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void shutdown() {
        singleScheduled.shutdown();

    }

    public static ExternalServerBuilder newBuilder(int port) {
        return new ExternalServerBuilder(port);
    }

    public static void main(String[] args) {
        int port = 22022;

        ExternalServerBuilder builder = ExternalServer.newBuilder(port);

        ExternalServer externalServer = builder.build();

        externalServer.startup();
        System.out.println("OK!");

    }
}
