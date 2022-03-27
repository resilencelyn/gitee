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
package com.iohao.little.game.net.external.bootstrap.initializer;

import com.iohao.little.game.net.external.bootstrap.heart.IdleProcessSetting;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelPipeline;
import lombok.Setter;
import lombok.experimental.Accessors;

import java.util.Map;
import java.util.Objects;

/**
 * @author 洛朱
 * @date 2022-01-13
 */
@Setter
@Accessors(chain = true)
public class ExternalChannelInitializerCallbackOption {

    /** user processors of rpc server */
    Map<String, ChannelHandler> channelHandlerProcessors;
    /** 默认数据包最大 1MB */
    int packageMaxSize = 1024 * 1024;
    /** http 升级 websocket 协议地址 */
    String websocketPath = "/websocket";
    /** 心跳相关的构建器 */
    IdleProcessSetting idleProcessSetting;

    /**
     * 添加其他 handler 到 pipeline 中
     *
     * @param pipeline pipeline
     */
    void channelHandler(ChannelPipeline pipeline) {
        // 心跳
        this.idleHandler(pipeline);

        // 业务 handler
        this.channelHandlerProcessors(pipeline);
    }

    private void idleHandler(ChannelPipeline pipeline) {
        // 心跳处理
        if (Objects.isNull(idleProcessSetting)) {
            return;
        }

        idleProcessSetting.idleHandler(pipeline);
    }

    private void channelHandlerProcessors(ChannelPipeline pipeline) {
        if (Objects.nonNull(this.channelHandlerProcessors)) {
            for (Map.Entry<String, ChannelHandler> entry : this.channelHandlerProcessors.entrySet()) {
                pipeline.addLast(entry.getKey(), entry.getValue());
            }
        }
    }
}
