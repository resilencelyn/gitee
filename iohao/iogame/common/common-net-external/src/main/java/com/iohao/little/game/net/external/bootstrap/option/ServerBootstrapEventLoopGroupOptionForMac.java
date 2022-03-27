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
package com.iohao.little.game.net.external.bootstrap.option;

import com.iohao.little.game.net.external.bootstrap.ServerBootstrapEventLoopGroupOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.ServerChannel;
import io.netty.channel.kqueue.KQueueEventLoopGroup;
import io.netty.channel.kqueue.KQueueServerSocketChannel;

/**
 * 服务端 for Mac nio 处理类
 *
 * @author 洛朱
 * @date 2022-01-09
 */
public class ServerBootstrapEventLoopGroupOptionForMac implements ServerBootstrapEventLoopGroupOption {
    @Override
    public EventLoopGroup bossGroup() {
        return new KQueueEventLoopGroup(
                1,
                ServerBootstrapEventLoopGroupOption.bossThreadFactory()
        );
    }

    @Override
    public EventLoopGroup workerGroup() {
        int availableProcessors = Runtime.getRuntime().availableProcessors() << 1;

        return new KQueueEventLoopGroup(
                availableProcessors,
                ServerBootstrapEventLoopGroupOption.workerThreadFactory()
        );
    }

    @Override
    public Class<? extends ServerChannel> channelClass() {
        return KQueueServerSocketChannel.class;
    }
}
