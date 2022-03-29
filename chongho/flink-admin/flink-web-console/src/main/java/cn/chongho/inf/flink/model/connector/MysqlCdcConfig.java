package cn.chongho.inf.flink.model.connector;

import cn.chongho.inf.flink.constants.ConnectorConfig;
import cn.chongho.inf.flink.utils.StringUtils;
import lombok.Data;

/**
 * @author ming
 *    'connector' = 'mysql-cdc',
 *    'scan.startup.mode' = 'latest-offset',
 *     'server-time-zone' = 'Asia/Shanghai',
 *     'hostname' = 'rm-2zea4h41ew1cd3njd.mysql.rds.aliyuncs.com',
 *     'port' = '3306',
 *     'username' = 'finapp',
 *     'password' = 'FinApp',
 *     'database-name' = 'event2',
 *     'table-name' = 'event_info',
 *     'debezium.skipped.operations' = 'd'
 */
@Data
public class MysqlCdcConfig implements ConfigToSql {

    private final String connector ="mysql-cdc";

    /**
     * 启动模式
     * initial，全量
     * latest-offset，增量
     */
    private String scanStartupMode;

    private String serverTimeZone = "Asia/Shanghai";

    private String hostName;

    private String port;

    private String userName;

    private String passWord;

    private String databaseName;

    private String tableName;

    private Integer debeziumMinRowCountToStreamResult = 0;

    /**
     * 需要跳过的记录
     * c,插入创建
     * u,更新
     * d,删除
     */
    private String debeziumSkippedOperations;


    public static MysqlCdcConfig createConfigByUrl(String url, String userName , String passWord){
        url = url.split("://")[1].split("/")[0];
        String[] urlArray = url.split(":");
        MysqlCdcConfig mysqlCdcConfig = new MysqlCdcConfig();
        mysqlCdcConfig.setHostName(urlArray[0]);
        mysqlCdcConfig.setPort(urlArray[1]);
        mysqlCdcConfig.setUserName(userName);
        mysqlCdcConfig.setPassWord(passWord);
        return mysqlCdcConfig;
    }

    /**
     * 生成配置sql
     * @return
     */
    @Override
    public String doConfigToSql(){

        String configSql = "(" +
                "'" + ConnectorConfig.ConfigValue.CONNECTOR.getValue() + "'" +
                "='" + connector + "'," +
                "'" + ConnectorConfig.ConfigValue.SCANSTARTUPMODE.getValue() + "'" +
                "='" + scanStartupMode + "'," +
                "'" + ConnectorConfig.ConfigValue.SERVERTIMEZONE.getValue() + "'" +
                "='" + serverTimeZone + "'," +
                "'" + ConnectorConfig.ConfigValue.HOSTNAME.getValue() + "'" +
                "='" + hostName + "'," +
                "'" + ConnectorConfig.ConfigValue.PORT.getValue() + "'" +
                "='" + port + "'," +
                "'" + ConnectorConfig.ConfigValue.USERNAME.getValue() + "'" +
                "='" + userName + "'," +
                "'" + ConnectorConfig.ConfigValue.PASSWORD.getValue() + "'" +
                "='" + passWord + "'," +
                "'" + ConnectorConfig.ConfigValue.DATABASENAME.getValue() + "'" +
                "='" + databaseName + "'," +
                "'" + ConnectorConfig.ConfigValue.TABLENAME.getValue() + "'" +
                "='" + tableName + "'," +
                "'" + ConnectorConfig.ConfigValue.DEBEZIUMMINROWCOUNTTOSTREAMRESULT.getValue() + "'" +
                "='" + debeziumMinRowCountToStreamResult + "'," ;


        if(!StringUtils.isEmpty(debeziumSkippedOperations)){
            String operations = "'" + ConnectorConfig.ConfigValue.DEBEZIUMSKIPPEDOPERATIONS.getValue() + "'"+
                            "='" + debeziumSkippedOperations + "',";
            configSql = configSql + operations;
        }
        configSql = configSql.substring(0 , configSql.length() - 1);
        configSql = configSql + ")";
        return configSql;
    }

}
