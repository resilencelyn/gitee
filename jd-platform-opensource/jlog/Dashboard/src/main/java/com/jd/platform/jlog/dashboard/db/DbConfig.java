package com.jd.platform.jlog.dashboard.db;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import ru.yandex.clickhouse.ClickHouseConnection;
import ru.yandex.clickhouse.ClickHouseDataSource;
import ru.yandex.clickhouse.settings.ClickHouseProperties;

import java.sql.SQLException;

/**
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-23
 */
@Configuration
public class DbConfig {
    @Value("${clickhouse.url}")
    private String address;
    @Value("${clickhouse.username}")
    private String username;
    @Value("${clickhouse.password}")
    private String password;
    @Value("${clickhouse.db}")
    private String db;

    @Value("${clickhouse.poolSize}")
    private String poolSize;

    @Bean
    public ConnectionPool connectionPool() {
        ClickHouseConnection conn;
        ClickHouseProperties properties = new ClickHouseProperties();
        properties.setUser(username);
        properties.setPassword(password);
        properties.setDatabase(db);
        properties.setSocketTimeout(60000);
        ClickHouseDataSource clickHouseDataSource = new ClickHouseDataSource(address, properties);

        ConnectionPool connectionPool = new ConnectionPool();
        try {
            //创建指定数量的数据库连接
            for (int i = 0; i < Integer.valueOf(poolSize); i++) {
                conn = clickHouseDataSource.getConnection();
                connectionPool.addConnection(conn);
            }

            return connectionPool;
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return null;
    }

}
