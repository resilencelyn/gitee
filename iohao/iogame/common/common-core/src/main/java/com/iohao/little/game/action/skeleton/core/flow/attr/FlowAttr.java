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
package com.iohao.little.game.action.skeleton.core.flow.attr;

import com.alipay.remoting.AsyncContext;
import com.iohao.little.game.action.skeleton.core.ServerContext;

/**
 * flow 上下文的一些扩展属性
 *
 * @author 洛朱
 * @date 2022-01-31
 */
public interface FlowAttr {
    /** 异常消息 */
    FlowOption<String> msgException = FlowOption.valueOf("msgException");
    /** 当前项目启动的服务上下文（当前服务器） */
    FlowOption<ServerContext> serverContext = FlowOption.valueOf("serverContext");
    /** bolt async 上下文 */
    FlowOption<AsyncContext> asyncContext = FlowOption.valueOf("asyncContext");
}
