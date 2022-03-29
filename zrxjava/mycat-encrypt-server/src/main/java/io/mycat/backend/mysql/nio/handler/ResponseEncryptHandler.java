package io.mycat.backend.mysql.nio.handler;

import io.mycat.MycatServer;
import io.mycat.config.model.encrypt.EncryptServer;
import io.mycat.config.model.encrypt.constant.YesOrNo;
import io.mycat.net.mysql.RowDataPacket;
import io.mycat.route.RouteResultset;
import io.mycat.server.NonBlockingSession;
import io.mycat.server.parser.ServerParse;

import java.nio.ByteBuffer;
import java.util.Map;

/**
 * @ClassName ResponseEncryptHandler
 * @Author zrx
 * @Date 2022/2/12 21:05
 */
public class ResponseEncryptHandler {

	public static byte[] getBytes(RouteResultset rrs, NonBlockingSession session, int fieldCount, byte[] row) {
		boolean decrypt = YesOrNo.YES.getCode().equals(MycatServer.getInstance().getConfig().getSystem().getDecrypt());
		Map<String, EncryptServer> encryptConfigMap = MycatServer.getInstance().getConfig().getEncryptConfigMap();
		//zrx 处理返回结果
		if (decrypt && rrs.getSqlType() == ServerParse.SELECT && encryptConfigMap.containsKey(session.getSource().getSchema())) {
			//读取
			RowDataPacket resultsetRow = new RowDataPacket(fieldCount);
			//解密读取
			if (resultsetRow.readDecrypt(row)) {
				ByteBuffer buffer = ByteBuffer.allocate(row.length);
				resultsetRow.write(buffer);
				buffer.flip();
				byte[] bytes = new byte[buffer.remaining()];
				buffer.get(bytes, 0, bytes.length);
				//改变结果集
				row = bytes;
			}
		}
		return row;
	}
}
