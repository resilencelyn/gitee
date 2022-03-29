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
package io.mycat.server;

import io.mycat.MycatServer;
import io.mycat.backend.mysql.listener.DefaultSqlExecuteStageListener;
import io.mycat.backend.mysql.listener.SqlExecuteStageListener;
import io.mycat.config.ErrorCode;
import io.mycat.config.MycatConfig;
import io.mycat.config.model.SchemaConfig;
import io.mycat.config.model.SystemConfig;
import io.mycat.config.model.encrypt.EncryptDataSource;
import io.mycat.config.model.encrypt.EncryptHelper;
import io.mycat.config.model.encrypt.EncryptServer;
import io.mycat.config.model.encrypt.constant.YesOrNo;
import io.mycat.config.model.encrypt.util.MySelectVisitor;
import io.mycat.net.FrontendConnection;
import io.mycat.route.RouteResultset;
import io.mycat.server.handler.MysqlProcHandler;
import io.mycat.server.parser.ServerParse;
import io.mycat.server.response.Heartbeat;
import io.mycat.server.response.InformationSchemaProfiling;
import io.mycat.server.response.InformationSchemaProfilingSqlyog;
import io.mycat.server.response.Ping;
import io.mycat.server.util.SchemaUtil;
import io.mycat.util.SplitUtil;
import io.mycat.util.TimeUtil;
import net.sf.jsqlparser.expression.Alias;
import net.sf.jsqlparser.expression.BinaryExpression;
import net.sf.jsqlparser.expression.Expression;
import net.sf.jsqlparser.expression.Parenthesis;
import net.sf.jsqlparser.expression.StringValue;
import net.sf.jsqlparser.expression.operators.relational.EqualsTo;
import net.sf.jsqlparser.expression.operators.relational.ExpressionList;
import net.sf.jsqlparser.expression.operators.relational.InExpression;
import net.sf.jsqlparser.expression.operators.relational.ItemsList;
import net.sf.jsqlparser.expression.operators.relational.LikeExpression;
import net.sf.jsqlparser.expression.operators.relational.NotEqualsTo;
import net.sf.jsqlparser.parser.CCJSqlParserUtil;
import net.sf.jsqlparser.schema.Column;
import net.sf.jsqlparser.schema.Table;
import net.sf.jsqlparser.statement.Statement;
import net.sf.jsqlparser.statement.delete.Delete;
import net.sf.jsqlparser.statement.insert.Insert;
import net.sf.jsqlparser.statement.select.FromItem;
import net.sf.jsqlparser.statement.select.Join;
import net.sf.jsqlparser.statement.select.PlainSelect;
import net.sf.jsqlparser.statement.select.Select;
import net.sf.jsqlparser.statement.select.SelectBody;
import net.sf.jsqlparser.statement.select.SetOperationList;
import net.sf.jsqlparser.statement.select.SubSelect;
import net.sf.jsqlparser.statement.update.Update;
import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.channels.NetworkChannel;
import java.nio.charset.StandardCharsets;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * @author mycat
 */
public class ServerConnection extends FrontendConnection {
	private static final Logger LOGGER = LoggerFactory
			.getLogger(ServerConnection.class);
	private long authTimeout = SystemConfig.DEFAULT_AUTH_TIMEOUT;

	/**
	 * 保存SET SQL_SELECT_LIMIT的值, default 解析为-1.
	 */
	private volatile int sqlSelectLimit = -1;
	private volatile boolean txReadonly;
	private volatile int txIsolation;
	private volatile boolean autocommit;
	private volatile boolean preAcStates; //上一个ac状态,默认为true
	private volatile boolean txInterrupted;
	private volatile String txInterrputMsg = "";
	private long lastInsertId;
	private NonBlockingSession session;
	/**
	 * 标志是否执行了lock tables语句，并处于lock状态
	 */
	private volatile boolean isLocked = false;
	private Queue<SqlEntry> executeSqlQueue;
	private SqlExecuteStageListener listener;

	public ServerConnection(NetworkChannel channel)
			throws IOException {
		super(channel);
		this.txInterrupted = false;
		this.autocommit = true;
		this.preAcStates = true;
		this.txReadonly = false;
		this.executeSqlQueue = new LinkedBlockingQueue<>();
		this.listener = new DefaultSqlExecuteStageListener(this);
	}

