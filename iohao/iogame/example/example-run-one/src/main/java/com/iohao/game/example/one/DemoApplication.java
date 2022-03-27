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
package com.iohao.game.example.one;

import com.iohao.game.example.one.server.DemoLogicServer;
import com.iohao.little.game.net.external.config.ExternalOtherConfig;
import com.iohao.little.game.net.external.simple.SimpleHelper;

import java.util.List;

/**
 * 启动类
 * <pre>
 *     启动 对外服、网关服、逻辑服; 并生成游戏业务文档
 * </pre>
 * @author 洛朱
 * @date 2022-02-24
 */
public class DemoApplication {
    public static void main(String[] args) {
        // 注意，这个是临时测试用的，设置为 false 表示不用登录就可以访问逻辑服的方法
        ExternalOtherConfig.verifyIdentity = false;

        // 游戏对外服端口
        int port = 10100;

        // 游戏网关端口
        int gatewayPort = 10200;

        // 逻辑服
        var demoLogicServer = new DemoLogicServer(gatewayPort);

        // 启动 对外服、网关服、逻辑服; 并生成游戏业务文档
        SimpleHelper.run(port, gatewayPort, List.of(demoLogicServer));

        /*
         * 该示例文档地址
         * https://www.yuque.com/iohao/game/zm6qg2
         */
    }
}
