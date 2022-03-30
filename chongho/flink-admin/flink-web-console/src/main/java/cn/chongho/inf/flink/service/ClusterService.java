package cn.chongho.inf.flink.service;


import cn.chongho.inf.flink.model.Cluster;

import java.util.List;
import java.util.Map;

/**
 * @author ming
 */
public interface ClusterService {

    /**
     * 查询
     * @param pageNum
     * @param pageSize
     * @param cluster
     * @return
     */
    List<Cluster> getClusterByPage(int pageNum , int pageSize , Cluster cluster);

    /**
     * 返回总条数
     * @param cluster
     * @return
     */
    int selectCount(Cluster cluster);


    boolean update(Cluster cluster);

    boolean delete(Integer flinkColonyConfigId);

    /**
     * 返回一个
     * @param cluster
     * @return
     */
    Cluster getClusterOne(Cluster cluster);

    /**
     * 返回一个
     * @param flinkColonyConfigId
     * @return
     */
    Cluster getClusterById(Integer flinkColonyConfigId);

    /**
     * 返回全部
     * @return
     */
    List<Cluster> getAllCluster();

    List<Map<String, String>> getColonyInfo();
}
