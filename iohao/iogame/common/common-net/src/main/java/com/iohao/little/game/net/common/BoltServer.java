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
/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.iohao.little.game.net.common;

import com.alipay.remoting.ConnectionEventProcessor;
import com.alipay.remoting.ConnectionEventType;
import com.alipay.remoting.rpc.RpcServer;
import com.alipay.remoting.rpc.protocol.UserProcessor;

/**
 * bolt server
 *
 * @author 洛朱
 * @date 2021-12-12
 */
public class BoltServer {
    /** port */
    private final int port;

    /** rpc server */
    private final RpcServer server;

    public BoltServer(int port) {
        this(port, true);
    }

    public BoltServer(int port, boolean manageFeatureEnabled) {
        this.port = port;
        this.server = new RpcServer(this.port, manageFeatureEnabled);
    }

    public BoltServer(int port, boolean manageFeatureEnabled, boolean syncStop) {
        this.port = port;
        this.server = new RpcServer(this.port, manageFeatureEnabled, syncStop);
    }

    public boolean startup() {
        this.server.startup();
        return true;
    }

    public void shutdown() {
        this.server.shutdown();
    }

    public RpcServer getRpcServer() {
        return this.server;
    }

    public void registerUserProcessor(UserProcessor<?> processor) {
        this.server.registerUserProcessor(processor);
    }

    public void addConnectionEventProcessor(ConnectionEventType type,
                                            ConnectionEventProcessor processor) {
        this.server.addConnectionEventProcessor(type, processor);
    }
}
