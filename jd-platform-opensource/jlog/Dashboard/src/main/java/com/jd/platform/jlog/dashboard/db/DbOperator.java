package com.jd.platform.jlog.dashboard.db;

/**
 * 数据库条件语句符号
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-30
 */
public enum  DbOperator {
    //大于，小于，等于
    GE("1"), LE("2"), EQUEL("0");

    private String code;

    DbOperator(String code) {
        this.code = code;
    }

    public String getCode() {
        return this.code;
    }
}
