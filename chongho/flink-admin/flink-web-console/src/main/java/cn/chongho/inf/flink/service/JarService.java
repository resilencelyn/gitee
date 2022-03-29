package cn.chongho.inf.flink.service;


import cn.chongho.inf.flink.model.Jar;

import java.util.Date;
import java.util.List;

/**
 * @author ming
 */
public interface JarService {

    /**
     * 查询
     * @param pageNum
     * @param pageSize
     * @param jar
     * @return
     */
    List<Jar> getJarsByPage(int pageNum , int pageSize , Jar jar , Date startTime ,Date endTime);

    /**
     * 返回总条数
     * @param jar
     * @return
     */
    int selectCount(Jar jar, Date startTime ,Date endTime);

    /**
     * 查询
     * @param jar
     * @return
     */
    List<Jar> getAllJars(Jar jar);

    /**
     * 跟新
     * @param jar
     * @return
     */
    boolean updateJar(Jar jar);

    /**
     * 新增
     * @param jar
     * @return
     */
    boolean addJar(Jar jar);

    /**
     * 返回一个
     * @param jar
     * @return
     */
    Jar getJarOne(Jar jar);
}
