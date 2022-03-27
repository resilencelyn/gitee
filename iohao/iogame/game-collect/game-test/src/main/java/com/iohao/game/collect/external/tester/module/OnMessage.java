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
package com.iohao.game.collect.external.tester.module;

import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessageCmdCode;

import java.util.Objects;

/**
 * @author 洛朱
 * @date 2022-01-29
 */
public interface OnMessage {
    Object process(ExternalMessage message, byte[] data);

    int getCmdMerge();

    Object requestData();

    default Class<?> bizDataClass() {
        return null;
    }

    default ExternalMessage createExternalMessage() {

        ExternalMessage request = new ExternalMessage();
        request.setCmdCode(ExternalMessageCmdCode.biz);
        request.setProtocolSwitch(0);

        request.setCmdMerge(this.getCmdMerge());

        if (Objects.nonNull(this.requestData())) {
            request.setData(this.requestData());
        }

        return request;
    }

}
