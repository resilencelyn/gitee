package io.mycat.config.model.encrypt;

import java.util.Map;

/**
 * @ClassName EncryptConfig 仅支持mysql
 * @Author zrx
 * @Date 2022/1/31 11:29
 */
public class EncryptServer {

	/**
	 * schema.xml中的schema
	 */
	private String schema;
	private Map<String, EncryptDataSource> encryptDataSourceMap;

	public Map<String, EncryptDataSource> getEncryptDataSourceMap() {
		return encryptDataSourceMap;
	}

	public void setEncryptDataSourceMap(Map<String, EncryptDataSource> encryptDataSourceMap) {
		this.encryptDataSourceMap = encryptDataSourceMap;
	}

	public String getSchema() {
		return schema;
	}

	public void setSchema(String schema) {
		this.schema = schema;
	}
}
