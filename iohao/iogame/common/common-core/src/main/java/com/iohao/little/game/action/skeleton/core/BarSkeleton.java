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

import com.iohao.little.game.action.skeleton.core.doc.ActionSendDocs;
import com.iohao.little.game.action.skeleton.core.doc.ErrorCodeDocs;
import com.iohao.little.game.action.skeleton.core.flow.*;
import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;
import lombok.experimental.Accessors;


/**
 * 整个核心的骨架.积木骷髅 (业务框架)
 * <p>
 * 其实这就是一个积木骷髅的游戏.
 * <pre>
 * - ta可以处理所有来访者 {@link Handler}, 你可以为ta添加多种处理链,直到你满意为止.
 * - ta可以拿起笔和本子 {@link ActionMethodInOut}, 记录所看到的东西.
 * - 处理完后你会让ta如何收尾 {@link ActionAfter}
 * </pre>
 *
 * @author 洛朱
 * @Date 2021-12-12
 */
@Accessors(chain = true)
@Setter(AccessLevel.PACKAGE)
@Getter(AccessLevel.PACKAGE)
public class BarSkeleton {

    /** ActionCommandManager */
    @Getter
    final ActionCommandManager actionCommandManager = new ActionCommandManager();
    /** handler array */
    final Handler[] handlers;
    /** handler not null, 表示只有一个 handler */
    final Handler handler;

    /** InOut 插件相关 */
    InOutManager inOutManager;
    /** 命令执行器 */
    ActionCommandFlowExecute actionCommandFlowExecute;
    /** tcp action 对象创建工厂 */
    ActionFactoryBean<Object> actionFactoryBean;
    /** InvokeActionMethod */
    ActionMethodInvoke actionMethodInvoke;
    /** 方法参数解析器 */
    ActionMethodParamParser actionMethodParamParser;
    /** 异常处理 */
    @Getter
    ActionMethodExceptionProcess actionMethodExceptionProcess;
    /** 结果包装器 */
    @Getter
    ActionMethodResultWrap actionMethodResultWrap;
    /** 框架执行完后, 最后需要做的事. 一般用于write数据到客户端 */
    ActionAfter actionAfter;

    /** 响应对象的创建 */
    ResponseMessageCreate responseMessageCreate;
    /** 推送相关的文档 */
    @Getter
    ActionSendDocs actionSendDocs;
    /** 错误码相关的文档 */
    @Getter
    ErrorCodeDocs errorCodeDocs;

    BarSkeleton(Handler[] handlers) {
        this.handlers = handlers;
        if (this.handlers.length == 1) {
            this.handler = handlers[0];
        } else {
            this.handler = null;
        }
    }

    public static BarSkeletonBuilder newBuilder() {
        return new BarSkeletonBuilder();
    }

    public void handle(FlowContext flowContext) {
        flowContext.setBarSkeleton(this);
        if (this.handler != null) {
            this.handler.handler(flowContext);
            return;
        }

        for (Handler theHandler : handlers) {
            if (!theHandler.handler(flowContext)) {
                return;
            }
        }
    }
}
