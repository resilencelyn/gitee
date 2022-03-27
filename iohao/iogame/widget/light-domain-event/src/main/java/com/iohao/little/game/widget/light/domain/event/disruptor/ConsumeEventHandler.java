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
package com.iohao.little.game.widget.light.domain.event.disruptor;

import com.iohao.little.game.widget.light.domain.event.message.DomainEventHandler;
import com.lmax.disruptor.EventHandler;

/**
 * @author 洛朱
 * @date 2021-12-26
 */
public record ConsumeEventHandler(
        DomainEventHandler<?> eventHandler) implements EventHandler<EventDisruptor> {

    @Override
    public void onEvent(EventDisruptor event, long sequence, boolean endOfBatch) {
        if (event.isEventSource()) {
            eventHandler.onEvent(event.getDomainEventSource(), sequence, endOfBatch);
        } else {
            eventHandler.onEvent(event.getValue(), sequence, endOfBatch);
        }
    }
}
