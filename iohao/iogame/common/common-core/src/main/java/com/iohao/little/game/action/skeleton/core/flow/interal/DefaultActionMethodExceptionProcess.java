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

import com.iohao.little.game.action.skeleton.core.exception.ActionErrorEnum;
import com.iohao.little.game.action.skeleton.core.exception.MsgException;
import com.iohao.little.game.action.skeleton.core.flow.ActionMethodExceptionProcess;

/**
 * default ActionMethodExceptionProcess
 *
 * @author 洛朱
 * @date 2021-12-20
 */
public class DefaultActionMethodExceptionProcess implements ActionMethodExceptionProcess {
    @Override
    public MsgException processException(Throwable e) {

        if (e instanceof MsgException msgException) {
            return msgException;
        }

        // 到这里，一般不是用户自定义的错误，很可能是用户引入的第三方包的错误
        e.printStackTrace();

        return new MsgException(ActionErrorEnum.systemOtherErrCode);
    }


    private DefaultActionMethodExceptionProcess() {

    }

    public static DefaultActionMethodExceptionProcess me() {
    	return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final DefaultActionMethodExceptionProcess ME = new DefaultActionMethodExceptionProcess();
    }
}
