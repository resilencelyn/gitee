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
package com.iohao.little.game.net.external.bootstrap;

import com.alipay.remoting.rpc.RpcClient;
import com.iohao.little.game.net.client.BoltClientServer;
import com.iohao.little.game.net.client.BoltClientServerSetting;
import lombok.experimental.UtilityClass;

/**
 * @author 洛朱
 * @date 2022-01-11
 */
@UtilityClass
public class ExternalServerKit {
    /** 网关 rpcClient */
    public RpcClient rpcClient;
    /** 网关 BoltClientServer */
    public BoltClientServer boltClientServer;
    /** 网关 BoltClientServerSetting */
    public BoltClientServerSetting boltClientServerSetting;

    public void setBoltClientServer(BoltClientServer boltClientServer) {
        ExternalServerKit.rpcClient = boltClientServer.getRpcClient();
        ExternalServerKit.boltClientServer = boltClientServer;
        ExternalServerKit.boltClientServerSetting = boltClientServer.getBoltClientServerSetting();
    }

    public String address() {
        return boltClientServerSetting.getAddress();
    }
}
