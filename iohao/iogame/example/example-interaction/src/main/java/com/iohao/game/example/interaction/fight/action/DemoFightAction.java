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

import com.iohao.game.example.interaction.msg.DemoFightMsg;
import com.iohao.game.example.interaction.msg.DemoWeatherMsg;
import com.iohao.game.example.interaction.weather.action.DemoCmdForWeather;
import com.iohao.little.game.action.skeleton.annotation.ActionController;
import com.iohao.little.game.action.skeleton.annotation.ActionMethod;
import com.iohao.little.game.action.skeleton.core.CmdInfo;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;

/**
 * 战斗 action
 *
 * @author 洛朱
 * @date 2022-03-24
 */
@ActionController(DemoCmdForFight.cmd)
public class DemoFightAction {

    /**
     * 战斗
     *
     * @param flowContext flowContext
     * @return current demoFightReq
     */
    @ActionMethod(DemoCmdForFight.fight)
    public DemoFightMsg fight(FlowContext flowContext) {
        // 路由：这个路由是将要访问逻辑服的路由（表示你将要去的地方）
        CmdInfo todayWeatherCmd = flowContext.getCmdInfo(DemoCmdForWeather.cmd, DemoCmdForWeather.todayWeather);
        // 根据路由信息来请求其他子服务器（其他逻辑服）的数据
        DemoWeatherMsg demoWeatherMsg = flowContext.invokeModuleMessageData(todayWeatherCmd, DemoWeatherMsg.class);

        DemoFightMsg demoFightMsg = new DemoFightMsg();
        // 把天气预报逻辑服的数据 增强给自己
        demoFightMsg.attack = demoWeatherMsg.attack;

        // 加一点点描述，以 30 为描述的临界点
        int criticalPoint = 30;
        if (demoFightMsg.attack > criticalPoint) {
            demoFightMsg.description = "英雄攻击得到了 极大的增强";
        } else {
            demoFightMsg.description = "英雄攻击得到了 小部分加成";
        }

        return demoFightMsg;
    }
}
