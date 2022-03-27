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
package com.iohao.little.game.broadcast;

import com.alipay.remoting.exception.RemotingException;
import com.iohao.little.game.action.skeleton.core.BarSkeleton;
import com.iohao.little.game.action.skeleton.core.flow.ActionMethodResultWrap;
import com.iohao.little.game.action.skeleton.core.flow.FlowContext;
import com.iohao.little.game.action.skeleton.protocol.ResponseMessage;
import com.iohao.little.game.net.client.common.BoltClientProxy;
import com.iohao.little.game.net.message.common.BroadcastMessage;

import java.util.Collection;
import java.util.Objects;

/**
 * 广播相关操作
 *
 * @author 洛朱
 * @date 2022-01-29
 */
public record Broadcast(BoltClientProxy boltClientProxy) {
    /**
     * 广播给指定用户列表
     *
     * @param flowContext flowContext
     * @param userIdList  用户列表
     */
    public void broadcast(FlowContext flowContext, Collection<Long> userIdList) {
        this.internalBroadcast(flowContext, userIdList);
    }

    private void internalBroadcast(FlowContext flowContext, Collection<Long> userIdList) {

        ResponseMessage responseMessage = getResponseMessage(flowContext);

        // 广播上下文
        BroadcastMessage broadcastMessage = new BroadcastMessage();
        broadcastMessage.setResponseMessage(responseMessage);
        broadcastMessage.setUserIdList(userIdList);

        try {
            this.boltClientProxy.oneway(broadcastMessage);
        } catch (RemotingException e) {
            e.printStackTrace();
        }
    }

    /**
     * 广播给全部用户
     *
     * @param flowContext flowContext
     */
    public void broadcastAll(FlowContext flowContext) {
        this.broadcast(flowContext, -1);
    }

    /**
     * 广播给单个用户
     *
     * @param flowContext flowContext
     * @param userId      接收广播的用户
     */
    public void broadcast(FlowContext flowContext, long userId) {
        ResponseMessage response = this.getResponseMessage(flowContext);

        this.internalBroadcast(response, userId);
    }

    /**
     * 给单个用户广播
     *
     * @param responseMessage 消息
     * @param userId          userId -1 给全体广播
     */
    private void internalBroadcast(ResponseMessage responseMessage, long userId) {
        if (userId > 0) {
            responseMessage.setUserId(userId);
        }

        // 广播上下文
        BroadcastMessage broadcastMessage = new BroadcastMessage();
        broadcastMessage.setResponseMessage(responseMessage);
        broadcastMessage.setBroadcastAll(userId == -1);


        try {
            this.boltClientProxy.oneway(broadcastMessage);
        } catch (RemotingException e) {
            e.printStackTrace();
        }

    }

    private ResponseMessage getResponseMessage(FlowContext flowContext) {
        // 业务方法的返回值
        Objects.requireNonNull(flowContext.getMethodResult());

        BarSkeleton barSkeleton = flowContext.getBarSkeleton();
        ActionMethodResultWrap actionMethodResultWrap = barSkeleton.getActionMethodResultWrap();
        actionMethodResultWrap.wrap(flowContext);

        return flowContext.getResponse();
    }

}
