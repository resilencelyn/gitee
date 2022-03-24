package com.jd.platform.jlog.dashboard.db;

/**
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-30
 */
public class WhereCause {
    /**
     * 数据库列名
     */
    private String key;
    /**
     * value
     */
    private Object value;
    /**
     * 大于小于等于
     */
    private DbOperator operator;

    public WhereCause() {
    }

    public WhereCause(String key, Object value, DbOperator operator) {
        this.key = key;
        this.value = value;
        this.operator = operator;
    }

    public String getKey() {
        return key;
    }

    public void setKey(String key) {
        this.key = key;
    }

    public Object getValue() {
        return value;
    }

    public void setValue(Object value) {
        this.value = value;
    }

    public DbOperator getOperator() {
        return operator;
    }

    public void setOperator(DbOperator operator) {
        this.operator = operator;
    }
}
