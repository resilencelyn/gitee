package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.model.DbSource;
import cn.chongho.inf.flink.model.DbTable;
import cn.chongho.inf.flink.model.DbTableColumn;
import cn.chongho.inf.flink.service.DbSourceService;
import cn.chongho.inf.flink.service.DbTableColumnService;
import cn.chongho.inf.flink.service.DbTableService;
import cn.chongho.inf.flink.utils.DesUtils;
import com.alibaba.druid.pool.DruidDataSource;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * @author ming
 */
@Service
@Slf4j
public class DbInfoServiceImpl {

    @Autowired
    private DbSourceService dbSourceService;

    @Autowired
    private DbTableService dbTableService;

    @Autowired
    private DbTableColumnService dbTableColumnService;

    @Value("${sys.des.secretkey}")
    private String secretKey;

    @Value("${sys.des.offset}")
    private String offset;

    /**
     * 查询表信息保存
     * @param dbId
     */
//    @Async("myTaskAsyncPool")
    public void createTableByDbId(Integer dbId){
        DbSource dbSource = dbSourceService.getDbSourceById(dbId);
        log.info("同步数据库:({},{})表信息",dbId, dbSource.getName());

        DruidDataSource druidDataSource = new DruidDataSource();
        druidDataSource.setUsername(DesUtils.decrypt(secretKey, offset, dbSource.getUsername()));
        druidDataSource.setPassword(DesUtils.decrypt(secretKey, offset, dbSource.getPassword()));
        druidDataSource.setUrl(dbSource.getUrl());
        JdbcTemplate jdbcTemplate = new JdbcTemplate(druidDataSource);
        String sql = "SELECT table_name,table_comment FROM information_schema.tables WHERE table_schema='"+ dbSource.getName() +"' AND table_type='base table'";
        List<Map<String, Object>> maps = jdbcTemplate.queryForList(sql);
        log.info("tables:{}", maps.size());
        DbTable dbTable = new DbTable();
        dbTable.setDbId(dbId);
        Map<String, Integer> dbTableNameMap = dbTableService.getAllDbTable(dbTable).stream().collect(Collectors.toMap(DbTable::getTableName,DbTable::getId));
        maps.forEach( stringObjectMap -> {
            String tableName = stringObjectMap.get("table_name").toString();
            if(dbTableNameMap.containsKey(tableName)){
                createTableColumnByDbId(dbTableNameMap.get(tableName) ,jdbcTemplate ,dbSource.getName());
                return;
            }
            DbTable newDbTable = new DbTable();
            newDbTable.setDbId(dbId);
            newDbTable.setTableName(stringObjectMap.get("table_name").toString());
            newDbTable.setTableComment(stringObjectMap.get("table_comment").toString());
            dbTableService.addDbTable(newDbTable);
            createTableColumnByDbId(newDbTable.getId() ,jdbcTemplate ,dbSource.getName());
        });
        druidDataSource.close();
    }


    /**
     * 查询列信息保存
     * @param dbTableId
     * @param jdbcTemplate
     * @param dbName
     */
    private void createTableColumnByDbId(Integer  dbTableId ,JdbcTemplate jdbcTemplate ,String dbName){
        DbTable dbTable = dbTableService.getDbTableById(dbTableId);
        String sql = "SELECT column_name,column_type,column_key,column_comment FROM information_schema.columns WHERE table_schema='"+ dbName +"' AND table_name='"+ dbTable.getTableName() +"'";
        List<Map<String, Object>> maps = jdbcTemplate.queryForList(sql);
        DbTableColumn dbTableColumn = new DbTableColumn();
        dbTableColumn.setTableId(dbTableId);
        List<String> tableColumnList = dbTableColumnService.getAllDbTableColumn(dbTableColumn).stream().map(DbTableColumn::getColumnName).collect(Collectors.toList());
        maps.forEach( stringObjectMap -> {
            String  columnName = stringObjectMap.get("column_name").toString();
            if(tableColumnList.contains(columnName)){
                return;
            }
            DbTableColumn newDbTableColumn = new DbTableColumn();
            newDbTableColumn.setTableId(dbTableId);
            newDbTableColumn.setColumnName(columnName);
            newDbTableColumn.setColumnType(stringObjectMap.get("column_type").toString());
            newDbTableColumn.setColumnKey(stringObjectMap.get("column_key").toString());
            newDbTableColumn.setColumnComment(stringObjectMap.get("column_comment").toString());
            dbTableColumnService.addDbTableColumn(newDbTableColumn);
        });
    }
}
