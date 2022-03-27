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
package com.iohao.little.game.action.skeleton.protocol;

import com.iohao.little.game.action.skeleton.core.exception.ActionErrorEnum;
import com.iohao.little.game.action.skeleton.core.CmdInfo;
import com.iohao.little.game.action.skeleton.core.CmdInfoFlyweightFactory;
import com.iohao.little.game.action.skeleton.core.CmdKit;
import lombok.AccessLevel;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;
import lombok.experimental.Accessors;
import lombok.experimental.FieldDefaults;

import java.io.Serial;
import java.io.Serializable;

/**
 * 消息基类
 *
 * @author 洛朱
 * @Date 2021-12-20
 */
@ToString
@Getter
@Setter
@FieldDefaults(level = AccessLevel.PROTECTED)
@Accessors(chain = true)
public abstract class BarMessage implements Serializable {
    @Serial
    private static final long serialVersionUID = 562068269463876111L;
    /** 目标路由 */
    int cmd;
    /** 目标子路由 */
    int subCmd;
    /**
     * 合并两个参数,分别存放在 [高16 和 低16]
     * <pre>
     *     例如 cmd = 600; subCmd = 700;
     *     merge 的结果: 39322300
     *     那么merge对应的二进制是: 0000 0010 0101 1000 0000 0010 1011 1100
     * </pre>
     */
    int cmdMerge;

    /**
     * rpc type
     * <pre>
     *     see : {@link com.alipay.remoting.rpc.RpcCommandType}
     *
     *     在 bolt 中， 掉用方使用
     *     （{@link com.alipay.remoting.rpc.RpcServer#oneway}
     *     或 {@link com.alipay.remoting.rpc.RpcClient#oneway}）的 oneway 方法
     *
     *     则 AsyncContext.sendResponse 无法回传响应
     *     原因可阅读 {@link com.alipay.remoting.rpc.protocol.RpcRequestProcessor#sendResponseIfNecessary} 源码。
     *
     *
     *     业务框架保持与 bolt 的风格一至使用 RpcCommandType
     *     不同的是业务框架会用 RpcCommandType 区别使用什么方式来发送响应。
     *
     *     如果 rpcCommandType != RpcCommandType.REQUEST_ONEWAY ,
     *     就使用 {@link com.alipay.remoting.AsyncContext#sendResponse} 来发送响应
     *     具体发送逻辑可读 {@link com.iohao.little.game.action.skeleton.core.flow.interal.DefaultActionAfter} 源码
     *
     * </pre>
     */
    byte rpcCommandType;

    /** 响应码: 0:成功, 其他表示有错误 */
    int responseStatus;
    /** JSR303、JSR 349 验证信息 */
    String validatorMsg;

    /** userId */
    long userId;
    /** 一般指用户的 channelId （来源于对外服的 channel 长连接） */
    String userChannelId;

    /** 实际请求的业务参数 */
    Object data;
    /** 实际请求的业务参数 byte[] */
    byte[] dataContent;

    public BarMessage setCmdInfo(CmdInfo cmdInfo) {
        this.cmd = cmdInfo.getCmd();
        this.subCmd = cmdInfo.getSubCmd();
        this.cmdMerge = cmdInfo.getCmdMerge();

        return this;
    }

    public CmdInfo getCmdInfo() {
        return CmdInfoFlyweightFactory.me().getCmdInfo(this.cmd, this.subCmd);
    }

    public BarMessage setCmdMerge(int cmdMerge) {
        this.cmdMerge = cmdMerge;
        this.cmd = CmdKit.getCmd(cmdMerge);
        this.subCmd = CmdKit.getSubCmd(cmdMerge);

        return this;
    }

    /**
     * 设置验证的错误信息
     *
     * @param validatorMsg 错误信息
     * @return this
     */
    public BarMessage setValidatorMsg(String validatorMsg) {
        if (validatorMsg != null) {
            this.validatorMsg = validatorMsg;
            this.responseStatus = ActionErrorEnum.validateErrCode.getCode();
        }

        return this;
    }

    /**
     * 是否有错误
     * <pre>
     *     this.errorCode != 0 表示有错误
     * </pre>
     *
     * @return true 有错误码
     */
    public boolean hasError() {
        return this.responseStatus != 0;
    }
}
