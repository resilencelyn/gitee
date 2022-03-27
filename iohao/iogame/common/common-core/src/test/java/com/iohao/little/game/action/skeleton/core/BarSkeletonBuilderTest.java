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

import com.iohao.little.game.action.skeleton.core.action.TempAction;
import com.iohao.little.game.action.skeleton.core.flow.interal.DebugInOut;
import org.junit.Test;

/**
 * @author 洛朱
 * @date 2021-12-24
 */
public class BarSkeletonBuilderTest {

    @Test
    public void build() {
        // 业务框架构建器
        BarSkeletonBuilder builder = BarSkeleton.newBuilder();
        // 添加(请求响应)处理类. 用户可以定义自己的业务控制器
        builder
                .addActionController(TempAction.class)
                .addInOut(new DebugInOut())
        ;

        // 构建业务框架
        BarSkeleton barSkeleton = builder.build();
    }
}