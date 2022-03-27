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
package com.iohao.game.collect.external.tester.socket;

import com.iohao.game.collect.proto.common.LoginVerify;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import lombok.extern.slf4j.Slf4j;

import java.util.concurrent.atomic.LongAdder;

/**
 * @author 洛朱
 * @date 2022-01-13
 */
@Slf4j
@ChannelHandler.Sharable
public class TestExternalHandler extends SimpleChannelInboundHandler<ExternalMessage> {

    LongAdder id = new LongAdder();

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, ExternalMessage message) {
        log.info("接收用户的数据 {}", message);

        id.increment();

        if (id.longValue() % 5 == 0) {
            message.setResponseStatus(id.shortValue());
        }

        LoginVerify loginVerify = new LoginVerify();
        loginVerify.jwt = ("jwt:" + id.toString());

        message.setData(loginVerify);

        log.info("响应 用户的数据 {}", message);

        // 响应结果给用户
        ctx.writeAndFlush(message);
    }

}
