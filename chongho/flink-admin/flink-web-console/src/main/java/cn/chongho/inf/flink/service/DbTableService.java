package cn.chongho.inf.flink.service;




import cn.chongho.inf.flink.model.AddDbTable;
import cn.chongho.inf.flink.model.DbTable;

import java.util.List;

/**
 * @author ming
 */
public interface DbTableService {

    /**
     * 查询
     * @param pageNum
     * @param pageSize
     * @param dbTable
     * @return
     */
    List<DbTable> getDbTableByPage(int pageNum, int pageSize, DbTable dbTable);


    int selectCount(DbTable dbTable);


    List<DbTable> getAllDbTable(DbTable dbTable);

    DbTable getDbTableById(Integer dbTableId);

    /**
     * 跟新
     * @param dbTable
     * @return
     */
    boolean updateDbTable(DbTable dbTable);

    boolean deleteDbTable(DbTable dbTable);

    /**
     * 新增
     * @param dbTable
     * @return
     */
    boolean addDbTable(DbTable dbTable);

    boolean addTableAndColumn(AddDbTable addDbTable);

    List<DbTable> findDbTableBydbId(List<Integer> dbIdList);
}
