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

import com.iohao.little.game.action.skeleton.core.data.TestDataKit;
import com.iohao.little.game.action.skeleton.core.flow.ActionMethodInOut;
import com.iohao.little.game.action.skeleton.core.flow.interal.DebugInOut;
import lombok.extern.slf4j.Slf4j;

import java.util.ArrayList;
import java.util.List;

@Slf4j
public class PrintActionKitTest {

    @org.junit.Test
    public void printInout() {
        log.info("hello");
        List<ActionMethodInOut> inOutList = new ArrayList<>();
        inOutList.add(new DebugInOut());
        PrintActionKit.printInout(inOutList);
    }

    @org.junit.Test
    public void printHandler() {
        List<Handler> handlerList = new ArrayList<>();
        handlerList.add(new ActionCommandHandler());
        PrintActionKit.printHandler(handlerList);
    }

    @org.junit.Test
    public void log() {
        BarSkeleton barSkeleton = TestDataKit.newBarSkeleton();

        log.info("BarSkeleton: {}", barSkeleton);
    }
}