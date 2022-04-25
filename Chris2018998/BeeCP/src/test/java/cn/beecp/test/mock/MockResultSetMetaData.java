/*
 * Copyright(C) Chris2018998
 * Contact:Chris2018998@tom.com
 *
 * Licensed under GNU Lesser General Public License v2.1
 */
package cn.beecp.test.mock;

import java.sql.ResultSetMetaData;

/**
 * @author Chris.Liao
 * @version 1.0
 */
public class MockResultSetMetaData extends MockBase implements ResultSetMetaData {
    public int getColumnCount() {
        return 1;
    }

    public boolean isAutoIncrement(int column) {
        return true;
    }

    public boolean isCaseSensitive(int column) {
        return true;
    }

    public boolean isSearchable(int column) {
        return true;
    }

    public boolean isCurrency(int column) {
        return true;
    }

    public int isNullable(int column) {
        return 1;
    }

    public boolean isSigned(int column) {
        return true;
    }

    public int getColumnDisplaySize(int column) {
        return 1;
    }

    public String getColumnLabel(int column) {
        return "";
    }

    public String getColumnName(int column) {
        return "";
    }

    public String getSchemaName(int column) {
        return "";
    }

    public int getPrecision(int column) {
        return 1;
    }

    public int getScale(int column) {
        return 1;
    }

    public String getTableName(int column) {
        return "";
    }

    public String getCatalogName(int column) {
        return "";
    }

    public int getColumnType(int column) {
        return 1;
    }

    public String getColumnTypeName(int column) {
        return "";
    }

    public boolean isReadOnly(int column) {
        return true;
    }

    public boolean isWritable(int column) {
        return true;
    }

    public boolean isDefinitelyWritable(int column) {
        return true;
    }

    public String getColumnClassName(int column) {
        return "";
    }
}
