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
package com.iohao.game.collect.external.tester.module;

import cn.hutool.core.util.RandomUtil;
import com.iohao.game.collect.hall.HallCmd;
import com.iohao.game.collect.proto.common.LoginVerify;
import com.iohao.game.collect.proto.common.UserInfo;
import com.iohao.little.game.action.skeleton.core.CmdKit;
import com.iohao.little.game.common.kit.ProtoKit;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import lombok.extern.slf4j.Slf4j;

/**
 * @author 洛朱
 * @date 2022-01-29
 */
@Slf4j
public class LoginVerifyOnMessage implements OnMessage {

    @Override
    public Object process(ExternalMessage message, byte[] data) {
        UserInfo userInfo = ProtoKit.parseProtoByte(data, UserInfo.class);

        return userInfo;
    }

    @Override
    public int getCmdMerge() {
        return CmdKit.merge(HallCmd.cmd, HallCmd.loginVerify);
    }

    @Override
    public Object requestData() {
        LoginVerify loginVerify = new LoginVerify();
        loginVerify.jwt = "test";
        loginVerify.jwt = RandomUtil.randomInt(10000) + "";

        log.info("建立连接后 执行的方法 {}", loginVerify);
        return loginVerify;
    }

    public static LoginVerifyOnMessage me() {
        return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final LoginVerifyOnMessage ME = new LoginVerifyOnMessage();
    }

}
