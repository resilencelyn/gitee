package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.mapper.DbTableMapper;
import cn.chongho.inf.flink.model.AddDbTable;
import cn.chongho.inf.flink.model.DbSource;
import cn.chongho.inf.flink.model.DbTable;
import cn.chongho.inf.flink.model.DbTableColumn;
import cn.chongho.inf.flink.service.DbSourceService;
import cn.chongho.inf.flink.service.DbTableColumnService;
import cn.chongho.inf.flink.service.DbTableService;
import com.alibaba.fastjson.JSONObject;
import org.apache.ibatis.session.RowBounds;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import tk.mybatis.mapper.entity.Example;

import javax.annotation.Resource;
import java.util.List;

/**
 * @author ming
 */
@Service
public class DbTableServiceImpl implements DbTableService {

    @Resource
    private DbTableMapper dbTableMapper;

    @Autowired
    private DbSourceService dbSourceService;

    @Autowired
    private DbTableColumnService dbTableColumnService;

    @Override
    public List<DbTable> getDbTableByPage(int pageNum, int pageSize, DbTable dbSource) {
        int offset = (pageNum-1) * pageSize;
        return dbTableMapper.selectByRowBounds(dbSource,new RowBounds(offset, pageSize));
    }

    @Override
    public int selectCount(DbTable dbSource) {
        return dbTableMapper.selectCount(dbSource);
    }

    @Override
    public List<DbTable> getAllDbTable(DbTable dbSource) {
        return dbTableMapper.select(dbSource);
    }

    @Override
    public DbTable getDbTableById(Integer dbTableId) {
        return dbTableMapper.selectOne(new DbTable(dbTableId));
    }

    @Override
    public boolean updateDbTable(DbTable dbSource) {
        return dbTableMapper.updateByPrimaryKeySelective(dbSource) > 0;
    }

    @Override
    public boolean deleteDbTable(DbTable dbTable) {
        return dbTableMapper.delete(dbTable) > 0;
    }

    @Override
    public boolean addDbTable(DbTable dbSource) {
        return dbTableMapper.insert(dbSource) > 0;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public boolean addTableAndColumn(AddDbTable addDbTable) {
        DbSource dbSourceById = dbSourceService.getDbSourceById(addDbTable.getDbId());
        if(dbSourceById == null){
            return false;
        }
        DbTable dbTable = new DbTable();
        dbTable.setDbId(dbSourceById.getId());
        dbTable.setTableName(addDbTable.getTableName());
        DbTable table = dbTableMapper.selectOne(dbTable);
        Integer tableId;
        if(table == null){
            DbTable newTable = new DbTable();
            newTable.setDbId(dbSourceById.getId());
            newTable.setTableName(addDbTable.getTableName());
            newTable.setTableComment(addDbTable.getTableComment());
            dbTableMapper.insert(newTable);

            tableId = newTable.getId();
        }else{
            tableId = table.getId();
            DbTable updateTable = new DbTable();
            updateTable.setId(tableId);
            updateTable.setTableComment(addDbTable.getTableComment());
            dbTableMapper.updateByPrimaryKeySelective(updateTable);
        }
        List<DbTableColumn> columnList = JSONObject.parseArray(addDbTable.getColumnAssociation() ,DbTableColumn.class);
        dbTableColumnService.addTableColumnByList(tableId ,columnList);
        return true;
    }

    @Override
    public List<DbTable> findDbTableBydbId(List<Integer> dbIdList) {
        Example example = new Example(DbTable.class);
        Example.Criteria criteria = example.createCriteria();
        criteria.andIn("dbId" , dbIdList);
        return dbTableMapper.selectByExample(example);
    }
}
