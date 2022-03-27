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
package com.iohao.game.collect.common;

/**
 * @author 洛朱
 * @date 2022-01-12
 */
public interface GameConfig {
    /** 对外服务器 port */
    int externalPort = 10088;
    /** 对外服务器 ip */
    String externalIp = "127.0.0.1";
    /** http 升级 websocket 协议地址 */
    String websocketPath = "/websocket";

    /** 网关 port */
    int gatePort = 10086;
    /** 网关 ip */
    String gateIp = "127.0.0.1";



}
