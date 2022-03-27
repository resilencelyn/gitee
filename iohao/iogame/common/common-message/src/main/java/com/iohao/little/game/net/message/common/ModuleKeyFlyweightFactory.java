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
package com.iohao.little.game.net.message.common;

import org.jctools.maps.NonBlockingHashMap;

import java.util.Map;
import java.util.Objects;

/**
 * ModuleInfoKey 享元工厂
 *
 * @author 洛朱
 * @date 2022-03-10
 */
final class ModuleKeyFlyweightFactory {
    /**
     * <pre>
     * key : moduleId
     * value : ModuleInfoKeyRegion
     * </pre>
     */
    private final Map<Integer, ModuleKeyRegion> regionMap = new NonBlockingHashMap<>();

    /**
     * 确保只有一个服的时候，可以使用该方法
     *
     * @param moduleId 模块id
     * @return ModuleInfoKey
     */
    public ModuleKey getModuleKey(int moduleId) {
        return getModuleKey(moduleId, 0);
    }

    /**
     * 取同业务模块下的服务器 - 根据序列id获取
     *
     * @param moduleId   模块id
     * @param sequenceId 序列id
     * @return ModuleInfoKey
     */
    public ModuleKey getModuleKey(int moduleId, int sequenceId) {
        ModuleKeyRegion region = getModuleKeyRegion(moduleId);
        return region.getModuleKey(sequenceId);
    }

    private ModuleKeyRegion getModuleKeyRegion(int moduleId) {
        ModuleKeyRegion keyRegion = regionMap.get(moduleId);

        if (Objects.isNull(keyRegion)) {
            keyRegion = new ModuleKeyRegion(moduleId);
            keyRegion = regionMap.putIfAbsent(moduleId, keyRegion);
            if (Objects.isNull(keyRegion)) {
                keyRegion = regionMap.get(moduleId);
            }
        }

        return keyRegion;
    }

    private static class ModuleKeyRegion {
        /**
         * <pre>
         * key : sequenceId
         * value : ModuleInfoKey
         * </pre>
         */
        private final Map<Integer, ModuleKey> map = new NonBlockingHashMap<>();

        final int moduleId;

        ModuleKeyRegion(int moduleId) {
            this.moduleId = moduleId;
        }

        ModuleKey getModuleKey(int sequenceId) {
            ModuleKey moduleKey = map.get(sequenceId);

            if (Objects.isNull(moduleKey)) {
                moduleKey = new ModuleKey(moduleId, sequenceId);
                moduleKey = map.putIfAbsent(sequenceId, moduleKey);
                if (Objects.isNull(moduleKey)) {
                    moduleKey = map.get(sequenceId);
                }
            }

            return moduleKey;
        }
    }

    public static ModuleKeyFlyweightFactory me() {
        return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final ModuleKeyFlyweightFactory ME = new ModuleKeyFlyweightFactory();
    }
}