	@Override
	public boolean isIdleTimeout() {
		if (isAuthenticated) {
			return super.isIdleTimeout();
		} else {
			return TimeUtil.currentTimeMillis() > Math.max(lastWriteTime, lastReadTime) + this.authTimeout;
		}
	}

	public long getAuthTimeout() {
		return authTimeout;
	}

	public void setAuthTimeout(long authTimeout) {
		this.authTimeout = authTimeout;
	}

	public int getTxIsolation() {
		return txIsolation;
	}

	public void setTxIsolation(int txIsolation) {
		this.txIsolation = txIsolation;
	}

	public boolean isAutocommit() {
		return autocommit;
	}

	public void setAutocommit(boolean autocommit) {
		this.autocommit = autocommit;
	}

	public boolean isTxReadonly() {
		return txReadonly;
	}

	public void setTxReadonly(boolean txReadonly) {
		this.txReadonly = txReadonly;
	}

	public int getSqlSelectLimit() {
		return sqlSelectLimit;
	}

	public void setSqlSelectLimit(int sqlSelectLimit) {
		this.sqlSelectLimit = sqlSelectLimit;
	}

	public long getLastInsertId() {
		return lastInsertId;
	}

	public void setLastInsertId(long lastInsertId) {
		this.lastInsertId = lastInsertId;
	}

	/**
	 * 设置是否需要中断当前事务
	 */
	public void setTxInterrupt(String txInterrputMsg) {
		if (!autocommit && !txInterrupted) {
			txInterrupted = true;
			this.txInterrputMsg = txInterrputMsg;
		}
	}

	/**
	 * 清空食事务中断
	 */
	public void clearTxInterrupt() {
		if (!autocommit && txInterrupted) {
			txInterrupted = false;
			this.txInterrputMsg = "";
		}
	}

	public boolean isTxInterrupted() {
		return txInterrupted;
	}

	public NonBlockingSession getSession2() {
		return session;
	}

	public void setSession2(NonBlockingSession session2) {
		this.session = session2;
	}

	public boolean isLocked() {
		return isLocked;
	}

	public void setLocked(boolean isLocked) {
		this.isLocked = isLocked;
	}

	@Override
	public void ping() {
		Ping.response(this);
	}

	@Override
	public void heartbeat(byte[] data) {
		Heartbeat.response(this, data);
	}

	public void execute(String sql, int type) {
		//连接状态检查
		if (this.isClosed()) {
			LOGGER.warn("ignore execute ,server connection is closed " + this);
			return;
		}
		// 事务状态检查 (zrx 关闭)
		// 在事务中执行时如果出现了 SQL 执行异常，数据访问代理为了数据安全，会强制将事务状态设置为中断，需要显式调用 rollback 后才能继续执行其他 SQL。
		//线上环境中有的直接捕获了sql异常，没有回滚，由于是查询，所以忽略
		/*if (txInterrupted) {
			writeErrMessage(ErrorCode.ER_YES,
					"Transaction error, need to rollback." + txInterrputMsg);
			return;
		}*/

		// 检查当前使用的DB
		String db = this.schema;
		boolean isDefault = true;
		if (db == null) {
			db = SchemaUtil.detectDefaultDb(sql, type);
			if (db == null) {
				db = MycatServer.getInstance().getConfig().getUsers().get(user).getDefaultSchema();
				if (db == null) {
					writeErrMessage(ErrorCode.ERR_BAD_LOGICDB,
							"No MyCAT Database selected");
					return;
				}
			}
			isDefault = false;
		}

		// 兼容PhpAdmin's, 支持对MySQL元数据的模拟返回
		//// TODO: 2016/5/20 支持更多information_schema特性
//		if (ServerParse.SELECT == type
//				&& db.equalsIgnoreCase("information_schema") ) {
//			MysqlInformationSchemaHandler.handle(sql, this);
//			return;
//		}

		if (ServerParse.SELECT == type
				&& sql.contains("mysql")
				&& sql.contains("proc")) {

			SchemaUtil.SchemaInfo schemaInfo = SchemaUtil.parseSchema(sql);
			if (schemaInfo != null
					&& "mysql".equalsIgnoreCase(schemaInfo.schema)
					&& "proc".equalsIgnoreCase(schemaInfo.table)) {

				// 兼容MySQLWorkbench
				MysqlProcHandler.handle(sql, this);
				return;
			}
		}

		SchemaConfig schema = MycatServer.getInstance().getConfig().getSchemas().get(db);
		if (schema == null) {
			writeErrMessage(ErrorCode.ERR_BAD_LOGICDB,
					"Unknown MyCAT Database '" + db + "'");
			return;
		}

		//fix navicat   SELECT STATE AS `State`, ROUND(SUM(DURATION),7) AS `Duration`, CONCAT(ROUND(SUM(DURATION)/*100,3), '%') AS `Percentage` FROM INFORMATION_SCHEMA.PROFILING WHERE QUERY_ID= GROUP BY STATE ORDER BY SEQ
		if (ServerParse.SELECT == type && sql.contains(" INFORMATION_SCHEMA.PROFILING ") && sql.contains("CONCAT(ROUND(SUM(DURATION)/")) {
			InformationSchemaProfiling.response(this);
			return;
		}

		//fix sqlyog select state, round(sum(duration),5) as `duration (summed) in sec` from information_schema.profiling where query_id = 0 group by state order by `duration (summed) in sec` desc
		if (ServerParse.SELECT == type && sql.contains(" information_schema.profiling ") && sql.contains("duration (summed) in sec")) {
			InformationSchemaProfilingSqlyog.response(this);
			return;
		}
		/* 当已经设置默认schema时，可以通过在sql中指定其它schema的方式执行
		 * 相关sql，已经在mysql客户端中验证。
		 * 所以在此处增加关于sql中指定Schema方式的支持。
		 */
		if (isDefault && schema.isCheckSQLSchema() && isNormalSql(type)) {
			SchemaUtil.SchemaInfo schemaInfo = SchemaUtil.parseSchema(sql);
			if (schemaInfo != null && schemaInfo.schema != null && !schemaInfo.schema.equals(db)) {
				SchemaConfig schemaConfig = MycatServer.getInstance().getConfig().getSchemas().get(schemaInfo.schema);
				if (schemaConfig != null) {
					schema = schemaConfig;
				}
			}
		}

		routeEndExecuteSQL(sql, type, schema);

	}

