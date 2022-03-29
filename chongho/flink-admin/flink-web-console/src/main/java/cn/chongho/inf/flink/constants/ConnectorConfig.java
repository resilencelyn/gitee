package cn.chongho.inf.flink.constants;

/**
 * @author ming
 */

public class ConnectorConfig {

    public enum ConfigValue {
        CONNECTOR("connector"),
        URL("url"),
        HOSTNAME("hostname"),
        PORT("port"),
        USERNAME("username"),
        PASSWORD("password"),
        SCANSTARTUPMODE("scan.startup.mode"),
        SERVERTIMEZONE("server-time-zone"),
        DATABASENAME("database-name"),
        TABLENAME("table-name"),
        DEBEZIUMMINROWCOUNTTOSTREAMRESULT("debezium.min.row. count.to.stream.result"),
        DEBEZIUMSKIPPEDOPERATIONS("debezium.skipped.operations"),
        HOSTS("hosts"),
        INDEX("index"),
        ;

        ConfigValue(String value) {
            this.value = value;
        }

        public String getValue() {
            return value;
        }

        private String value;
    }

    public enum ScanStartupMode {
        INITIAL("initial"),
        LATESTOFFSET("latest-offset");

        ScanStartupMode(String value) {
            this.value = value;
        }

        private String value;

        public String getValue() {
            return value;
        }
    }
}
