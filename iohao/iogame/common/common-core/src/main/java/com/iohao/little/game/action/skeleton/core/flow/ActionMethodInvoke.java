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
package com.iohao.little.game.action.skeleton.core.flow;

/**
 * ActionMethod Invoke
 * <pre>
 *     掉用业务层的方法 (即对外提供的方法)
 * </pre>
 *
 * @author 洛朱
 * @date 2021-12-20
 */
public interface ActionMethodInvoke {
    /**
     * 具体的业务方法掉用
     * <pre>
     *     类有上有注解 {@link com.iohao.little.game.action.skeleton.annotation.ActionController}
     *     方法有注解 {@link com.iohao.little.game.action.skeleton.annotation.ActionMethod}
     *     只要有这两个注解的，就是业务类
     * </pre>
     *
     * @param flowContext flow 上下文
     * @return 返回值
     */
    Object invoke(FlowContext flowContext);

}