	private boolean isNormalSql(int type) {
		return ServerParse.SELECT == type || ServerParse.INSERT == type || ServerParse.UPDATE == type || ServerParse.DELETE == type || ServerParse.DDL == type;
	}

	public RouteResultset routeSQL(String sql, int type) {

		// 检查当前使用的DB
		String db = this.schema;
		if (db == null) {
			db = SchemaUtil.detectDefaultDb(sql, type);
			if (db == null) {
				db = MycatServer.getInstance().getConfig().getUsers().get(user).getDefaultSchema();
				if (db == null) {
					writeErrMessage(ErrorCode.ERR_BAD_LOGICDB,
							"No MyCAT Database selected");
					return null;
				}
			}

		}
		SchemaConfig schema = MycatServer.getInstance().getConfig()
				.getSchemas().get(db);
		if (schema == null) {
			writeErrMessage(ErrorCode.ERR_BAD_LOGICDB,
					"Unknown MyCAT Database '" + db + "'");
			return null;
		}

		// 路由计算
		RouteResultset rrs = null;
		try {
			rrs = MycatServer
					.getInstance()
					.getRouterservice()
					.route(MycatServer.getInstance().getConfig().getSystem(),
							schema, type, sql, this.charset, this);

		} catch (Exception e) {
			StringBuilder s = new StringBuilder();
			LOGGER.warn(s.append(this).append(sql).toString() + " err:" + e.toString(), e);
			String msg = e.getMessage();
			writeErrMessage(ErrorCode.ER_PARSE_ERROR, msg == null ? e.getClass().getSimpleName() : msg);
			return null;
		}
		return rrs;
	}


