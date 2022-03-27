package com.iohao.game.tank.net.onmessage;

import com.iohao.game.collect.hall.HallCmd;
import com.iohao.game.collect.proto.common.UserInfo;
import com.iohao.game.collect.proto.tank.TankEnterRoom;
import com.iohao.game.tank.net.TankOnMessage;
import com.iohao.little.game.action.skeleton.core.CmdKit;
import com.iohao.little.game.common.kit.ProtoKit;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;

/**
 * @author 洛朱
 * @date 2022-03-06
 */
public class TankLoginVerifyOnMessage implements TankOnMessage {
    @Override
    public int getCmdMerge() {
        return CmdKit.merge(HallCmd.cmd, HallCmd.loginVerify);
    }

    @Override
    public Object response(ExternalMessage externalMessage, byte[] data) {
        UserInfo userInfo = ProtoKit.parseProtoByte(data, UserInfo.class);

        // 这里可以先加入房间
        TankEnterRoom tankEnterRoom = new TankEnterRoom();
        tankEnterRoom.team = 1;
        TankEnterRoomOnMessage.me().request(tankEnterRoom);


        return userInfo;
    }


    private TankLoginVerifyOnMessage() {

    }

    public static TankLoginVerifyOnMessage me() {
        return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final TankLoginVerifyOnMessage ME = new TankLoginVerifyOnMessage();
    }
}
