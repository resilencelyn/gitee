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

import com.iohao.little.game.action.skeleton.core.CmdInfo;
import com.iohao.little.game.net.message.common.ModuleMessage;
import org.jctools.maps.NonBlockingHashMap;

import java.util.Map;
import java.util.Objects;

/**
 * 模块信息管理
 *
 * @author 洛朱
 * @date 2021-12-20
 */
public class ModuleInfoManager {
    /**
     * key : cmdMerge
     * value : moduleId
     */
    Map<Integer, Integer> moduleIdMap = new NonBlockingHashMap<>();
    Map<Integer, ModuleInfoRegion> moduleInfoRegionMap = new NonBlockingHashMap<>();
    ModuleInfoRegion externalModuleInfoRegion = new ModuleInfoRegion();

    public ModuleInfoProxy getModuleInfo(CmdInfo cmdInfo) {
        Integer moduleId = moduleIdMap.get(cmdInfo.getCmdMerge());

        return this.getModuleInfoByModuleId(moduleId);
    }

    public ModuleInfoProxy getModuleInfoByModuleId(Integer moduleId) {

        if (Objects.isNull(moduleId)) {
            return null;
        }

        ModuleInfoRegion moduleInfoRegion = moduleInfoRegionMap.get(moduleId);

        if (Objects.isNull(moduleInfoRegion)) {
            return null;
        }

        return moduleInfoRegion.getModuleInfo();

    }

    public ModuleInfoProxy getExternalModuleInfo() {
        return externalModuleInfoRegion.getModuleInfo();
    }


    public void addModuleInfo(ModuleMessage moduleMessage) {
        switch (moduleMessage.getModuleType()) {
            case LOGIC -> extractedLogic(moduleMessage);
            case EXTERNAL -> externalModuleInfoRegion.addModuleInfo(moduleMessage);
        }
    }

    private void extractedLogic(ModuleMessage moduleMessage) {
        var moduleId = moduleMessage.getModuleKey().moduleId;

        ModuleInfoRegion moduleInfoRegion = moduleInfoRegionMap.get(moduleId);

        // 无锁化
        if (Objects.isNull(moduleInfoRegion)) {
            moduleInfoRegion = new ModuleInfoRegion();
            moduleInfoRegion = moduleInfoRegionMap.putIfAbsent(moduleId, moduleInfoRegion);
            if (Objects.isNull(moduleInfoRegion)) {
                moduleInfoRegion = moduleInfoRegionMap.get(moduleId);
            }
        }

        moduleInfoRegion.addModuleInfo(moduleMessage);

        int[] cmdMergeArray = moduleMessage.getCmdMergeArray();

        for (int cmdMerge : cmdMergeArray) {
            moduleIdMap.put(cmdMerge, moduleId);
        }
    }

    public static ModuleInfoManager me() {
        return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final ModuleInfoManager ME = new ModuleInfoManager();
    }


}
