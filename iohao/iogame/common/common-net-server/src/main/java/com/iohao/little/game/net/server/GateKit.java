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
package com.iohao.little.game.net.server;

import com.iohao.little.game.action.skeleton.protocol.RequestMessage;
import com.iohao.little.game.net.common.BoltServer;
import com.iohao.little.game.net.server.module.ModuleInfoManager;
import com.iohao.little.game.net.server.module.ModuleInfoProxy;
import lombok.Getter;
import lombok.Setter;
import lombok.experimental.UtilityClass;

/**
 * 网关工具
 *
 * @author 洛朱
 * @date 2021-12-20
 */
@UtilityClass
public class GateKit {
    @Getter
    @Setter
    private volatile BoltServer boltServer;

    public ModuleInfoProxy getModuleInfo(RequestMessage requestMessage) {
        var cmdInfo = requestMessage.getCmdInfo();
        // 根据 cmdInfo 查找出可以处理请求的模块
        return ModuleInfoManager.me().getModuleInfo(cmdInfo);
    }

//    /**
//     * 将请求转发到逻辑服
//     *
//     * @param asyncCtx       asyncCtx
//     * @param requestMessage 请求
//     */
//    public void sendToLogicServer(AsyncContext asyncCtx, RequestMessage requestMessage) {
//        var cmdInfo = requestMessage.getCmdInfo();
//        // 根据 cmdInfo 查找出可以处理请求的模块
//        var moduleInfo = ModuleInfoManager.me().getModuleInfo(cmdInfo);
//
//        try {
//            // 请求方 请求其它服务器得到的响应数据
//            var responseMessage = (ResponseMessage) moduleInfo.invokeSync(requestMessage);
//            // 将响应数据给回请求方
//            asyncCtx.sendResponse(responseMessage);
//        } catch (RemotingException | InterruptedException e) {
//            e.printStackTrace();
//        }
//    }
}