	public void routeEndExecuteSQL(String sql, final int type, final SchemaConfig schema) {
		// 路由计算
		RouteResultset rrs = null;
		try {
			boolean sqlPass = !sql.contains("information_schema") && (type == ServerParse.DELETE || type == ServerParse.INSERT || type == ServerParse.UPDATE || type == ServerParse.SELECT);
			//zrx 如果是增删查改
			if (sqlPass) {
				// zrx 路由之前修改sql
				//zrx 获取是否开启加密
				boolean encrypt = YesOrNo.YES.getCode().equals(MycatServer.getInstance().getConfig().getSystem().getEncrypt());
				String schemaName = schema.getName();
				String[] nodes = schema.getAllDataNodes().toArray(new String[]{});
				//获取当前的数据库
				MycatConfig conf = MycatServer.getInstance().getConfig();
				Map<String, EncryptServer> encryptConfigMap = conf.getEncryptConfigMap();
				boolean passSchema = encryptConfigMap.containsKey(schemaName);
				boolean passDataSource = false;
				String dataSource = null;
				if (passSchema) {
					//如果是同一个 schema 下的，数据库结构都是一样的
					for (String node : nodes) {
						dataSource = conf.getDataNodes().get(node).getDatabase();
						if (encryptConfigMap.get(schemaName).getEncryptDataSourceMap().containsKey(dataSource)) {
							passDataSource = true;
						}
					}
				}
				boolean shouldEncrypt = encrypt && passSchema && passDataSource;
				//zrx 处理加密
				if (shouldEncrypt) {
					EncryptDataSource encryptDataSource = encryptConfigMap.get(schemaName).getEncryptDataSourceMap().get(dataSource);
					//获取需要被加密的表和字段
					Map<String, Set<String>> encryptTableColMap = encryptDataSource.getEncryptTableColMap();
					//根据sql类型解析sql
					try {
						//解析sql
						Statement statement = CCJSqlParserUtil.parse(sql);
						if (ServerParse.UPDATE == type) {
							Update updateStatement = (Update) statement;
							Table table = updateStatement.getTable();
							String updateTable = table.getName().toLowerCase().replaceAll("`", "");
							//别名
							Alias alias = table.getAlias();
							if (encryptTableColMap.containsKey(updateTable)) {
								//获取需要加密的列
								Set<String> columns = encryptTableColMap.get(updateTable);
								//获取sql中的列
								List<Column> sqlColumns = updateStatement.getColumns();
								List<Expression> expressions = updateStatement.getExpressions();
								if (sqlColumns != null && expressions != null) {
									//遍历更新的列，查看是否需要加密
									replaceSqlValue(columns, sqlColumns, expressions);
									//替换sql
									sql = updateStatement.toString();
								}
								//获取 where 条件
								Expression where = updateStatement.getWhere();
								if (where != null) {
									encryptParser(encryptTableColMap, where, columns, alias == null ? null : alias.getName().toLowerCase().replaceAll("`", ""), updateTable);
									sql = updateStatement.toString();
								}
							}
							Select select = updateStatement.getSelect();
							if (select != null) {
								SelectBody selectBody = select.getSelectBody();
								encryptParserSelect(selectBody, encryptTableColMap);
								//替换sql
								sql = updateStatement.toString();
							}
						} else if (ServerParse.INSERT == type) {
							Insert insertStatement = (Insert) statement;
							String inerstTable = insertStatement.getTable().getName().toLowerCase().replaceAll("`", "");
							if (encryptTableColMap.containsKey(inerstTable)) {
								//获取需要加密的列
								Set<String> columns = encryptTableColMap.get(inerstTable);
								//如果insert语句中包含需要加密的表，获取插入的列和值
								List<Column> sqlColumns = insertStatement.getColumns();
								ExpressionList itemsList = (ExpressionList) insertStatement.getItemsList();
								if (sqlColumns != null && itemsList != null) {
									//遍历插入的列，查看是否有需要加密的
									List<Expression> sqlInsertValues = itemsList.getExpressions();
									replaceSqlValue(columns, sqlColumns, sqlInsertValues);
									//替换sql
									sql = insertStatement.toString();
								}
							}
							Select select = insertStatement.getSelect();
							if (select != null) {
								SelectBody selectBody = select.getSelectBody();
								encryptParserSelect(selectBody, encryptTableColMap);
								//替换sql
								sql = insertStatement.toString();
							}
						} else if (ServerParse.DELETE == type) {
							Delete deleteStatement = (Delete) statement;
							Table table = deleteStatement.getTable();
							String deleteTable = table.getName().toLowerCase().replaceAll("`", "");
							//别名
							Alias alias = table.getAlias();
							if (encryptTableColMap.containsKey(deleteTable)) {
								//获取需要加密的列
								Set<String> columns = encryptTableColMap.get(deleteTable);
								Expression where = deleteStatement.getWhere();
								if (where != null) {
									encryptParser(encryptTableColMap, where, columns, alias == null ? null : alias.getName().toLowerCase().replaceAll("`", ""), deleteTable);
									sql = deleteStatement.toString();
								}
							}
						} else {
							Select selectStatement = (Select) statement;
							SelectBody body = selectStatement.getSelectBody();
							encryptParserSelect(body, encryptTableColMap);
							sql = selectStatement.toString();
						}
					} catch (Exception e) {
						LOGGER.error("encrypt sql parser error:", e);
						if (!sql.contains("convert(no,SIGNED)")) {
							writeErrMessage(ErrorCode.ERR_HANDLE_DATA, "encrypt sql parser error:" + e.toString());
							return;
						}
					}
				}
			}
			//生成 rrs 路由对象
			rrs = MycatServer
					.getInstance()
					.getRouterservice()
					.route(MycatServer.getInstance().getConfig().getSystem(),
							schema, type, sql, this.charset, this);

		} catch (Exception e) {
			StringBuilder s = new StringBuilder();
			LOGGER.warn(s.append(this).append(sql).toString() + " err:" + e.toString(), e);
			String msg = e.getMessage();
			writeErrMessage(ErrorCode.ER_PARSE_ERROR, msg == null ? e.getClass().getSimpleName() : msg);
			return;
		}
		if (rrs != null) {
			// #支持mariadb驱动useBatchMultiSend=true,连续接收到的sql先放入队列，等待前面处理完成后再继续处理。
			// 参考https://mariadb.com/kb/en/option-batchmultisend-description/
			boolean executeNow = false;
			synchronized (this.executeSqlQueue) {
				executeNow = this.executeSqlQueue.isEmpty();
				this.executeSqlQueue.add(new SqlEntry(sql, type, rrs));
				if (LOGGER.isDebugEnabled()) {
					LOGGER.debug("add queue,executeSqlQueue size {}", executeSqlQueue.size());
				}
			}

			if (executeNow) {
				this.executeSqlId++;
				session.execute(rrs, rrs.isSelectForUpdate() ? ServerParse.UPDATE : type);
			}
		}
	}

