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
package com.iohao.game.collect.common.room.flow;

import com.iohao.game.collect.common.room.RuleInfo;
import com.iohao.little.game.action.skeleton.core.exception.MsgException;

/**
 * 游戏规则信息解析
 * <pre>
 *     具体的玩法规则协议, 在 json 中完成
 * </pre>
 *
 * @author 洛朱
 * @date 2022-01-14
 */
public interface RoomRuleInfoCustom {
    /**
     * 获取子游戏规则信息
     *
     * @param ruleInfoJson 游戏规则 - json
     * @return 规则信息
     * @throws MsgException e
     */
    RuleInfo getRuleInfo(String ruleInfoJson) throws MsgException;
}
