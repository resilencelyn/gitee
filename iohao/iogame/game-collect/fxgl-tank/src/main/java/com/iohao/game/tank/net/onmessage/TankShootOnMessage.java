package com.iohao.game.tank.net.onmessage;

import com.iohao.game.collect.proto.tank.TankBullet;
import com.iohao.game.collect.tank.TankCmd;
import com.iohao.game.tank.net.TankOnMessage;
import com.iohao.little.game.action.skeleton.core.CmdKit;
import com.iohao.little.game.common.kit.ProtoKit;
import com.iohao.little.game.net.external.bootstrap.message.ExternalMessage;

/**
 * @author 洛朱
 * @date 2022-03-06
 */
public class TankShootOnMessage implements TankOnMessage {

    @Override
    public int getCmdMerge() {
        return CmdKit.merge(TankCmd.cmd, TankCmd.shooting);
    }

    @Override
    public Object response(ExternalMessage externalMessage, byte[] data) {
        TankBullet tankBullet = ProtoKit.parseProtoByte(data, TankBullet.class);

        return tankBullet;
    }

    private TankShootOnMessage() {

    }

    public static TankShootOnMessage me() {
        return Holder.ME;
    }

    /** 通过 JVM 的类加载机制, 保证只加载一次 (singleton) */
    private static class Holder {
        static final TankShootOnMessage ME = new TankShootOnMessage();
    }
}
