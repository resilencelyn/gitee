/*
 * Copyright (c) 2020, OpenCloudDB/MyCAT and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software;Designed and Developed mainly by many Chinese
 * opensource volunteers. you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2 only, as published by the
 * Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Any questions about this component can be directed to it's project Web address
 * https://code.google.com/p/opencloudb/.
 *
 */
package io.mycat.net.mysql;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

import io.mycat.backend.mysql.BufferUtil;
import io.mycat.backend.mysql.MySQLMessage;
import io.mycat.buffer.BufferArray;
import io.mycat.config.model.encrypt.EncryptHelper;
import io.mycat.net.FrontendConnection;

/**
 * From server to client. One packet for each row in the result set.
 *
 * <pre>
 * Bytes                   Name
 * -----                   ----
 * n (Length Coded String) (column value)
 * ...
 *
 * (column value):         The data in the column, as a character string.
 *                         If a column is defined as non-character, the
 *                         server converts the value into a character
 *                         before sending it. Since the value is a Length
 *                         Coded String, a NULL can be represented with a
 *                         single byte containing 251(see the description
 *                         of Length Coded Strings in section "Elements" above).
 *
 * @see http://forge.mysql.com/wiki/MySQL_Internals_ClientServer_Protocol#Row_Data_Packet
 * </pre>
 *
 * @author mycat
 */
public class RowDataPacket extends MySQLPacket {
	private static final byte NULL_MARK = (byte) 251;
	private static final byte EMPTY_MARK = (byte) 0;


	public byte[] value;
	public int fieldCount;
	public final List<byte[]> fieldValues;

	public RowDataPacket(int fieldCount) {
		this.fieldCount = fieldCount;
		this.fieldValues = new ArrayList<>(fieldCount);
	}

	public void add(byte[] value) {
		//这里应该修改value
		fieldValues.add(value);
	}

	public void addFieldCount(int add) {
		//这里应该修改field
		fieldCount = fieldCount + add;
	}

	public void read(byte[] data) {
		value = data;
		MySQLMessage mm = new MySQLMessage(data);
		packetLength = mm.readUB3();
		packetId = mm.read();
		for (int i = 0; i < fieldCount; i++) {
			byte[] bytes = mm.readBytesWithLength();
			fieldValues.add(bytes);
		}
	}

	public boolean readDecrypt(byte[] data) {
		boolean decrypt = false;
		value = data;
		MySQLMessage mm = new MySQLMessage(data);
		packetLength = mm.readUB3();
		packetId = mm.read();
		for (int i = 0; i < fieldCount; i++) {
			byte[] bytes = mm.readBytesWithLength();
			//zrx 判断前缀是否是 !zl 否需要解密
			if (bytes != null && bytes.length > 0
					&& bytes[0] == 33 && bytes[1] == 122 && bytes[2] == 108) {
				//读取加密后的字符串
				String encryptHex = new String(bytes, StandardCharsets.UTF_8);
				//解密并设置值
				bytes = EncryptHelper.decode(EncryptHelper.hexStringToBytes(encryptHex));
				decrypt = true;
			}
			fieldValues.add(bytes);
		}
		return decrypt;
	}

	@Override
	public ByteBuffer write(ByteBuffer bb, FrontendConnection c,
							boolean writeSocketIfFull) {
		bb = c.checkWriteBuffer(bb, c.getPacketHeaderSize(), writeSocketIfFull);
		BufferUtil.writeUB3(bb, calcPacketSize());
		bb.put(packetId);
		for (int i = 0; i < fieldCount; i++) {
			byte[] fv = fieldValues.get(i);
			if (fv == null) {
				bb = c.checkWriteBuffer(bb, 1, writeSocketIfFull);
				bb.put(RowDataPacket.NULL_MARK);
			} else if (fv.length == 0) {
				bb = c.checkWriteBuffer(bb, 1, writeSocketIfFull);
				bb.put(RowDataPacket.EMPTY_MARK);
			} else {
				bb = c.checkWriteBuffer(bb, BufferUtil.getLength(fv),
						writeSocketIfFull);
				BufferUtil.writeLength(bb, fv.length);
				bb = c.writeToBuffer(fv, bb);
			}
		}
		return bb;
	}

	public void write(ByteBuffer buffer) {
		BufferUtil.writeUB3(buffer, calcPacketSize());
		buffer.put(packetId);
		for (int i = 0; i < fieldCount; i++) {
			byte[] fv = fieldValues.get(i);
			if (fv == null) {
				buffer.put(NULL_MARK);
			} else {
				BufferUtil.writeLength(buffer, fv.length);
				buffer.put(fv);
			}
		}
	}

	@Override
	public int calcPacketSize() {
		int size = 0;
		for (int i = 0; i < fieldCount; i++) {
			byte[] v = fieldValues.get(i);
			size += (v == null || v.length == 0) ? 1 : BufferUtil.getLength(v);
		}
		return size;
	}

	@Override
	protected String getPacketInfo() {
		return "MySQL RowData Packet";
	}

	public void write(BufferArray bufferArray) {
		int size = calcPacketSize();
		ByteBuffer buffer = bufferArray.checkWriteBuffer(packetHeaderSize + size);
		BufferUtil.writeUB3(buffer, size);
		buffer.put(packetId);
		for (int i = 0; i < fieldCount; i++) {
			byte[] fv = fieldValues.get(i);
			if (fv == null) {
				buffer = bufferArray.checkWriteBuffer(1);
				buffer.put(RowDataPacket.NULL_MARK);
			} else if (fv.length == 0) {
				buffer = bufferArray.checkWriteBuffer(1);
				buffer.put(RowDataPacket.EMPTY_MARK);
			} else {
				buffer = bufferArray.checkWriteBuffer(BufferUtil
						.getLength(fv.length));
				BufferUtil.writeLength(buffer, fv.length);
				bufferArray.write(fv);
			}
		}
	}

}
