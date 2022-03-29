package io.mycat.config.model.encrypt.util;

import net.sf.jsqlparser.JSQLParserException;
import net.sf.jsqlparser.parser.CCJSqlParserManager;
import net.sf.jsqlparser.schema.Table;
import net.sf.jsqlparser.statement.Statement;
import net.sf.jsqlparser.statement.select.FromItem;
import net.sf.jsqlparser.statement.select.FromItemVisitor;
import net.sf.jsqlparser.statement.select.Join;
import net.sf.jsqlparser.statement.select.LateralSubSelect;
import net.sf.jsqlparser.statement.select.ParenthesisFromItem;
import net.sf.jsqlparser.statement.select.PlainSelect;
import net.sf.jsqlparser.statement.select.SelectVisitor;
import net.sf.jsqlparser.statement.select.SetOperationList;
import net.sf.jsqlparser.statement.select.SubJoin;
import net.sf.jsqlparser.statement.select.SubSelect;
import net.sf.jsqlparser.statement.select.TableFunction;
import net.sf.jsqlparser.statement.select.ValuesList;
import net.sf.jsqlparser.statement.select.WithItem;
import net.sf.jsqlparser.statement.values.ValuesStatement;

import java.io.StringReader;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class MySelectVisitor implements SelectVisitor {

	private Map<String, Set<String>> tableAliasMap;

	MySelectVisitor(Map<String, Set<String>> tableAliasMap) {
		this.tableAliasMap = tableAliasMap;
	}

	@Override
	public void visit(PlainSelect ps) {
		FromItemVisitor fromItemVisitor = new FromItemVisitor() {
			@Override
			public void visit(Table table) {
				String tableName = table.getName().toLowerCase().replaceAll("`", "");
				String alias = table.getAlias() == null ? null : table.getAlias().getName().toLowerCase().replaceAll("`", "");
				Set<String> set = tableAliasMap.get(tableName);
				if (set == null) {
					set = new HashSet<>(1);
					tableAliasMap.put(tableName, set);
					set.add(alias);
				} else {
					set.add(alias);
				}
			}

			@Override
			public void visit(SubSelect ss) {
				//ss.getSelectBody().accept(new MySelectVisitor(tableAliasMap));
			}

			@Override
			public void visit(SubJoin sj) {
				sj.getLeft().accept(this);
				sj.getJoinList().forEach(s -> {
					s.getRightItem().accept(this);
				});
			}

			@Override
			public void visit(LateralSubSelect lateralSubSelect) {
				//lateralSubSelect.accept(this);
			}

			@Override
			public void visit(ValuesList valuesList) {
			}

			@Override
			public void visit(TableFunction tableFunction) {
			}

			@Override
			public void visit(ParenthesisFromItem parenthesisFromItem) {
				parenthesisFromItem.accept(this);
				parenthesisFromItem.getFromItem().accept(this);
			}
		};
		//访问select中的from部分，这里获取到第一个表名
		FromItem fromItem = ps.getFromItem();
		if (fromItem != null) {
			fromItem.accept(fromItemVisitor);
		}
		//访问select中的join部分，这里获取到第二个表名
		List<Join> joins = ps.getJoins();
		if (joins != null) {
			for (Join join : joins) {
				join.getRightItem().accept(fromItemVisitor);
			}
		}
	}

	@Override
	public void visit(SetOperationList setOperationList) {
		//setOperationList.accept(this);
	}

	@Override
	public void visit(WithItem withItem) {
	}

	@Override
	public void visit(ValuesStatement valuesStatement) {
	}

	public static Map<String, Set<String>> getTables(String sql) throws JSQLParserException {
		CCJSqlParserManager parserManager = new CCJSqlParserManager();
		Statement stmt;
		stmt = parserManager.parse(new StringReader(sql));
		//使用visitor模式访问SQL的组成部分
		MyStatementVisitor visitor = new MyStatementVisitor();
		stmt.accept(visitor);
		return visitor.getTableAliasMap();
	}


	public static void main(String[] args) throws JSQLParserException {
		System.out.println(getTables("SELECT * FROM (SELECT p.people_id,p.name,p.card_type,p.card_code,p.phone,api.check_status,api.acid_people_info_id " +
				"FROM real_people p LEFT JOIN real_people p1 ON p.people_id=p1.people_id JOIN std_door d ON p.door_id = d.door_id LEFT JOIN std_building b ON d.building_id = b.building_id LEFT JOIN acid_people_info api ON p.card_code = api.card_code AND api.check_round=1 JOIN cmn_grid g ON p.grid_code = g.grid_code WHERE p.people_id IN " +
				"(SELECT people_id FROM (SELECT MAX(p.people_id) people_id FROM real_people p LEFT JOIN acid_people_info api ON p.card_code = api.card_code AND api.check_round=1 JOIN cmn_grid g ON p.grid_code=g.grid_code WHERE p.change_type='0' " +
				"GROUP BY p.card_code UNION ALL SELECT people_id FROM real_people p JOIN cmn_grid g ON p.grid_code=g.grid_code WHERE change_type='0')))"));
	}
}
