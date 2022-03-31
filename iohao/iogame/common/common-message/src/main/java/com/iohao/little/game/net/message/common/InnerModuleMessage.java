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

import com.iohao.little.game.action.skeleton.protocol.RequestMessage;
import lombok.Data;

import java.io.Serializable;

/**
 * 子模块之间的访问
 * <pre>
 *     如： 子模块A 访问 子模块B 的某个方法，因为只有子模块B持有这些数据
 * </pre>
 *
 * @author 洛朱
 * @date 2021-12-20
 */
@Data
public class InnerModuleMessage implements Serializable {
    RequestMessage requestMessage;
}
