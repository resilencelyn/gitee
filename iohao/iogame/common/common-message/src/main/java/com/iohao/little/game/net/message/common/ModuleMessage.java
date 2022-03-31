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

import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;
import lombok.experimental.Accessors;
import lombok.experimental.FieldDefaults;

import java.io.Serial;
import java.io.Serializable;
import java.util.Objects;

/**
 * 子模块信息 （子游戏服的信息、逻辑服）
 *
 * @author 洛朱
 * @date 2022-03-10
 */
@Getter
@Setter
@Accessors(chain = true)
@FieldDefaults(level = AccessLevel.PRIVATE)
public class ModuleMessage implements Serializable {
    @Serial
    private static final long serialVersionUID = -1570849960266785140L;
    /** 模块key */
    ModuleKey moduleKey;
    /** 模块名 */
    String name;
    /** 描述 */
    String description;
    /** 逻辑服地址 */
    String address;
    int[] cmdMergeArray;
    /** 逻辑服类型 */
    ModuleType moduleType = ModuleType.LOGIC;

    public ModuleMessage(ModuleKey moduleKey) {
        this.moduleKey = moduleKey;
    }

    public ModuleMessage() {
    }

    @Override
    public String toString() {
        return "ModuleMessage{" +
                "moduleKey=" + moduleKey +
                ", name='" + name + '\'' +
                ", description='" + description + '\'' +
                ", moduleType='" + moduleType + '\'' +
                '}';
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (!(o instanceof ModuleMessage that)) {
            return false;
        }

        return Objects.equals(getModuleKey(), that.getModuleKey());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getModuleKey());
    }
}
