package io.mycat.config.model.encrypt;

import java.util.List;
import java.util.Map;

/**
 * @ClassName EncryptColumn
 * @Author zrx
 * @Date 2022/1/31 12:15
 */
public class EncryptColumn {
	private String name;
	private Map<String, String> encryptTableMap;
	private List<EncryptTablePk> encryptTablePks;

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public Map<String, String> getEncryptTableMap() {
		return encryptTableMap;
	}

	public void setEncryptTableMap(Map<String, String> encryptTableMap) {
		this.encryptTableMap = encryptTableMap;
	}

	public List<EncryptTablePk> getEncryptTablePks() {
		return encryptTablePks;
	}

	public void setEncryptTablePks(List<EncryptTablePk> encryptTablePks) {
		this.encryptTablePks = encryptTablePks;
	}
}
