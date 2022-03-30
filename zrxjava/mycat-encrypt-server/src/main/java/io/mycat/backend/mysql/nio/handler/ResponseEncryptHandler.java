package io.mycat.backend.mysql.nio.handler;

import io.mycat.MycatServer;
import io.mycat.config.model.encrypt.EncryptServer;
import io.mycat.config.model.encrypt.constant.YesOrNo;
import io.mycat.net.mysql.RowDataPacket;
import io.mycat.route.RouteResultset;
import io.mycat.server.NonBlockingSession;
import io.mycat.server.parser.ServerParse;
import io.mycat.util.StringUtil;
import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.nio.ByteBuffer;
import java.util.Map;

/**
 * @ClassName ResponseEncryptHandler
 * @Author zrx
 * @Date 2022/2/12 21:05
 */
public class ResponseEncryptHandler {

	private static final Logger LOGGER = LoggerFactory.getLogger("ResponseEncryptHandler");

	public static byte[] getBytes(RouteResultset rrs, NonBlockingSession session, int fieldCount, byte[] row) {
		boolean decrypt = YesOrNo.YES.getCode().equals(MycatServer.getInstance().getConfig().getSystem().getDecrypt());
		Map<String, EncryptServer> encryptConfigMap = MycatServer.getInstance().getConfig().getEncryptConfigMap();
		String schema = session.getSource().getSchema();
		LOGGER.debug("decrypt:" + decrypt);
		LOGGER.debug("schema:" + schema);
		LOGGER.debug("decrypt schemas:" + encryptConfigMap.keySet().toString());
		//zrx 处理返回结果 StringUtils.isBlank(schema)) 有的连接工具如 dbeaver 会去除 schema，导致为 null
		if (decrypt && rrs.getSqlType() == ServerParse.SELECT && (encryptConfigMap.containsKey(schema) || StringUtils.isBlank(schema))) {
			LOGGER.debug("decrypt start");
			//读取
			RowDataPacket resultsetRow = new RowDataPacket(fieldCount);
			//解密读取
			LOGGER.debug("decrypt row start");
			if (resultsetRow.readDecrypt(row)) {
				LOGGER.debug("decrypt row...");
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
