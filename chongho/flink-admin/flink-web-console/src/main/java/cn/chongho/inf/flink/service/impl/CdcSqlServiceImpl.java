package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.constants.DbColunmDataType;
import cn.chongho.inf.flink.model.connector.JdbcConfig;
import cn.chongho.inf.flink.model.connector.MysqlCdcConfig;
import cn.chongho.inf.flink.service.DbSourceService;
import cn.chongho.inf.flink.service.DbTableColumnService;
import cn.chongho.inf.flink.service.DbTableService;
import cn.chongho.inf.flink.utils.DesUtils;
import cn.chongho.inf.flink.utils.StringUtils;
import com.alibaba.fastjson.JSON;
import cn.chongho.inf.flink.model.*;
import cn.chongho.inf.flink.model.connector.ElasticSearchConfig;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * @author ming
 */
@Service
public class CdcSqlServiceImpl {

    @Value("${sys.des.secretkey}")
    private String desSecretkey;

    @Value("${sys.des.offset}")
    private String desOffset;

    @Resource
    private DbTableColumnService dbTableColumnService;

    @Autowired
    private DbSourceService dbSourceService;

    @Autowired
    private DbTableService dbTableService;

    private ThreadLocal<Map<String, String>> tableNameThreadLocal = new ThreadLocal<>();

    /**
     * 构造源flinksql
     * @param cdcJob
     * @param sourceInfo
     * @return
     */
    public String buildSourceSql(CdcJob cdcJob, SourceInfo sourceInfo){

        DbSource db = dbSourceService.getDbSourceById(sourceInfo.getDbId());
        DbTable table = dbTableService.getDbTableById(sourceInfo.getTableId());


        String tableName = createTableName(cdcJob.getId() , db.getId() ,db.getName(), table.getTableName());

        Map<String, String> tableNameMap = tableNameThreadLocal.get();
        if(tableNameMap == null){
            tableNameMap = new HashMap<>();
        }
        tableNameMap.put(createTableNameKey(db.getId() ,db.getName(), table.getTableName()) ,tableName);
        tableNameThreadLocal.set(tableNameMap);

        List<String> sourceColumnNameList = null;
        Integer jobType = cdcJob.getJobType();

        StringBuilder sourcePrimaryColumn = null;
        //这里如果是单表同步，指定了需要同步的列，只查出需要同步的列
        if(jobType.equals(Constant.CdcJobType.POINT_TO_POINT_SQL.getValue())){
            List<ColumnAssociation> columnAssociationList = JSON.parseArray(cdcJob.getColumnAssociation() ,ColumnAssociation.class);

            sourceColumnNameList = columnAssociationList.stream().map(ColumnAssociation::getSourceColumnName).collect(Collectors.toList());

            //单表同步的主键列为目标表指定的主键列对应的列
            Map<String, String> colMap = columnAssociationList.stream()
                    .collect(Collectors.toMap(ColumnAssociation::getTargetColumnName, ColumnAssociation::getSourceColumnName));
            sourcePrimaryColumn = new StringBuilder();
            for(String key : cdcJob.getPrimaryColumn().split(",")){
                sourcePrimaryColumn.append(colMap.get(key));
                sourcePrimaryColumn.append(",");
            }
            sourcePrimaryColumn.deleteCharAt(sourcePrimaryColumn.length() - 1);
        }else if(jobType.equals(Constant.CdcJobType.CONFLUENCE_SQL.getValue())){
            sourcePrimaryColumn = new StringBuilder(sourceInfo.getTablePriKey());
        }
        //列信息
        List<DbTableColumn> columnList = dbTableColumnService.getDbTableColumnByNames(table.getId() , sourceColumnNameList);
        StringBuilder sqlBuilder = new StringBuilder();

        String tableSql = createTableSql(db.getProviderType(), tableName, columnList, sourcePrimaryColumn.toString());

        sqlBuilder.append(tableSql).append(" WITH ");

        db.getProviderType().toUpperCase();

        Constant.DbType dbType = Enum.valueOf(Constant.DbType.class, db.getProviderType().toUpperCase());

        String dbUsernameAfterDecrypt = DesUtils.decrypt(desSecretkey, desOffset, db.getUsername());

        String dbPasswordAfterDecrypt = DesUtils.decrypt(desSecretkey, desOffset, db.getPassword());


        switch (dbType){
            case MYSQL:
                MysqlCdcConfig cdcConfig = MysqlCdcConfig.createConfigByUrl(db.getUrl(), dbUsernameAfterDecrypt, dbPasswordAfterDecrypt);

                cdcConfig.setDatabaseName(db.getName());
                cdcConfig.setTableName(table.getTableName());

                cdcConfig.setScanStartupMode(cdcJob.getScanStartupMode());
                cdcConfig.setDebeziumSkippedOperations(cdcJob.getSkippedOperations());

                sqlBuilder.append(cdcConfig.doConfigToSql());
                break;
            case ELASTICSEARCH:
                break;
        }
        return sqlBuilder.toString();
    }


