package cn.chongho.inf.flink.model.connector;

import cn.chongho.inf.flink.constants.ConnectorConfig;
import lombok.Data;

/**
 * @author ming
 *      'connector' = 'jdbc',
 *      'url' = 'jdbc:mysql://rm-2zea4h41ew1cd3njd.mysql.rds.aliyuncs.com:3306/pecker?serverTimezone=Asia/Shanghai',
 *      'table-name' = 'event_info',
 *      'username' = 'finapp',
 *      'password' = 'FinApp'
 */
@Data
public class JdbcConfig implements ConfigToSql{

    private final String connector = "jdbc";

    private String url;

    private String tableName;

    private String userName;

    private String passWord;

    /**
     * 生成配置sql
     * @return
     */
    @Override
    public String doConfigToSql(){

        String configSql = "(" +
                "'" + ConnectorConfig.ConfigValue.CONNECTOR.getValue() + "'" +
                "='" + connector + "'," +
                "'" + ConnectorConfig.ConfigValue.URL.getValue() + "'" +
                "='" + url + "'," +
                "'" + ConnectorConfig.ConfigValue.TABLENAME.getValue() + "'" +
                "='" + tableName + "'," +
                "'" + ConnectorConfig.ConfigValue.USERNAME.getValue() + "'" +
                "='" + userName + "'," +
                "'" + ConnectorConfig.ConfigValue.PASSWORD.getValue() + "'" +
                "='" + passWord + "'" +
                ")";
        return configSql;
    }

    public JdbcConfig(String url, String userName, String passWord) {
        this.url = url;
        this.userName = userName;
        this.passWord = passWord;
    }

    public JdbcConfig() {
    }
}
