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
package com.iohao.game.collect.tank.send;

import com.iohao.game.collect.common.send.AbstractFlowContextSend;
import com.iohao.game.collect.proto.common.UserInfo;
import com.iohao.game.collect.proto.tank.TankBullet;
import com.iohao.game.collect.proto.tank.TankLocation;
import com.iohao.game.collect.tank.TankCmd;
import com.iohao.little.game.action.skeleton.annotation.DocActionSend;
import com.iohao.little.game.action.skeleton.annotation.DocActionSends;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import lombok.extern.slf4j.Slf4j;

/**
 * 坦克相关推送
 *
 * @author 洛朱
 * @date 2022-01-31
 */
@Slf4j
@DocActionSends({
        @DocActionSend(cmd = TankCmd.cmd, subCmd = TankCmd.shooting, dataClass = TankBullet.class),
        @DocActionSend(cmd = TankCmd.cmd, subCmd = TankCmd.tankMove, dataClass = TankLocation.class),
        @DocActionSend(cmd = TankCmd.cmd, subCmd = 11, dataClass = UserInfo.class),
})
public class TankSend extends AbstractFlowContextSend {

    public TankSend(FlowContext flowContext) {
        super(flowContext);
    }

    @Override
    public void send() {
        this.execute();
    }
}
