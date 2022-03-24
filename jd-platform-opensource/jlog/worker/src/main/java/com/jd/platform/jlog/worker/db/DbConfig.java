package com.jd.platform.jlog.worker.db;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;
import ru.yandex.clickhouse.ClickHouseDataSource;
import ru.yandex.clickhouse.settings.ClickHouseProperties;

import java.sql.Connection;
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
    /**
     * model几个线程去入库
     */
    @Value("${clickhouse.poolSize}")
    private String modelPoolSize;
    /**
     * 数据库节点数量(15个)
     */
    @Value("${clickhouse.nodeNumber}")
    private String nodeNumber;

    /**
     * 当前使用的哪个数据库机器
     */
    private static int INDEX = 0;
    private static int REPLICA = 0;

    public Connection getConnection() {
        ClickHouseProperties properties = new ClickHouseProperties();
        properties.setUser(username);
        properties.setPassword(password);
        properties.setDatabase(db);
        properties.setSocketTimeout(60000);
        ClickHouseDataSource clickHouseDataSource = new ClickHouseDataSource(parseHostAddress(), properties);

        try {
            return clickHouseDataSource.getConnection();
        } catch (SQLException e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * 挑一台ck机器的地址返回
     */
    private String parseHostAddress() {
        //共多少个机器
        int totalNodeNumber = Integer.valueOf(nodeNumber);

        //lf06-clickhouse-000012-clickhouse-0-0.realtime-data-platform.svc.lf06.n.jd.local
        //在0-0，0-1，1-0，1-1.。。到14-1之间循环
        String realHost = address.replace("0-0", INDEX + "-" + REPLICA % 2);

        if (REPLICA % 2 == 1) {
            INDEX++;
        }

        REPLICA++;

        if (INDEX > totalNodeNumber - 1) {
            INDEX = 0;
        }
        if (REPLICA > 1) {
            REPLICA = 0;
        }

        return realHost;
    }


}
