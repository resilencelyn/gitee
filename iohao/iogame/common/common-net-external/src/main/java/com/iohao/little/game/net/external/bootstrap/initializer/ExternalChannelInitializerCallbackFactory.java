/*
 * # iohao.com . 渔民小镇
 * Copyright (C) 2021 - 2022 double joker （262610965@qq.com） . All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.iohao.little.game.net.external.bootstrap.initializer;

import com.iohao.little.game.net.external.bootstrap.ExternalChannelInitializerCallback;
import com.iohao.little.game.net.external.bootstrap.ExternalJoinEnum;
import lombok.extern.slf4j.Slf4j;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Supplier;

/**
 * ExternalChannelInitializerCallback 工厂
 *
 * @author 洛朱
 * @date 2022-03-13
 */
@Slf4j
public class ExternalChannelInitializerCallbackFactory {

    final Map<ExternalJoinEnum, Supplier<ExternalChannelInitializerCallback>> map = new HashMap<>();

    /**
     * 创建 ExternalChannelInitializerCallback，Channel 初始化的业务编排 (自定义业务编排)
     *
     * @param externalJoinEnum externalJoinEnum
     * @return ExternalChannelInitializerCallback
     */
    public ExternalChannelInitializerCallback createExternalChannelInitializerCallback(ExternalJoinEnum externalJoinEnum) {

        if (!this.map.containsKey(externalJoinEnum)) {
            throw new RuntimeException("externalJoinEnum expected: " + Arrays.toString(ExternalJoinEnum.values()));
        }

        Supplier<ExternalChannelInitializerCallback> supplier = this.map.get(externalJoinEnum);

        // 连接方式
        ExternalChannelInitializerCallback channelInitializerCallback = supplier.get();

        log.info("启动方式========================{}========================", externalJoinEnum.getName());

        return channelInitializerCallback;
    }

    public void putExternalChannelInitializerCallback(ExternalJoinEnum externalJoinEnum, Supplier<ExternalChannelInitializerCallback> supplier) {
        this.map.put(externalJoinEnum, supplier);
    }

    private ExternalChannelInitializerCallbackFactory() {
        // put tcp socket
        this.putExternalChannelInitializerCallback(ExternalJoinEnum.SOCKET, ExternalChannelInitializerCallbackSocket::new);
        // put websocket
        this.putExternalChannelInitializerCallback(ExternalJoinEnum.WEBSOCKET, ExternalChannelInitializerCallbackWebsocket::new);
    }

    public static ExternalChannelInitializerCallbackFactory me() {
        return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final ExternalChannelInitializerCallbackFactory ME = new ExternalChannelInitializerCallbackFactory();
    }
}