	/**
	 * 替换sql中的值
	 *
	 * @param columns
	 * @param sqlColumns
	 * @param expressions
	 */
	private void replaceSqlValue(Set<String> columns, List<Column> sqlColumns, List<Expression> expressions) {
		for (int i = 0; i < sqlColumns.size(); i++) {
			String columnName = sqlColumns.get(i).getColumnName().toLowerCase().replaceAll("`", "");
			if (columns.contains(columnName)) {
				//获取需要被替换的值
				Expression expression = expressions.get(i);
				if (expression instanceof StringValue) {
					StringValue stringValue = (StringValue) expression;
					if (StringUtils.isNotEmpty(stringValue.getValue())) {
						stringValue.setValue(EncryptHelper.hexWithPre(EncryptHelper.encode(stringValue.getValue().getBytes(StandardCharsets.UTF_8))));
					}
				}
			}
		}
	}

	/**
	 * 针对查询中的查询条件进行加密
	 *
	 * @param body
	 */
	private void encryptParserSelect(SelectBody body, Map<String, Set<String>> encryptTableColMap) {
		try {
			if (body instanceof PlainSelect) {
				PlainSelect select = (PlainSelect) body;
				FromItem fromItem = select.getFromItem();
				if (fromItem instanceof Table) {
					Expression where = select.getWhere();
					//where条件不为空，才需要加密
					if (where != null) {
						//获取sql中的表和别名
						Map<String, Set<String>> tableAlias = MySelectVisitor.getTables(select.toString());
						//遍历表，判断是否存在需要加密的表
						encryptWhere(encryptTableColMap, where, tableAlias);
					}
				} else if (fromItem instanceof SubSelect) {
					SubSelect subSelect = (SubSelect) fromItem;
					SelectBody selectBody = subSelect.getSelectBody();
					//获取子查询的别名
					Alias subSelectAlias = subSelect.getAlias();
					Expression where = select.getWhere();
					if (where != null && subSelectAlias != null) {
						if (selectBody instanceof SetOperationList) {
							// 如果子查询内部是 union
							SetOperationList operationList = (SetOperationList) selectBody;
							List<SelectBody> selects = operationList.getSelects();
							Map<String, Set<String>> tableAlias = new LinkedHashMap<>(6);
							for (SelectBody stb : selects) {
								Map<String, Set<String>> tables = MySelectVisitor.getTables(stb.toString());
								for (Map.Entry<String, Set<String>> entry : tables.entrySet()) {
									Set<String> set = new HashSet<>(1);
									set.add(subSelectAlias.getName());
									tableAlias.put(entry.getKey(), set);
								}
							}
							//遍历表，判断是否存在子查询的别名需要加密的字段
							encryptWhere(encryptTableColMap, where, tableAlias);
						} else if (selectBody instanceof PlainSelect) {
							Map<String, Set<String>> tableAlias = MySelectVisitor.getTables(selectBody.toString());
							//如果外层查询条件和子查询的别名都不为空，获取子查询查的表
							for (Map.Entry<String, Set<String>> entry : tableAlias.entrySet()) {
								//别名都设置为子查询的别名
								Set<String> set = new HashSet<>(1);
								set.add(subSelectAlias.getName());
								entry.setValue(set);
							}
							//遍历表，判断是否存在子查询的别名需要加密的字段
							encryptWhere(encryptTableColMap, where, tableAlias);
						}
					}
					//递归调用
					encryptParserSelect(selectBody, encryptTableColMap);
				}
				List<Join> joins = select.getJoins();
				if (joins != null) {
					for (Join join : joins) {
						FromItem rightItem = join.getRightItem();
						if (rightItem instanceof SubSelect) {
							SubSelect subSelect = (SubSelect) rightItem;
							SelectBody selectBody = subSelect.getSelectBody();
							//递归调用
							encryptParserSelect(selectBody, encryptTableColMap);
						}
					}
				}
			} else if (body instanceof SetOperationList) {
				SetOperationList operationList = (SetOperationList) body;
				for (SelectBody selectBody : operationList.getSelects()) {
					encryptParserSelect(selectBody, encryptTableColMap);
				}
			}

		} catch (Exception e) {
			LOGGER.error("encrypt subselect sql parser error:", e);
			writeErrMessage(ErrorCode.ERR_HANDLE_DATA, "encrypt subselect sql parser error:" + e.toString());
		}

	}

