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
package com.iohao.little.game.common.kit.attr;

import java.util.Objects;

/**
 * 属性项
 *
 * @author 洛朱
 * @date 2022-01-31
 */
public record AttrOption<T>(String name, T defaultValue) {

    public static <T> AttrOption<T> valueOf(String name) {
        return new AttrOption<T>(name, null);
    }

    public static <T> AttrOption<T> valueOf(String name, T defaultValue) {
        return new AttrOption<T>(name, defaultValue);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        AttrOption<?> that = (AttrOption<?>) o;

        return Objects.equals(name, that.name);
    }

    @Override
    public int hashCode() {
        return name != null ? name.hashCode() : 0;
    }
}
