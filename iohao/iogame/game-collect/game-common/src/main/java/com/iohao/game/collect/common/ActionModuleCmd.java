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

import com.iohao.little.game.action.skeleton.core.CmdInfo;
import com.iohao.little.game.action.skeleton.core.CmdInfoFlyweightFactory;

/**
 * 逻辑服 cmd 模块
 *
 * @author 洛朱
 * @date 2022-01-11
 */
public interface ActionModuleCmd {
    interface Info {
        /**
         * 主路由 cmd
         *
         * @return cmd
         */
        int cmd();

        /**
         * cmdInfo
         *
         * @param subCmd subCmd
         * @return CmdInfo
         */
        default CmdInfo cmdInfo(int subCmd) {
            return CmdInfoFlyweightFactory.me().getCmdInfo(cmd(), subCmd);
        }
    }

    /** 用户模块 */
    int hallModuleCmd = 1;
    /** 游戏 坦克模块 */
    int tankModuleCmd = 2;

}
