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
package io.mycat.config.loader.xml;

import io.mycat.config.model.encrypt.EncryptColumn;
import io.mycat.config.model.encrypt.EncryptDataSource;
import io.mycat.config.model.encrypt.EncryptServer;
import io.mycat.config.model.encrypt.EncryptTablePk;
import io.mycat.config.util.ConfigException;
import io.mycat.config.util.ConfigUtil;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * @author zrx
 */
public class XMLEncryptLoader {

	/**
	 * zrx 添加加解密配置
	 */
	private volatile Map<String, EncryptServer> encryptConfigMap;

	public XMLEncryptLoader() {
		encryptConfigMap = new LinkedHashMap<>(6);
		this.load();
	}

	private void load() {
		//读取server.xml配置
		try (InputStream dtd = XMLServerLoader.class.getResourceAsStream("/encrypt.dtd"); InputStream xml = XMLServerLoader.class.getResourceAsStream("/encrypt.xml")) {
			Element root = ConfigUtil.getDocument(dtd, xml).getDocumentElement();
			NodeList serverList = root.getElementsByTagName("server");
			for (int i = 0, n = serverList.getLength(); i < n; i++) {
				Node serverNode = serverList.item(i);
				if (serverNode instanceof Element) {
					Element serverEl = (Element) serverNode;
					String schema = serverEl.getAttribute("schema");
					EncryptServer server = new EncryptServer();
					server.setSchema(schema);
					encryptConfigMap.put(schema, server);
					NodeList datasourceList = serverEl.getElementsByTagName("datasource");
					//datasource的map
					Map<String, EncryptDataSource> encryptDataSourceMap = new LinkedHashMap<>(6);
					server.setEncryptDataSourceMap(encryptDataSourceMap);
					for (int j = 0, m = datasourceList.getLength(); j < m; j++) {
						Node datasourceNode = datasourceList.item(j);
						if (datasourceNode instanceof Element) {
							Element datasourceEl = (Element) datasourceNode;
							String name = datasourceEl.getAttribute("name").toLowerCase();
							String ip = datasourceEl.getAttribute("ip");
							String port = datasourceEl.getAttribute("port");
							String username = datasourceEl.getAttribute("username");
							String password = datasourceEl.getAttribute("password");
							String auto = datasourceEl.getAttribute("auto");
							String dbType = datasourceEl.getAttribute("dbType");
							EncryptDataSource dataSource = new EncryptDataSource();
							dataSource.setName(name);
							dataSource.setIp(ip);
							dataSource.setPort(port);
							dataSource.setUsername(username);
							dataSource.setPassword(password);
							dataSource.setAuto(auto);
							dataSource.setDbType(dbType);
							encryptDataSourceMap.put(name, dataSource);
							NodeList columnList = datasourceEl.getElementsByTagName("column");
							//column的map
							Map<String, EncryptColumn> encryptColumnMap = new LinkedHashMap<>(6);
							Map<String, Set<String>> encryptTableColMap = new LinkedHashMap<>(6);
							dataSource.setEncryptColumnMap(encryptColumnMap);
							dataSource.setEncryptTableColMap(encryptTableColMap);
							for (int k = 0, o = columnList.getLength(); k < o; k++) {
								Node columnNode = columnList.item(k);
								if (columnNode instanceof Element) {
									Element columnEl = (Element) columnNode;
									String columnName = columnEl.getAttribute("name").toLowerCase();
									EncryptColumn column = new EncryptColumn();
									column.setName(columnName);
									encryptColumnMap.put(columnName, column);
									NodeList tableList = columnEl.getElementsByTagName("table");
									//table的map
									Map<String, String> encryptTableMap = new LinkedHashMap<>(6);
									List<EncryptTablePk> encryptTablePks = new ArrayList<>(6);
									column.setEncryptTableMap(encryptTableMap);
									column.setEncryptTablePks(encryptTablePks);
									for (int l = 0, p = tableList.getLength(); l < p; l++) {
										Node tableNode = tableList.item(l);
										if (tableNode instanceof Element) {
											Element tableEl = (Element) tableNode;
											String tableName = tableEl.getAttribute("name").toLowerCase();
											String pk = tableEl.getAttribute("pk");
											encryptTableMap.put(tableName, tableName);
											EncryptTablePk encryptTablePk = new EncryptTablePk();
											encryptTablePk.setName(tableName);
											encryptTablePk.setPk(pk);
											encryptTablePks.add(encryptTablePk);
											if (encryptTableColMap.containsKey(tableName)) {
												Set<String> columns = encryptTableColMap.get(tableName);
												columns.add(columnName);
											} else {
												Set<String> colList = new HashSet<>(3);
												colList.add(columnName);
												encryptTableColMap.put(tableName, colList);
											}

										}
									}
								}
							}
						}
					}
				}
			}
		} catch (ConfigException e) {
			throw e;
		} catch (Exception e) {
			throw new ConfigException(e);
		}
	}

	public Map<String, EncryptServer> getEncryptConfigMap() {
		return encryptConfigMap;
	}

}
