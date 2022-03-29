package cn.chongho.inf.flink.job;

import org.apache.commons.lang.StringUtils;
import org.apache.flink.api.java.utils.ParameterTool;
import org.apache.flink.streaming.api.CheckpointingMode;
import org.apache.flink.streaming.api.environment.StreamExecutionEnvironment;
import org.apache.flink.table.api.SqlDialect;
import org.apache.flink.table.api.bridge.java.StreamTableEnvironment;

/**
 * @author ming
 */
public class FlinkCdcSqlTemplateJob {

    static {
        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            Class.forName("com.mysql.jdbc.Driver");
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) throws Exception {
        ParameterTool parameterTool = ParameterTool.fromArgs(args);

        StreamExecutionEnvironment env = StreamExecutionEnvironment.getExecutionEnvironment();

        env.enableCheckpointing(60 * 1000L);
        env.getCheckpointConfig().setCheckpointingMode(CheckpointingMode.EXACTLY_ONCE);

        env.getCheckpointConfig().setMaxConcurrentCheckpoints(1);


        StreamTableEnvironment tableEnv = StreamTableEnvironment.create(env);
        tableEnv.getConfig().setSqlDialect(SqlDialect.DEFAULT);

        //数据源
        String sourceTableSql = parameterTool.get("sourceTableSql");
        //目标表
        String targetTableSql = parameterTool.get("targetTableSql");
        String insertSql = parameterTool.get("insertSql");

        if (StringUtils.isBlank(sourceTableSql)
                || StringUtils.isBlank(targetTableSql) || StringUtils.isBlank(insertSql)) {
            return;
        }

        for (String sql : sourceTableSql.split(";")) {
            tableEnv.executeSql(sql);
        }


        tableEnv.executeSql(targetTableSql);

        tableEnv.executeSql(insertSql);
    }
}