    /**
     * 构造目标flinksql
     * @param cdcJob
     * @return
     */
    public String buildTargetSql(CdcJob cdcJob){

        DbSource db = dbSourceService.getDbSourceById(cdcJob.getTargetDbId());
        DbTable table = dbTableService.getDbTableById(cdcJob.getTargetTableId());


        String tableName = createTableName(cdcJob.getId(), db.getId() ,db.getName(), table.getTableName());

        Map<String, String> tableNameMap = tableNameThreadLocal.get();
        tableNameMap.put(createTableNameKey(db.getId() ,db.getName(), table.getTableName()) ,tableName);
        tableNameThreadLocal.set(tableNameMap);

        List<String> targetColumnNameList = null;
        Integer jobType = cdcJob.getJobType();

        //这里如果是单表同步，指定了需要同步的列，只查出需要同步的列
        if(jobType.equals(Constant.CdcJobType.POINT_TO_POINT_SQL.getValue())){
            List<ColumnAssociation> columnAssociationList = JSON.parseArray(cdcJob.getColumnAssociation() ,ColumnAssociation.class);
            targetColumnNameList = columnAssociationList.stream().map(ColumnAssociation::getTargetColumnName).collect(Collectors.toList());
        }
        //列信息
        List<DbTableColumn> columnList = dbTableColumnService.getDbTableColumnByNames(table.getId() , targetColumnNameList);

        String tableSql = createTableSql(db.getProviderType(), tableName, columnList, cdcJob.getPrimaryColumn());

        StringBuilder sqlBuilder = new StringBuilder();
        sqlBuilder.append(tableSql).append(" WITH ");

        Constant.DbType dbType = Enum.valueOf(Constant.DbType.class, db.getProviderType().toUpperCase());

        String dbUsernameAfterDecrypt = DesUtils.decrypt(desSecretkey, desOffset, db.getUsername());
        String dbPasswordAfterDecrypt = DesUtils.decrypt(desSecretkey, desOffset, db.getPassword());

        switch (dbType){
            case MYSQL:
                JdbcConfig jdbcConfig = new JdbcConfig(db.getUrl(), dbUsernameAfterDecrypt, dbPasswordAfterDecrypt);

                jdbcConfig.setTableName(table.getTableName());
                sqlBuilder.append(jdbcConfig.doConfigToSql());
                break;
            case ELASTICSEARCH:
                ElasticSearchConfig elasticSearchConfig = new ElasticSearchConfig(db.getUrl(), dbUsernameAfterDecrypt, dbPasswordAfterDecrypt);

                elasticSearchConfig.setIndex(table.getTableName());
                sqlBuilder.append(elasticSearchConfig.doConfigToSql());
                break;
            default:
                break;
        }
        return sqlBuilder.toString();
    }


    private String createTableName(Integer jobId, Integer dbId,String dbName ,String tableName){
        String tempName = dbName + "_" + dbId + "_" + tableName + "_" + jobId;
        return tempName.replaceAll("-", "_");
    }

    private String createTableNameKey(Integer dbId,String dbName ,String tableName){
        return dbName + "_" + dbId + "_" + tableName ;
    }


