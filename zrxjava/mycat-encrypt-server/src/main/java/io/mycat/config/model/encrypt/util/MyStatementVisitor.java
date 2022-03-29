package io.mycat.config.model.encrypt.util;

import net.sf.jsqlparser.statement.Block;
import net.sf.jsqlparser.statement.Commit;
import net.sf.jsqlparser.statement.CreateFunctionalStatement;
import net.sf.jsqlparser.statement.DeclareStatement;
import net.sf.jsqlparser.statement.DescribeStatement;
import net.sf.jsqlparser.statement.ExplainStatement;
import net.sf.jsqlparser.statement.SetStatement;
import net.sf.jsqlparser.statement.ShowColumnsStatement;
import net.sf.jsqlparser.statement.ShowStatement;
import net.sf.jsqlparser.statement.StatementVisitor;
import net.sf.jsqlparser.statement.Statements;
import net.sf.jsqlparser.statement.UseStatement;
import net.sf.jsqlparser.statement.alter.Alter;
import net.sf.jsqlparser.statement.alter.sequence.AlterSequence;
import net.sf.jsqlparser.statement.comment.Comment;
import net.sf.jsqlparser.statement.create.index.CreateIndex;
import net.sf.jsqlparser.statement.create.schema.CreateSchema;
import net.sf.jsqlparser.statement.create.sequence.CreateSequence;
import net.sf.jsqlparser.statement.create.synonym.CreateSynonym;
import net.sf.jsqlparser.statement.create.table.CreateTable;
import net.sf.jsqlparser.statement.create.view.AlterView;
import net.sf.jsqlparser.statement.create.view.CreateView;
import net.sf.jsqlparser.statement.delete.Delete;
import net.sf.jsqlparser.statement.drop.Drop;
import net.sf.jsqlparser.statement.execute.Execute;
import net.sf.jsqlparser.statement.grant.Grant;
import net.sf.jsqlparser.statement.insert.Insert;
import net.sf.jsqlparser.statement.merge.Merge;
import net.sf.jsqlparser.statement.replace.Replace;
import net.sf.jsqlparser.statement.select.Select;
import net.sf.jsqlparser.statement.show.ShowTablesStatement;
import net.sf.jsqlparser.statement.truncate.Truncate;
import net.sf.jsqlparser.statement.update.Update;
import net.sf.jsqlparser.statement.upsert.Upsert;
import net.sf.jsqlparser.statement.values.ValuesStatement;

import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class MyStatementVisitor implements StatementVisitor {

	/**
	 * 表名和别名 一个表可能有多个别名
	 */
	private Map<String, Set<String>> tableAliasMap;

	Map<String, Set<String>> getTableAliasMap() {
		return tableAliasMap;
	}

	MyStatementVisitor() {
		this.tableAliasMap = new LinkedHashMap<>(6);
	}

	//访问select语句
	@Override
	public void visit(Select select) {
		//访问select的各个组成部分
		select.getSelectBody().accept(new MySelectVisitor(tableAliasMap));
	}

	@Override
	public void visit(Upsert upsert) {

	}

	@Override
	public void visit(UseStatement useStatement) {

	}

	@Override
	public void visit(Block block) {

	}

	@Override
	public void visit(ValuesStatement valuesStatement) {

	}

	@Override
	public void visit(DescribeStatement describeStatement) {

	}

	@Override
	public void visit(ExplainStatement explainStatement) {

	}

	@Override
	public void visit(ShowStatement showStatement) {

	}

	@Override
	public void visit(DeclareStatement declareStatement) {

	}

	@Override
	public void visit(Grant grant) {

	}

	@Override
	public void visit(CreateSequence createSequence) {

	}

	@Override
	public void visit(AlterSequence alterSequence) {

	}

	@Override
	public void visit(CreateFunctionalStatement createFunctionalStatement) {

	}

	@Override
	public void visit(CreateSynonym createSynonym) {

	}

	@Override
	public void visit(Comment comment) {

	}

	@Override
	public void visit(Commit commit) {

	}

	//访问delete语句
	@Override
	public void visit(Delete delete) {
	}

	//访问update语句
	@Override
	public void visit(Update update) {
	}

	//访问insert语句
	@Override
	public void visit(Insert insert) {
	}

	@Override
	public void visit(Replace replace) {
	}

	//访问drop，忽略
	@Override
	public void visit(Drop drop) {
	}

	//访问truncate，忽略
	@Override
	public void visit(Truncate truncate) {
	}

	@Override
	public void visit(CreateIndex createIndex) {

	}

	@Override
	public void visit(CreateSchema createSchema) {

	}

	//访问create，忽略
	@Override
	public void visit(CreateTable arg0) {
	}

	@Override
	public void visit(CreateView createView) {

	}

	@Override
	public void visit(AlterView alterView) {

	}

	@Override
	public void visit(Alter alter) {

	}

	@Override
	public void visit(Statements statements) {

	}

	@Override
	public void visit(Execute execute) {

	}

	@Override
	public void visit(SetStatement setStatement) {

	}

	@Override
	public void visit(ShowColumnsStatement showColumnsStatement) {

	}

	@Override
	public void visit(ShowTablesStatement showTablesStatement) {

	}

	@Override
	public void visit(Merge merge) {

	}
}
