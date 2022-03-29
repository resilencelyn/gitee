package cn.chongho.inf.flink.model.connector;

import cn.chongho.inf.flink.constants.ConnectorConfig;
import cn.chongho.inf.flink.utils.StringUtils;
import lombok.Data;

/**
 * @author ming
 */
@Data
public class ElasticSearchConfig implements ConfigToSql{

    private final String connector = "elasticsearch-7";

    private String hosts;

    private String index;

    private String userName;

    private String passWord;

    public ElasticSearchConfig(String hosts, String userName, String passWord) {
        this.hosts = hosts;
        this.userName = userName;
        this.passWord = passWord;
    }

    public ElasticSearchConfig() {
    }


    @Override
    public String doConfigToSql() {
        String configSql = "(" +
                "'" + ConnectorConfig.ConfigValue.CONNECTOR.getValue() + "'" +
                "='" + connector + "'," +
                "'" + ConnectorConfig.ConfigValue.HOSTS.getValue() + "'" +
                "='" + hosts + "'," +
                "'" + ConnectorConfig.ConfigValue.INDEX.getValue() + "'" +
                "='" + index + "',"  ;
            if(!StringUtils.isEmpty(userName) && !StringUtils.isEmpty(passWord)){
                configSql = configSql +
                        "'" + ConnectorConfig.ConfigValue.USERNAME.getValue() + "'" +
                        "='" + userName + "'," +
                        "'" + ConnectorConfig.ConfigValue.PASSWORD.getValue() + "'" +
                        "='" + passWord + "'," ;
            }
            configSql = configSql.substring(0 , configSql.length() - 1);
            configSql = configSql + ")";
        return configSql;
    }
}
