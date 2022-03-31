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
package com.iohao.little.game.net.client.common;

import com.iohao.little.game.action.skeleton.core.CmdInfo;
import com.iohao.little.game.net.message.common.ModuleKey;
import com.iohao.little.game.net.message.common.ModuleKeyKit;
import org.jctools.maps.NonBlockingHashMap;

import java.util.Map;
import java.util.Objects;

/**
 * 客户端代理管理器
 *
 * @author 洛朱
 * @date 2021-12-20
 */
public class BoltClientProxyManager {
    private final Map<ModuleKey, BoltClientProxy> boltClientMap = new NonBlockingHashMap<>();
    private BoltClientProxy gateProxy;

    public BoltClientProxy getBoltClientProxy(ModuleKey moduleKey) {

        // TODO: 2022/1/16 有问题这里
        if (Objects.isNull(moduleKey)) {
            // 把数据传到网关, is null 通常是请求其它服务器的数据
            return gateProxy;
        }

        BoltClientProxy boltClientProxy = boltClientMap.get(moduleKey);
        // 无锁化
        if (Objects.isNull(boltClientProxy)) {
            boltClientProxy = new BoltClientProxy();
            boltClientProxy = boltClientMap.putIfAbsent(moduleKey, boltClientProxy);

            if (Objects.isNull(boltClientProxy)) {
                boltClientProxy = boltClientMap.get(moduleKey);
            }

            // TODO: 这种情况只在全部游戏服务器都启动在一个进程时才会用得上
            gateProxy = boltClientProxy;
        }

        return boltClientProxy;
    }

    public BoltClientProxy getBoltClientProxy(CmdInfo cmdInfo) {
        ModuleKey moduleKey = ModuleKeyKit.getModuleKeyByCmdMerge(cmdInfo.getCmdMerge());
        return getBoltClientProxy(moduleKey);
    }

    public static BoltClientProxyManager me() {
        return Holder.ME;
    }

    private BoltClientProxyManager() {
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final BoltClientProxyManager ME = new BoltClientProxyManager();
    }
}
