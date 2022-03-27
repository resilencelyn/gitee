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
package com.iohao.little.game.net.server.module;

import com.iohao.little.game.net.message.common.ModuleMessage;

import java.util.*;

/**
 * 模块信息域
 * <pre>
 *     既同一个业务模块起了N个服务（来负载）
 * </pre>
 *
 * @author 洛朱
 * @Date 2021-12-20
 */
public class ModuleInfoRegion {
    Set<ModuleInfoProxy> moduleInfoProxySet = new HashSet<>();
    int moduleId;
    ModuleInfoProxy moduleInfoProxy;

    public void addModuleInfo(ModuleMessage moduleMessage) {
        moduleInfoProxySet.clear();

        ModuleInfoProxy moduleInfoProxy = new ModuleInfoProxy(moduleMessage);
        moduleInfoProxySet.add(moduleInfoProxy);

        this.moduleId = moduleMessage.getModuleKey().moduleId;
        this.moduleInfoProxy = moduleInfoProxy;
    }

    public ModuleInfoProxy getModuleInfo() {
        if (moduleInfoProxySet.isEmpty()) {
            return null;
        }

        return moduleInfoProxy;
//        return moduleInfoProxySet.stream().findAny().get();
    }
}