	/**
	 * 对where条件中需要加密的列加密
	 *
	 * @param encryptTableColMap
	 * @param where
	 * @param tableAlias
	 */
	private void encryptWhere(Map<String, Set<String>> encryptTableColMap, Expression where, Map<String, Set<String>> tableAlias) {
		for (Map.Entry<String, Set<String>> entry : tableAlias.entrySet()) {
			if (encryptTableColMap.containsKey(entry.getKey())) {
				//获取需要加密的列
				Set<String> columns = encryptTableColMap.get(entry.getKey());
				//如果表需要加密，对查询条件进行加密
				for (String alias : entry.getValue()) {
					encryptParser(encryptTableColMap, where, columns, alias, entry.getKey());
				}
			}
		}
	}

	/**
	 * 对查询条件加密
	 *
	 * @param expression
	 * @param columns
	 * @param tableAlias
	 * @param tableName
	 */
	private void encryptParser(Map<String, Set<String>> encryptTableColMap, Expression expression, Set<String> columns, String tableAlias, String tableName) {
		if (expression instanceof EqualsTo) {
			parserBinaryExpression(expression, columns, tableAlias, tableName);
		} else if (expression instanceof NotEqualsTo) {
			parserBinaryExpression(expression, columns, tableAlias, tableName);
		} else if (expression instanceof LikeExpression) {
			parserBinaryExpression(expression, columns, tableAlias, tableName);
		} else if (expression instanceof InExpression) {
			parserInExpression(encryptTableColMap, expression, columns, tableAlias, tableName);
		} else if (expression instanceof Parenthesis) {
			Parenthesis parenthesis = (Parenthesis) expression;
			encryptParser(encryptTableColMap, parenthesis.getExpression(), columns, tableAlias, tableName);
		} else if (expression instanceof BinaryExpression) {
			BinaryExpression binaryExpression = (BinaryExpression) expression;
			encryptParser(encryptTableColMap, binaryExpression.getLeftExpression(), columns, tableAlias, tableName);
			encryptParser(encryptTableColMap, binaryExpression.getRightExpression(), columns, tableAlias, tableName);
		}
	}

