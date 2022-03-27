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

import com.iohao.little.game.action.skeleton.core.exception.MsgException;
import com.iohao.little.game.action.skeleton.core.flow.ActionMethodResultWrap;
import com.iohao.little.game.action.skeleton.core.flow.attr.FlowAttr;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.common.kit.ProtoKit;
import lombok.extern.slf4j.Slf4j;

import java.util.Objects;

/**
 * proto 结果包装器
 *
 * @author 洛朱
 * @date 2022-01-12
 */
@Slf4j
public class DefaultActionMethodResultWrap implements ActionMethodResultWrap {
    @Override
    public void wrap(FlowContext flowContext) {
        final ResponseMessage responseMessage = flowContext.getResponse();
        // 业务方法的返回值
        final Object result = flowContext.getMethodResult();

        if (flowContext.isError()) {
            MsgException msgException = (MsgException) result;
            // 异常处理
            int code = msgException.getMsgCode();
            responseMessage.setResponseStatus(code);
            flowContext.option(FlowAttr.msgException, msgException.getMessage());
            return;
        }

        if (Objects.isNull(result) && !flowContext.getActionCommand().getActionMethodReturnInfo().isVoid()) {
            return;
        }

        // 业务方法返回值
        byte[] dataContent = ProtoKit.toBytes(result);
        responseMessage.setDataContent(dataContent);
    }


    private DefaultActionMethodResultWrap() {

    }

    public static DefaultActionMethodResultWrap me() {
    	return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final DefaultActionMethodResultWrap ME = new DefaultActionMethodResultWrap();
    }
}
