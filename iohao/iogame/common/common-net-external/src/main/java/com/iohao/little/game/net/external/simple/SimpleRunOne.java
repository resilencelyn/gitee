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
package com.iohao.little.game.net.external.simple;

import com.iohao.little.game.action.skeleton.core.doc.BarSkeletonDoc;
import com.iohao.little.game.common.kit.ExecutorKit;
import com.iohao.little.game.net.client.core.ClientStartup;
import com.iohao.little.game.net.external.ExternalServer;
import com.iohao.little.game.net.server.core.ServerStartup;
import lombok.AccessLevel;
import lombok.Setter;
import lombok.experimental.Accessors;
import lombok.experimental.FieldDefaults;

import java.util.List;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * 简单的启动器： 对外服、网关服、逻辑服
 * 谐音:拳皇97中的 round one ready go!
 * <pre>
 *     注意：
 *          这个工具只适合单机的开发或本地一体化的开发, 对于分步式不适合。
 * </pre>
 *
 * @author 洛朱
 * @date 2022-02-28
 */
@Setter
@Accessors(chain = true)
@FieldDefaults(level = AccessLevel.PRIVATE)
public class SimpleRunOne {
    final ScheduledExecutorService executorService = ExecutorKit.newScheduled(2, SimpleRunOne.class.getSimpleName());

    /** 网关 */
    ServerStartup gatewayServer;
    /** 对外服 */
    ExternalServer externalServer;
    /** 逻辑服 */
    List<ClientStartup> logicServerList;

    /**
     * 简单的快速启动
     * <pre>
     *     快速启动:
     *          对外服
     *          网关服
     *          逻辑服
     * </pre>
     */
    public void startup() {
        // 启动网关
        executorService.execute(() -> {
            gatewayServer.startup();
            System.out.println("网关启动 ok!");
        });

        // 启动逻辑服、对外服
        startupLogic();
    }

    private void startupLogic() {
        executorService.schedule(() -> {
            // 启动逻辑服
            logicServerList.forEach(ClientStartup::startup);

            // 启动游戏对外服
            externalServer.startup();

        }, 1, TimeUnit.SECONDS);

        try {
            TimeUnit.SECONDS.sleep(2);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        // 生成游戏文档
        BarSkeletonDoc.me().buildDoc();
    }

}
