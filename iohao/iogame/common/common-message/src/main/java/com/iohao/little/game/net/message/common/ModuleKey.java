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

import lombok.ToString;

import java.io.Serial;
import java.io.Serializable;
import java.util.Objects;

/**
 * 模块key
 * <pre>
 *     子服务器的唯一标识
 *
 *     moduleId : 表示相同业务的模块
 *     sequenceId : 相同业务模块启动了多个服务器
 *     如： 天气预报模块，启动了N台服务器提供查询
 * </pre>
 *
 * @author 洛朱
 * @date 2022-03-10
 */
@ToString
public class ModuleKey implements Serializable {
    @Serial
    private static final long serialVersionUID = 2294369045536024682L;
    /** 模块id */
    public final int moduleId;
    /** 序列 */
    public final int sequenceId;
    public final String unionId;

    ModuleKey(int moduleId, int sequenceId) {
        this.moduleId = moduleId;
        this.sequenceId = sequenceId;
        this.unionId = moduleId + "-" + sequenceId;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        ModuleKey moduleKey = (ModuleKey) o;
        return Objects.equals(unionId, moduleKey.unionId);
    }

    @Override
    public int hashCode() {
        return Objects.hash(unionId);
    }
}
