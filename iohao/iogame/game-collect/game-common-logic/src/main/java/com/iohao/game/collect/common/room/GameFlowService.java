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
package com.iohao.game.collect.common.room;

import com.iohao.game.collect.common.room.flow.*;
import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;
import lombok.experimental.FieldDefaults;

/**
 * 游戏流程
 * <pre>
 *     游戏规则
 *     创建玩家
 *
 *     房间创建
 *
 *     进入房间
 *     游戏开始
 *
 * </pre>
 *
 * @author 洛朱
 * @date 2022-01-14
 */
@Setter
@Getter
@FieldDefaults(level = AccessLevel.PRIVATE)
public class GameFlowService {
    /** 游戏规则 */
    RoomRuleInfoCustom roomRuleInfoCustom;
    /** 游戏开始 */
    RoomGameStartCustom roomGameStartCustom;
    /** 创建玩家 */
    RoomPlayerCreateCustom roomPlayerCreateCustom;
    /** 房间创建 */
    RoomCreateCustom roomCreateCustom;
    /** 进入房间 */
    RoomEnterCustom roomEnterCustom;

    public static GameFlowService me() {
    	return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final GameFlowService ME = new GameFlowService();
     }

}