    /**
     * 根据给定的列信息返回表sql
     * @param dbTypeName 数据库类型
     * @param tableName 按规则生成的表名
     * @param columnList 列
     * @param primaryColumn 主键列信息，多列用‘,’拼接
     * @return
     */
    private String createTableSql(String dbTypeName,String tableName ,List<DbTableColumn> columnList ,String primaryColumn ){

        Constant.DbType dbType = Enum.valueOf(Constant.DbType.class, dbTypeName.toUpperCase());

        StringBuilder tableSql = new StringBuilder("CREATE TABLE ").append(tableName).append(" ( ");

        columnList.forEach(column -> {
            tableSql.append(column.getColumnName()).append(" ");
            String columnType = column.getColumnType();

            String dataType;

            String flinkDataType;
            switch (dbType){
                case MYSQL:

                    dataType = columnType.split("\\(")[0];

                    DbColunmDataType.MysqlDataType mysqlDataType = null;
                    if(columnType.contains(DbColunmDataType.UNSIGNED)){
                        mysqlDataType = Enum.valueOf(DbColunmDataType.MysqlDataType.class , (dataType+ DbColunmDataType.UNSIGNED).toUpperCase());
                    }
                    if(mysqlDataType == null){
                        mysqlDataType = Enum.valueOf(DbColunmDataType.MysqlDataType.class , dataType.toUpperCase());
                    }
                    flinkDataType = mysqlDataType.getFlinkDataType();
                    if(mysqlDataType.isNeeddDigit() && columnType.contains("(")){
                        flinkDataType = flinkDataType + "(" + columnType.split("\\(")[1].split("\\)")[0] + ")";
                    }
                    tableSql.append(flinkDataType).append(",");
                    break;
                case ORACLE:
                    break;
                case ELASTICSEARCH:

                    dataType = columnType;

                    DbColunmDataType.ElasticsearchDataType elasticsearchDataType = Enum.valueOf(DbColunmDataType.ElasticsearchDataType.class , dataType.toUpperCase());
                    flinkDataType = elasticsearchDataType.getFlinkDataType();
                    tableSql.append(flinkDataType).append(",");
                    break;
            }
        });
        if(StringUtils.isEmpty(primaryColumn)){
            return tableSql.deleteCharAt(tableSql.length()-1).append(")").toString();
        }

        tableSql.append("PRIMARY KEY (").append(primaryColumn).append(") NOT ENFORCED ");

        tableSql.append(")");
        return tableSql.toString();
    }


    /**
     * 插入语句
     * @param cdcJob
     * @return
     */
    public String createInsertSql(CdcJob cdcJob ){

        Integer jobType = cdcJob.getJobType();

        if(jobType.equals(Constant.CdcJobType.CONFLUENCE_SQL.getValue())){
            String columnAssociation = cdcJob.getColumnAssociation();
            Map<String, String> tableNameMap = tableNameThreadLocal.get();
            for(String nameKey : tableNameMap.keySet()){
                columnAssociation = columnAssociation.replace(nameKey , tableNameMap.get(nameKey));
            }
            tableNameThreadLocal.remove();
            return columnAssociation;
        }
        List<ColumnAssociation> columnAssociationList = JSON.parseArray(cdcJob.getColumnAssociation() ,ColumnAssociation.class);
        SourceInfo sourceInfo = JSON.parseArray(cdcJob.getSourceInfo() , SourceInfo.class).get(0);

        DbSource sourceDb = dbSourceService.getDbSourceById(sourceInfo.getDbId());
        DbTable sourceTable = dbTableService.getDbTableById(sourceInfo.getTableId());

        String sourceTableName = createTableName(cdcJob.getId() , sourceDb.getId() ,sourceDb.getName(), sourceTable.getTableName());

        String targetTableName = createTableName(cdcJob.getId() , cdcJob.getTargetDbId() ,cdcJob.getTargetDbName(), cdcJob.getTargetTableName());

        StringBuilder insertSql = new StringBuilder();
        String targetColumn = columnAssociationList.stream().map(ColumnAssociation::getTargetColumnName).collect(Collectors.joining(","));
        String sourceColumn = columnAssociationList.stream().map(ColumnAssociation::getSourceColumnName).collect(Collectors.joining(","));
        insertSql.append("INSERT  INTO ").append(targetTableName).append(" ( ").append(targetColumn).append(")");
        insertSql.append(" SELECT ").append(sourceColumn).append(" FROM ").append(sourceTableName);
        return insertSql.toString();
    }
}
