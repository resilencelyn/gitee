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
package com.iohao.game.example.interaction.fight.action;

import com.iohao.game.example.common.DemoModuleCmd;

/**
 * 战斗服的 cmd
 *
 * @author 洛朱
 * @date 2022-03-24
 */
public interface DemoCmdForFight {
    /** 模块 - 主 cmd : 3 */
    int cmd = DemoModuleCmd.demoModule_3_fight_cmd;

    /** 示例 fight 方法 */
    int fight = 0;
}
