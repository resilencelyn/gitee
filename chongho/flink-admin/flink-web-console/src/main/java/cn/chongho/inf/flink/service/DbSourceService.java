package cn.chongho.inf.flink.service;



import cn.chongho.inf.flink.model.DbSource;

import java.util.List;

/**
 * @author ming
 */
public interface DbSourceService {

    /**
     * 查询
     * @param pageNum
     * @param pageSize
     * @param dbSource
     * @return
     */
    List<DbSource> getDbSourceByPage(int pageNum, int pageSize, DbSource dbSource);


    int selectCount(DbSource dbSource);

    List<DbSource> getAllDbSource(DbSource dbSource);

    DbSource getDbSourceById(Integer dbSourceId);
    /**
     * 跟新
     * @param dbSource
     * @return
     */
    boolean updateDbSource(DbSource dbSource);

    /**
     * 新增
     * @param dbSource
     * @return
     */
    boolean addDbSource(DbSource dbSource);

    /**
     * 同步数据库的表列信息
     * @param dbSourceId
     * @return
     */
    boolean syncDbInfo(Integer dbSourceId);
}
