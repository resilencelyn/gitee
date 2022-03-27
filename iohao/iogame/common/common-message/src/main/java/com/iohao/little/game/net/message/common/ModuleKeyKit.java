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

import lombok.experimental.UtilityClass;
import org.jctools.maps.NonBlockingHashMap;

import java.util.Map;

/**
 * 模块相关工具
 *
 * @author 洛朱
 * @date 2022-03-10
 */
@UtilityClass
public class ModuleKeyKit {

    /**
     * key : cmdMerge
     * value : moduleKey
     */
    final Map<Integer, ModuleKey> cmdMergeModuleKeyMap = new NonBlockingHashMap<>();

    /**
     * cmdMerge 与 moduleKey 关联
     * <pre>
     *     之后可以通过 cmdMerge 查找 moduleKey
     * </pre>
     *
     * @param cmdMergeArray cmdMergeArray
     * @param moduleKey     moduleKey
     */
    public void relation(int[] cmdMergeArray, ModuleKey moduleKey) {
        for (int cmdMerge : cmdMergeArray) {
            cmdMergeModuleKeyMap.put(cmdMerge, moduleKey);
        }
    }

    /**
     * 通过 cmdMerge 查找 moduleKey
     *
     * @param cmdMerge cmdMerge
     * @return moduleKey
     */
    public ModuleKey getModuleKeyByCmdMerge(int cmdMerge) {
        return cmdMergeModuleKeyMap.get(cmdMerge);
    }

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
        return ModuleKeyFlyweightFactory.me().getModuleKey(moduleId, sequenceId);
    }

}
