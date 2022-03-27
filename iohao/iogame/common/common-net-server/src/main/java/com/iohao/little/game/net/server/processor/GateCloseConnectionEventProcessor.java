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
package com.iohao.little.game.net.server.processor;

import com.alipay.remoting.Connection;
import com.alipay.remoting.ConnectionEventProcessor;
import org.junit.Assert;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * @author 洛朱
 * @date 2022-01-12
 */
public class GateCloseConnectionEventProcessor implements ConnectionEventProcessor {

    private final AtomicBoolean dicConnected = new AtomicBoolean();
    private final AtomicInteger disConnectTimes = new AtomicInteger();

    @Override
    public void onEvent(String remoteAddr, Connection conn) {
        Assert.assertNotNull(conn);
        dicConnected.set(true);
        disConnectTimes.incrementAndGet();
    }

    public boolean isDisConnected() {
        return this.dicConnected.get();
    }

    public int getDisConnectTimes() {
        return this.disConnectTimes.get();
    }

    public void reset() {
        this.disConnectTimes.set(0);
        this.dicConnected.set(false);
    }
}
