/*
 * Copyright(C) Chris2018998
 * Contact:Chris2018998@tom.com
 *
 * Licensed under GNU Lesser General Public License v2.1
 */
package cn.beecp.test.config;

import cn.beecp.BeeDataSourceConfig;
import cn.beecp.BeeDataSourceConfigException;
import cn.beecp.pool.PoolStaticCenter;
import cn.beecp.test.TestCase;

import java.lang.reflect.Field;
import java.net.URL;
import java.util.Map;

/**
 * @author Chris.Liao
 * @version 1.0
 */
public class PropertiesFileLoadTest extends TestCase {
    //private final String ConfigedUrl="jdbc:mysql://localhost/test";
    //private final String ConfigedDriver="com.mysql.cj.jdbc.Driver";
    private final String ConfigUrl = "jdbc:beecp://localhost/testdb";
    private final String ConfigDriver = "cn.beecp.test.mock.MockDriver";
    private final String filename = "beecp/config2.properties";
    private final BeeDataSourceConfig testConfig = new BeeDataSourceConfig();

    public void setUp() throws Throwable {
        URL url = PropertiesFileLoadTest.class.getClassLoader().getResource(filename);
        if (url == null) url = PropertiesFileLoadTest.class.getResource(filename);
        testConfig.loadFromPropertiesFile(url.getFile());
    }

    public void test() throws Exception {
        String msg = check();
        if (!PoolStaticCenter.isBlank(msg)) throw new BeeDataSourceConfigException(msg);
    }

    private String check() throws Exception {
        if (!"root".equals(testConfig.getUsername())) return "username error";
        if (!"root".equals(testConfig.getPassword())) return "password error";
        if (!ConfigUrl.equals(testConfig.getJdbcUrl())) return "jdbcUrl error";
        if (!ConfigDriver.equals(testConfig.getDriverClassName())) return "driverClassName error";
        if (!"test1".equals(testConfig.getDefaultCatalog())) return "defaultCatalog error";
        if (!"test2".equals(testConfig.getDefaultSchema())) return "defaultSchema error";
        if (!testConfig.isDefaultReadOnly()) return "defaultReadOnly error";
        if (!testConfig.isDefaultAutoCommit()) return "defaultAutoCommit error";
        if (testConfig.getDefaultTransactionIsolationCode() != 1) return "defaultTransactionIsolationCode error";
        if (!"READ_UNCOMMITTED".equals(testConfig.getDefaultTransactionIsolationName()))
            return "defaultTransactionIsolation error";
        if (!"SELECT 1".equals(testConfig.getValidTestSql())) return "connectionTestSQL error";
        if (!"Pool1".equals(testConfig.getPoolName())) return "poolName error";
        if (!testConfig.isFairMode()) return "fairMode error";
        if (testConfig.getInitialSize() != 1) return "initialSize error";
        if (testConfig.getMaxActive() != 10) return "maxActive error";
//        if (testConfig.getBorrowSemaphoreSize() != 4)return"borrowSemaphoreSize error");
        if (testConfig.getMaxWait() != 8000) return "maxWait error";
        if (testConfig.getIdleTimeout() != 18000) return "idleTimeout error";
        if (testConfig.getHoldTimeout() != 30000) return "holdTimeout error";
        if (testConfig.getValidTestTimeout() != 3) return "connectionTestTimeout error";
        if (testConfig.getValidAssumeTime() != 500) return "connectionTestInterval error";
        if (testConfig.getTimerCheckInterval() != 30000) return "idleCheckTimeInterval error";
        if (!testConfig.isForceCloseUsingOnClear()) return "forceCloseUsingOnClear error";
        if (testConfig.getDelayTimeForNextClear() != 3000) return "delayTimeForNextClear error";
        if (!"cn.beecp.pool.ConnectionFactoryByDriver".equals(testConfig.getConnectionFactoryClassName()))
            return "connectionFactoryClassName error";
        if (!"cn.beecp.pool.RawConnectionPool".equals(testConfig.getPoolImplementClassName()))
            return "poolImplementClassName error";
        if (!"cn.beecp.pool.RawConnectionPool".equals(testConfig.getPoolImplementClassName()))
            return "poolImplementClassName error";
        if (!testConfig.isEnableJmx()) return "enableJmx error";

        Field connectPropertiesField = BeeDataSourceConfig.class.getDeclaredField("connectProperties");
        connectPropertiesField.setAccessible(true);
        Map<String, Object> connectProperties = (Map) connectPropertiesField.get(testConfig);
        if (!"true".equals(connectProperties.get("cachePrepStmts")))
            return "connectProperties error";
        if (!"50".equals(connectProperties.get("prepStmtCacheSize")))
            return "connectProperties error";
        if (!"2048".equals(connectProperties.get("prepStmtCacheSqlLimit")))
            return "connectProperties error";
        if (!"true".equals(connectProperties.get("useServerPrepStmts")))
            return "connectProperties error";
        return null;
    }
}
