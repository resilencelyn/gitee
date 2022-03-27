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


import com.iohao.little.game.action.skeleton.annotation.ActionMethod;
import lombok.experimental.UtilityClass;

import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Arrays;
import java.util.Map;
import java.util.Objects;
import java.util.function.Predicate;
import java.util.stream.Stream;

/**
 * map 转 数组
 * <BR>
 *
 * @author 洛朱
 * @Date 2021-12-12
 */
@UtilityClass
class BarInternalKit {

    private final ActionCommand[][] EMPTY = new ActionCommand[0][0];

    /**
     * 查找类的所有方法
     * <pre>
     *     返回方法上带有 ActionMethod 的方法对象
     *
     *     不返回: 静态方法, 私有方法
     * </pre>
     *
     * @param actionControllerClass 类
     * @return 方法对象列表
     */
    Stream<Method> getMethodStream(Class<?> actionControllerClass) {
        return Arrays.stream(actionControllerClass.getDeclaredMethods())
                .filter(filterActionMethod());
    }

    private Predicate<Method> filterActionMethod() {
        return method -> {
            int mod = method.getModifiers();
            // 静态方法 与 私有方法不做处理
            if (Modifier.isStatic(mod) || Modifier.isPrivate(mod)) {
                return false;
            }

            // 必须实现注解的方法才处理
            return Objects.nonNull(method.getAnnotation(ActionMethod.class));
        };
    }

    /**
     * 将 map 转换成二维数组
     * <pre>
     *     第一维: cmd
     *     第二维: cmd 下面的子 subCmd
     * </pre>
     *
     * @param map                map
     * @param barSkeletonSetting config
     * @return 二维数组
     */
    ActionCommand[][] convertArray(Map<Integer, Map<Integer, ActionCommand>> map, BarSkeletonSetting barSkeletonSetting) {

        if (Objects.isNull(map) || map.isEmpty()) {
            return EMPTY;
        }

        // 获取主路由最大值
        int max = getMaxCmd(map, barSkeletonSetting);

        var behaviors = new ActionCommand[max][1];

        map.keySet().forEach(cmd -> {
            var subActionMap = map.get(cmd);
            // 子路由最大值
            int subCmdMax = (subActionMap.keySet().stream().max(Integer::compareTo).orElse(0)) + 1;

            ActionCommand[] subBehaviors = new ActionCommand[subCmdMax];

            for (Map.Entry<Integer, ActionCommand> subEntry : subActionMap.entrySet()) {
                subBehaviors[subEntry.getKey()] = subEntry.getValue();
            }

            behaviors[cmd] = subBehaviors;
        });

        return behaviors;
    }

    private int getMaxCmd(Map<Integer, Map<Integer, ActionCommand>> map, BarSkeletonSetting barSkeletonSetting) {
        // 获取最大的路由数字 并且+1
        int max = (map.keySet().stream().max(Integer::compareTo).get()) + 1;

        if (max > barSkeletonSetting.getCmdMaxLen()) {

            String info = String.format("cmd 超过最大默认值! 如果有需要, 请手动设置容量!  默认最大容量 %s. 当前容量 %s"
                    , barSkeletonSetting.getCmdMaxLen(), max
            );

            throw new RuntimeException(info);
        }

        // subCmd
        for (Map<Integer, ActionCommand> subActionMap : map.values()) {
            int subCmdMax = (subActionMap.keySet().stream().max(Integer::compareTo).orElse(0)) + 1;

            if (subCmdMax > barSkeletonSetting.getSubCmdMaxLen()) {

                String info = String.format("subCmd 超过最大默认值! 如果有需要, 请手动设置容量!  默认最大容量 %s. 当前容量 %s"
                        , barSkeletonSetting.getSubCmdMaxLen(), subCmdMax
                );

                throw new RuntimeException(info);
            }
        }

        return max;
    }

}
