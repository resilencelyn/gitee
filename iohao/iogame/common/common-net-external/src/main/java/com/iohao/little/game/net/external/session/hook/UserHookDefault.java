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
package com.iohao.little.game.net.external.session.hook;

import com.iohao.little.game.net.external.session.UserSession;
import com.iohao.little.game.net.external.session.UserSessions;
import lombok.extern.slf4j.Slf4j;

/**
 * UserHookDefault
 *
 * @author 洛朱
 * @date 2022-03-14
 */
@Slf4j
public class UserHookDefault implements UserHook {

    @Override
    public void into(UserSession userSession) {
        long userId = userSession.getUserId();
        log.info("玩家上线 userId: {} -- {}", userId, userSession.getUserChannelId());
        log.info("当前在线玩家数量： {}", UserSessions.me().countOnline());
    }

    @Override
    public void quit(UserSession userSession) {
        long userId = userSession.getUserId();
        log.info("玩家退出 userId: {} -- {}", userId, userSession.getUserChannelId());
        log.info("当前在线玩家数量： {}", UserSessions.me().countOnline());
    }
}
