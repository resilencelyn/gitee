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

import lombok.Getter;

import java.util.*;

/**
 * 命令管理器
 *
 * @author 洛朱
 * @date 2021-12-20
 */
public final class ActionCommandManager {
    /**
     * action map
     * <pre>
     *     key 是类的 cmd
     *     value 是 map {
     *         key 是 方法的 subCmd
     *         value 是 actionCommand 信息
     *     }
     * </pre>
     */
    final Map<Integer, Map<Integer, ActionCommand>> actionCommandMap = new HashMap<>();

    /**
     * action 数组. 下标对应 cmd
     */
    @Getter
    ActionCommand[][] actionCommands;

    /**
     * 获取命令处理器
     *
     * @param cmd    路由
     * @param subCmd 子路由
     * @return 命令处理器
     */
    public ActionCommand getActionCommand(int cmd, int subCmd) {

        if (cmd >= actionCommands.length) {
            return null;
        }

        var subActionCommands = actionCommands[cmd];

        if (subCmd >= subActionCommands.length) {
            return null;
        }

        return actionCommands[cmd][subCmd];
    }

    /**
     * 命令列表
     *
     * @return list
     */
    public List<ActionCommand> listActionCommand() {
        List<ActionCommand> actionCommandList = new LinkedList<>();
        for (Map<Integer, ActionCommand> value : this.actionCommandMap.values()) {
            actionCommandList.addAll(value.values());
        }
        return actionCommandList;
    }

    /**
     * 命令列表
     *
     * @return array
     */
    public int[] arrayCmdMerge() {
        var actionCommandList = this.listActionCommand();
        var cmdMergeArray = new int[actionCommandList.size()];

        var i = 0;
        for (ActionCommand actionCommand : actionCommandList) {
            cmdMergeArray[i] = actionCommand.getCmdInfo().getCmdMerge();
            i++;
        }

        return cmdMergeArray;
    }

}
