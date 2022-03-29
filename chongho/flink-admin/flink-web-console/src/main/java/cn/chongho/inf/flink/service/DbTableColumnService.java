package cn.chongho.inf.flink.service;





import cn.chongho.inf.flink.model.DbTableColumn;

import java.util.List;

/**
 * @author ming
 */
public interface DbTableColumnService {

    /**
     * 查询
     * @param pageNum
     * @param pageSize
     * @param dbTableColumn
     * @return
     */
    List<DbTableColumn> getDbTableColumnByPage(int pageNum, int pageSize, DbTableColumn dbTableColumn);


    int selectCount(DbTableColumn dbTableColumn);


    List<DbTableColumn> getAllDbTableColumn(DbTableColumn dbTableColumn);

    List<DbTableColumn> getDbTableColumnByNames(Integer tableId ,List<String> names);

    /**
     * 跟新
     * @param dbTableColumn
     * @return
     */
    boolean updateDbTableColumn(DbTableColumn dbTableColumn);

    boolean deleteDbTableColumn(DbTableColumn dbTableColumn);

    /**
     * 新增
     * @param dbTableColumn
     * @return
     */
    boolean addDbTableColumn(DbTableColumn dbTableColumn);


    void addTableColumnByList(Integer  dbTableId , List<DbTableColumn> columnList);
}
