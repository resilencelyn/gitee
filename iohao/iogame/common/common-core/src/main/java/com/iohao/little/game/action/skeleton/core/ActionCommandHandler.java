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
package com.iohao.little.game.action.skeleton.core;

import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import com.iohao.little.game.action.skeleton.protocol.RequestMessage;

/**
 * 该handler用于执行 {@link ActionCommand} 对象
 *
 * @author 洛朱
 * @date 2021-12-12
 */
public final class ActionCommandHandler implements Handler {

    @Override
    public boolean handler(FlowContext flowContext) {

        this.settingActionCommand(flowContext);

        // 命令流程执行器
        BarSkeleton barSkeleton = flowContext.getBarSkeleton();
        var actionCommandFlowExecute = barSkeleton.getActionCommandFlowExecute();

        actionCommandFlowExecute.execute(flowContext);

        return true;
    }

    private void settingActionCommand(FlowContext flowContext) {
        BarSkeleton barSkeleton = flowContext.getBarSkeleton();
        /*
        这里不做任何null判断了. 使用者们自行注意
        根据客户端的请求信息,获取对应的命令对象来处理这个请求
         */
        var actionCommandManager = barSkeleton.actionCommandManager;

        RequestMessage request = flowContext.getRequest();
        // 通过路由获取处理请求的 action
        var cmd = request.getCmd();
        var subCmd = request.getSubCmd();
        var actionCommand = actionCommandManager.getActionCommand(cmd, subCmd);
        flowContext.setActionCommand(actionCommand);
    }
}
