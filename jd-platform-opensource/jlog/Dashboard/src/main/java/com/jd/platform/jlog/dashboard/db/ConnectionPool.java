package com.jd.platform.jlog.dashboard.db;

import java.sql.Connection;
import java.util.ArrayList;
import java.util.List;

/**
 * ck连接池
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-24
 */
public class ConnectionPool {
    /**
     * 连接池
     */
    private List<Connection> connectionList = new ArrayList<>();
    private int count;

    public void addConnection(Connection connection) {
        connectionList.add(connection);
    }

    /**
     * 获取数据库连接
     */
    public Connection getConnection() {
        if (count >= connectionList.size()) {
            count = 0;
        }
        Connection connection = connectionList.get(count);
        count++;

        return connection;
    }
}
