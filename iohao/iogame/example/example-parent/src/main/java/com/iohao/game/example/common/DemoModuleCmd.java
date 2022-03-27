/*
 * # iohao.com . 渔民小镇
 * Copyright (C) 2021 - 2022 double joker （262610965@qq.com） . All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.iohao.game.example.common;

/**
 * 逻辑服 主 cmd
 * <pre>
 *     主 cmd 放在一个文件管理，可以使得模块的主 cmd 更加的清晰
 * </pre>
 *
 * @author 洛朱
 * @date 2022-03-23
 */
public interface DemoModuleCmd {
    /** 逻辑服 示例 cmd 模块1 */
    int demoModule_1_cmd = 1;

    /** 逻辑服 示例 cmd 模块2 (spring 相关模块) */
    int demoModule_2_spring_cmd = 2;

    /** 逻辑服 示例 cmd 模块3 (战斗服 相关模块) */
    int demoModule_3_fight_cmd = 3;

    /** 逻辑服 示例 cmd 模块4 (天气预报服 相关模块) */
    int demoModule_4_weather_cmd = 4;
}