	/**
	 * 解析in关键字左边的条件
	 *
	 * @param expression
	 */
	private void parserInExpression(Map<String, Set<String>> encryptTableColMap, Expression expression, Set<String> columns, String tableAlias, String tableName) {
		InExpression inExpression = (InExpression) expression;
		Expression leftExpression = inExpression.getLeftExpression();
		if (leftExpression instanceof Column) {
			Column column = (Column) leftExpression;
			String columnTable = column.getTable() == null ? null : column.getTable().getName().toLowerCase().replaceAll("`", "");
			String columnName = column.getColumnName().toLowerCase().replaceAll("`", "");
			//包含，说明需要加密，获取右边的表达式
			ItemsList rightItemsList = inExpression.getRightItemsList();
			if (columns.contains(columnName)) {
				if (isNotOwn(tableAlias, tableName, columnTable)) {
					return;
				}
				if (rightItemsList instanceof ExpressionList) {
					ExpressionList expressionList = (ExpressionList) rightItemsList;
					List<Expression> expressions = expressionList.getExpressions();
					for (Expression ex : expressions) {
						if (ex instanceof StringValue) {
							StringValue value = (StringValue) ex;
							if (StringUtils.isNotEmpty(value.getValue())) {
								value.setValue(EncryptHelper.hexWithPre(EncryptHelper.encode(value.getValue().getBytes(StandardCharsets.UTF_8))));
							}
						}
					}
				}
			}
			if (rightItemsList instanceof SubSelect) {
				//不是自己的别名，不解析
				if (isNotOwn(tableAlias, tableName, columnTable)) {
					return;
				}
				//解析子查询
				SubSelect subSelect = (SubSelect) rightItemsList;
				encryptParserSelect(subSelect.getSelectBody(), encryptTableColMap);
			}
		}
	}


	private void parserBinaryExpression(Expression expression, Set<String> columns, String tableAlias, String tableName) {
		BinaryExpression binartex = (BinaryExpression) expression;
		Expression leftExpression = binartex.getLeftExpression();
		if (leftExpression instanceof Column) {
			Column column = (Column) leftExpression;
			String columnTable = column.getTable() == null ? null : column.getTable().getName().toLowerCase().replaceAll("`", "");
			String columnName = column.getColumnName().toLowerCase().replaceAll("`", "");
			if (columns.contains(columnName)) {
				if (isNotOwn(tableAlias, tableName, columnTable)) {
					return;
				}
				//如果是需要加密的列，获取右边
				Expression rightExpression = binartex.getRightExpression();
				if ((binartex instanceof EqualsTo || binartex instanceof NotEqualsTo) && rightExpression instanceof StringValue) {
					StringValue value = (StringValue) rightExpression;
					//如果是相等
					if (StringUtils.isNotEmpty(value.getValue())) {
						value.setValue(EncryptHelper.hexWithPre(EncryptHelper.encode(value.getValue().getBytes(StandardCharsets.UTF_8))));
					}
				} else if (binartex instanceof LikeExpression && rightExpression instanceof StringValue) {
					//如果是like
					StringValue stringValue = (StringValue) rightExpression;
					String strVal = stringValue.getValue();
					String start = strVal.startsWith("%") ? "%" : EncryptHelper.PRE_ENCRYPT;
					String end = strVal.endsWith("%") ? "%" : "";
					String value = strVal.replaceAll("^%+|%+$", "");
					if (StringUtils.isNotEmpty(value)) {
						stringValue.setValue(start + EncryptHelper.hex(EncryptHelper.encode(value.getBytes(StandardCharsets.UTF_8))) + end);
					}
				}
			}
		}
	}

	/**
	 * 判断是否是自己的字段
	 *
	 * @param tableAlias
	 * @param tableName
	 * @param columnTable
	 * @return
	 */
	private boolean isNotOwn(String tableAlias, String tableName, String columnTable) {
		//列别名不为空 并且 （表别名不为空并且表别名不等于列别名 或者 表别名为空但是列别名不等于表名）
		return columnTable != null && ((tableAlias != null && !tableAlias.equals(columnTable)) || (tableAlias == null && !tableName.equals(columnTable)));
	}

	/**
	 * 提交事务
	 */
	public void commit() {
		if (txInterrupted) {
			LOGGER.warn("receive commit ,but found err message in Transaction {}", this);
			this.rollback();
//			writeErrMessage(ErrorCode.ER_YES,
//					"Transaction error, need to rollback.");
		} else {
			session.commit();
		}
	}

	/**
	 * 回滚事务
	 */
	public void rollback() {
		// 状态检查
		if (txInterrupted) {
			txInterrupted = false;
		}

		// 执行回滚
		session.rollback();
	}

