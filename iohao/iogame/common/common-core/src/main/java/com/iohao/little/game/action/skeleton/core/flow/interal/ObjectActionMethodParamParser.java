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

import com.iohao.little.game.action.skeleton.core.ActionCommand;
import com.iohao.little.game.action.skeleton.core.ValidatorKit;
import com.iohao.little.game.action.skeleton.core.flow.ActionMethodParamParser;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;

/**
 * action 方法参数解析器 actionCommand
 *
 * @author 洛朱
 * @Date 2021-12-17
 */
public class ObjectActionMethodParamParser implements ActionMethodParamParser {
    private static final Object[] METHOD_PARAMS = new Object[0];

    @Override
    public Object[] listParam(final FlowContext flowContext) {

        var actionCommand = flowContext.getActionCommand();
        if (!actionCommand.isHasMethodParam()) {
            return METHOD_PARAMS;
        }

        var request = flowContext.getRequest();
        var response = flowContext.getResponse();

        final var paramInfos = actionCommand.getParamInfos();

        final var len = paramInfos.length;
        final var pars = new Object[len];

        for (int i = 0; i < len; i++) {
            ActionCommand.ParamInfo paramInfo = paramInfos[i];
            Class<?> paramClazz = paramInfo.getParamClazz();

            if (FlowContext.class.equals(paramClazz)) {
                // flow 上下文
                pars[i] = flowContext;
                continue;
            }

            pars[i] = request.getData();
            if (paramInfo.isValidator()) {
                String validateMsg = ValidatorKit.validate(pars[i]);
                response.setValidatorMsg(validateMsg);
            }

        }

        return pars;
    }
}
