package io.mycat.config.model.encrypt;

import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * @ClassName EncryptDataSource
 * @Author zrx
 * @Date 2022/1/31 12:12
 */
public class EncryptDataSource {
	private String name;
	private String ip;
	private String port;
	private String username;
	private String password;
	private String auto;
	private String dbType;
	private Map<String, Set<String>> encryptTableColMap;
	private Map<String, EncryptColumn> encryptColumnMap;

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getIp() {
		return ip;
	}

	public void setIp(String ip) {
		this.ip = ip;
	}

	public String getPort() {
		return port;
	}

	public void setPort(String port) {
		this.port = port;
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public String getAuto() {
		return auto;
	}

	public void setAuto(String auto) {
		this.auto = auto;
	}

	public String getDbType() {
		return dbType;
	}

	public void setDbType(String dbType) {
		this.dbType = dbType;
	}

	public Map<String, EncryptColumn> getEncryptColumnMap() {
		return encryptColumnMap;
	}

	public void setEncryptColumnMap(Map<String, EncryptColumn> encryptColumnMap) {
		this.encryptColumnMap = encryptColumnMap;
	}

	public Map<String, Set<String>> getEncryptTableColMap() {
		return encryptTableColMap;
	}

	public void setEncryptTableColMap(Map<String, Set<String>> encryptTableColMap) {
		this.encryptTableColMap = encryptTableColMap;
	}
}
