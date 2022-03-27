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

import com.iohao.little.game.action.skeleton.core.action.ExampleActionCont;
import com.iohao.little.game.action.skeleton.core.action.pojo.BeeApple;
import com.iohao.little.game.action.skeleton.core.action.pojo.Bird;
import com.iohao.little.game.action.skeleton.core.data.TestDataKit;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import com.iohao.little.game.action.skeleton.protocol.RequestMessage;
import lombok.extern.slf4j.Slf4j;
import org.junit.Test;

@Slf4j
public class BarSkeletonTest {

    @Test
    public void newBuilder() {

        // 模拟路由信息
        CmdInfo cmdInfo = CmdInfoFlyweightFactory.me().getCmdInfo(ExampleActionCont.BeeActionCont.cmd, ExampleActionCont.BeeActionCont.hello);

        // 模拟请求
        RequestMessage requestMessage = new RequestMessage();
        requestMessage.setCmdInfo(cmdInfo);

        // 模拟请求数据 （一般由前端传入）
        BeeApple beeApple = new BeeApple();
        beeApple.setContent("hello 塔姆!");
        beeApple.setId(101);
        // 把模拟请求的数据,放入请求对象中
        requestMessage.setData(beeApple);

        log.info("BarSkeleton");

        // 构建业务框架
        BarSkeleton barSkeleton = TestDataKit.newBarSkeleton();

        var flowContext = new FlowContext()
                .setRequest(requestMessage);

        // 业务框架处理用户请求
        barSkeleton.handle(flowContext);
        System.out.println();

        // 业务框架处理用户请求
        barSkeleton.handle(flowContext);
        System.out.println();
    }

    @Test
    public void testVoid() {
        // 模拟路由信息
        CmdInfo cmdInfo = CmdInfoFlyweightFactory.me().getCmdInfo(ExampleActionCont.BirdActionCont.cmd, ExampleActionCont.BirdActionCont.test_void);


        // 模拟请求
        RequestMessage requestMessage = new RequestMessage();
        requestMessage.setCmdInfo(cmdInfo);

        // 模拟请求数据 （一般由前端传入）
        Bird bird = new Bird();
        bird.setName("hello 塔姆!");
        bird.setId(1010);
        // 把模拟请求的数据,放入请求对象中
        requestMessage.setData(bird);

        // 构建业务框架
        BarSkeleton barSkeleton = TestDataKit.newBarSkeleton();
        var flowContext = new FlowContext()
                .setRequest(requestMessage);

        // 业务框架处理用户请求
        barSkeleton.handle(flowContext);
        System.out.println();

    }
}