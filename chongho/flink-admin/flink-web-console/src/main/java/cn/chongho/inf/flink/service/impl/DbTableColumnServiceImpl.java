package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.mapper.DbTableColumnMapper;
import cn.chongho.inf.flink.model.DbTableColumn;
import cn.chongho.inf.flink.service.DbTableColumnService;
import org.apache.ibatis.session.RowBounds;
import org.springframework.stereotype.Service;
import tk.mybatis.mapper.entity.Example;

import javax.annotation.Resource;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * @author ming
 */
@Service
public class DbTableColumnServiceImpl implements DbTableColumnService {

    @Resource
    private DbTableColumnMapper dbTableColumnMapper;

    @Override
    public List<DbTableColumn> getDbTableColumnByPage(int pageNum, int pageSize, DbTableColumn dbTableColumn) {
        int offset = (pageNum-1) * pageSize;
        return dbTableColumnMapper.selectByRowBounds(dbTableColumn,new RowBounds(offset, pageSize));
    }

    @Override
    public int selectCount(DbTableColumn dbTableColumn) {
        return dbTableColumnMapper.selectCount(dbTableColumn);
    }

    @Override
    public List<DbTableColumn> getAllDbTableColumn(DbTableColumn dbTableColumn) {
        return dbTableColumnMapper.select(dbTableColumn);
    }

    @Override
    public List<DbTableColumn> getDbTableColumnByNames(Integer tableId, List<String> columnNames) {
        Example example = new Example(DbTableColumn.class);
        Example.Criteria columnExample = example.createCriteria();
        columnExample.andEqualTo("tableId" ,tableId);
        if(columnNames != null && !columnNames.isEmpty()){
            columnExample.andIn("columnName",columnNames);
        }
        return dbTableColumnMapper.selectByExample(example);
    }

    @Override
    public boolean updateDbTableColumn(DbTableColumn dbTableColumn) {
        return dbTableColumnMapper.updateByPrimaryKeySelective(dbTableColumn) > 0;
    }

    @Override
    public boolean deleteDbTableColumn(DbTableColumn dbTableColumn) {
        return dbTableColumnMapper.delete(dbTableColumn) > 0;
    }

    @Override
    public boolean addDbTableColumn(DbTableColumn dbTableColumn) {
        return dbTableColumnMapper.insert(dbTableColumn) > 0;
    }


    @Override
    public void addTableColumnByList(Integer  dbTableId , List<DbTableColumn> columnList){
        DbTableColumn dbTableColumn = new DbTableColumn();
        dbTableColumn.setTableId(dbTableId);
        Map<String, Integer> tableColumnMap = getAllDbTableColumn(dbTableColumn).stream().collect(Collectors.toMap(DbTableColumn::getColumnName,DbTableColumn::getId));
        columnList.forEach( column -> {
            DbTableColumn tableColumn = new DbTableColumn();
            tableColumn.setColumnType(column.getColumnType());
            tableColumn.setColumnComment(column.getColumnComment());
            if(tableColumnMap.containsKey(column.getColumnName())){
                tableColumn.setId(tableColumnMap.get(column.getColumnName()));
                updateDbTableColumn(tableColumn);
                return;
            }
            tableColumn.setColumnName(column.getColumnName());
            tableColumn.setTableId(dbTableId);
            addDbTableColumn(tableColumn);
        });
    }
}
