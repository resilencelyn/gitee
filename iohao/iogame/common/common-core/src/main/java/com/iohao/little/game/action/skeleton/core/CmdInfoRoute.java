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
package com.iohao.little.game.action.skeleton.core;

/**
 * cmd 路由
 * <pre>
 *     一般配合枚举使用，用来描述路由
 *     这里先做一个预留
 * </pre>
 *
 * @author 洛朱
 * @date 2022-03-24
 */
public interface CmdInfoRoute {
    /**
     * 主路由
     *
     * @return 主路由
     */
    int getCmd();

    /**
     * 子路由
     *
     * @return 子路由
     */
    int getSubCmd();

    /**
     * 命令路由信息
     *
     * @return CmdInfo
     */
    default CmdInfo getCmdInfo() {
        return CmdInfoFlyweightFactory.me().getCmdInfo(getCmd(), getSubCmd());
    }
}
