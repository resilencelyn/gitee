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
package com.iohao.little.game.action.skeleton.core.flow.interal;

import com.alipay.remoting.AsyncContext;
import com.alipay.remoting.rpc.RpcCommandType;
import com.iohao.little.game.action.skeleton.core.ActionCommand;
import com.iohao.little.game.action.skeleton.core.flow.ActionAfter;
import com.iohao.little.game.action.skeleton.core.flow.attr.FlowAttr;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import lombok.extern.slf4j.Slf4j;

import java.util.Objects;

/**
 * 默认的ActionAfter
 *
 * @author 洛朱
 * @date 2021-12-20
 */
@Slf4j
public class DefaultActionAfter implements ActionAfter {
    @Override
    public void execute(FlowContext flowContext) {
        final ResponseMessage response = flowContext.getResponse();

        AsyncContext asyncCtx = getAsyncContext(flowContext);

        if (Objects.isNull(asyncCtx)) {
            return;
        }

        // 有错误就响应给掉用方
        if (response.hasError()) {
            asyncCtx.sendResponse(response);
            return;
        }

        // action 方法返回值是 void 的，不做处理
        ActionCommand actionCommand = flowContext.getActionCommand();
        if (actionCommand.getActionMethodReturnInfo().isVoid()) {
            return;
        }

        // 防止 data 对象数据没有实现 Serializable 接口引发的异常
        response.setData(null);
        // 将数据回传给掉用方
        asyncCtx.sendResponse(response);
    }

    private AsyncContext getAsyncContext(FlowContext flowContext) {
        ResponseMessage response = flowContext.getResponse();
        byte rpcCommandType = response.getRpcCommandType();

        if (rpcCommandType != RpcCommandType.REQUEST_ONEWAY) {
            return flowContext.option(FlowAttr.asyncContext);
        } else {
            return flowContext.option(FlowAttr.serverContext);
        }
    }
}
