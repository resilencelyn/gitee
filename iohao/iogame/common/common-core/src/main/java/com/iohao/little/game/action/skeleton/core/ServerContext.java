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

import com.alipay.remoting.AsyncContext;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import com.iohao.little.game.action.skeleton.protocol.RequestMessage;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;

import java.util.Collection;

/**
 * 当前服务器上下文
 *
 * @author 洛朱
 * @date 2021-12-20
 */
public interface ServerContext extends AsyncContext {
    /**
     * 广播消息给 userIdList
     *
     * @param flowContext flowContext
     * @param userIdList  userIdList
     */
    void broadcast(FlowContext flowContext, Collection<Long> userIdList);

    /**
     * 根据路由信息来请求其他子服务器（其他逻辑服）的数据
     *
     * @param requestMessage requestMessage
     * @return ResponseMessage
     */
    ResponseMessage invokeModuleMessage(RequestMessage requestMessage);
}
