package com.iohao.game.tank.net;

import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessageCmdCode;

/**
 * @author 洛朱
 * @date 2022-03-06
 */
public interface TankOnMessage {

    int getCmdMerge();

    Object response(ExternalMessage externalMessage, byte[] data);

    default void request(Object data) {
        this.request(data, null);
    }

    default void request(Object data, Runnable runnable) {
        ExternalMessage externalMessage = this.createExternalMessage();

        externalMessage.setData(data);

        TankWebsocketClient.me().request(externalMessage);

        if (runnable != null) {
            TankWebsocketClient.me().getActionMap().put(this.getCmdMerge(), runnable);
        }
    }


    private ExternalMessage createExternalMessage() {

        ExternalMessage request = new ExternalMessage();
        request.setCmdCode(ExternalMessageCmdCode.biz);
        request.setProtocolSwitch(0);

        request.setCmdMerge(this.getCmdMerge());

        return request;
    }


}