	/**
	 * 执行lock tables语句方法
	 *
	 * @param sql
	 */
	public void lockTable(String sql) {
		// 事务中不允许执行lock table语句
		if (!autocommit) {
			writeErrMessage(ErrorCode.ER_YES, "can't lock table in transaction!");
			return;
		}
		// 已经执行了lock table且未执行unlock table之前的连接不能再次执行lock table命令
		if (isLocked) {
			writeErrMessage(ErrorCode.ER_YES, "can't lock multi-table");
			return;
		}
		RouteResultset rrs = routeSQL(sql, ServerParse.LOCK);
		if (rrs != null) {
			session.lockTable(rrs);
		}
	}

	/**
	 * 执行unlock tables语句方法
	 *
	 * @param sql
	 */
	public void unLockTable(String sql) {
		sql = sql.replaceAll("\n", " ").replaceAll("\t", " ");
		String[] words = SplitUtil.split(sql, ' ', true);
		if (words.length == 2 && ("table".equalsIgnoreCase(words[1]) || "tables".equalsIgnoreCase(words[1]))) {
			isLocked = false;
			session.unLockTable(sql);
		} else {
			writeErrMessage(ErrorCode.ER_UNKNOWN_COM_ERROR, "Unknown command");
		}

	}

	/**
	 * 撤销执行中的语句
	 *
	 * @param sponsor 发起者为null表示是自己
	 */
	public void cancel(final FrontendConnection sponsor) {
		processor.getExecutor().execute(new Runnable() {
			@Override
			public void run() {
				session.cancel(sponsor);
			}
		});
	}

	@Override
	public void close(String reason) {
		super.close(reason);
		session.terminate();
		if (getLoadDataInfileHandler() != null) {
			getLoadDataInfileHandler().clear();
		}
	}

	/**
	 * add huangyiming 检测字符串中某字符串出现次数
	 *
	 * @param srcText
	 * @param findText
	 * @return
	 */
	public static int appearNumber(String srcText, String findText) {
		int count = 0;
		Pattern p = Pattern.compile(findText);
		Matcher m = p.matcher(srcText);
		while (m.find()) {
			count++;
		}
		return count;
	}

	@Override
	public String toString() {

		return "ServerConnection [id=" + id + ", schema=" + schema + ", host="
				+ host + ", user=" + user + ",txIsolation=" + txIsolation
				+ ", autocommit=" + autocommit + ", schema=" + schema + ", executeSql=" + executeSql + "]" +
				this.getSession2();

	}

	public boolean isPreAcStates() {
		return preAcStates;
	}

	public void setPreAcStates(boolean preAcStates) {
		this.preAcStates = preAcStates;
	}

	public SqlExecuteStageListener getListener() {
		return listener;
	}

	public void setListener(SqlExecuteStageListener listener) {
		this.listener = listener;
	}

	@Override
	public void checkQueueFlow() {
		RouteResultset rrs = session.getRrs();
		if (rrs != null && rrs.getNodes().length > 1 && session.getRrs().needMerge()) {
			// 多节点合并结果集语句需要拉取所有数据，无法流控
			return;
		} else {
			// 非合并结果集语句进行流量控制检查。
			flowController.check(session.getTargetMap());
		}
	}

	@Override
	public void resetConnection() {
		// 1 简单点直接关闭后端连接。若按照mysql官方的提交事务或回滚事务，mycat都会回包给应用，引发包乱序。
		session.closeAndClearResources("receive com_reset_connection");

		// 2 重置用户变量
		this.txInterrupted = false;
		this.autocommit = true;
		this.preAcStates = true;
		this.txReadonly = false;
		this.lastInsertId = 0;

		super.resetConnection();
	}

	/**
	 * sql执行完成后回调函数
	 */
	public void onEventSqlCompleted() {
		SqlEntry sqlEntry = null;
		synchronized (this.executeSqlQueue) {
			this.executeSqlQueue.poll();// 弹出已经执行成功的
			sqlEntry = this.executeSqlQueue.peek();
			if (LOGGER.isDebugEnabled()) {
				LOGGER.debug("poll queue,executeSqlQueue size {}", this.executeSqlQueue.size());
			}
		}
		if (sqlEntry != null) {
			this.executeSqlId++;
			session.execute(sqlEntry.rrs, sqlEntry.rrs.isSelectForUpdate() ? ServerParse.UPDATE : sqlEntry.type);
		}
	}

	private class SqlEntry {
		public String sql;
		public int type;
		public RouteResultset rrs;

		public SqlEntry(String sql, int type, RouteResultset rrs) {
			this.sql = sql;
			this.type = type;
			this.rrs = rrs;
		}
	}

}
