package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.mapper.FlinkColonyConfigMapper;
import cn.chongho.inf.flink.model.Cluster;
import cn.chongho.inf.flink.service.ClusterService;
import org.apache.ibatis.session.RowBounds;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;


/**
 * @author ming
 */
@Service
public class ClusterServiceImpl implements ClusterService {

    @Resource
    private FlinkColonyConfigMapper flinkColonyConfigMapper;

    @Override
    public List<Cluster> getClusterByPage(int pageNum, int pageSize, Cluster cluster) {
        int offset = (pageNum-1) * pageSize;
        return flinkColonyConfigMapper.selectByRowBounds(cluster,new RowBounds(offset, pageSize));
    }

    @Override
    public int selectCount(Cluster cluster) {
        return flinkColonyConfigMapper.selectCount(cluster);
    }

    @Override
    public boolean update(Cluster cluster) {
        int ret = 0;
        if (cluster.getId() != null) {
            ret = flinkColonyConfigMapper.updateByPrimaryKeySelective(cluster);
        } else  {
            cluster.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
            ret = flinkColonyConfigMapper.insertSelective(cluster);
        }
        return ret > 0;
    }


    @Override
    public boolean delete(Integer flinkColonyConfigId) {
        Cluster cluster = new Cluster();
        cluster.setId(flinkColonyConfigId);
        cluster.setEnableFlag(Constant.EnableFlag.NOT.ordinal());
        return flinkColonyConfigMapper.updateByPrimaryKeySelective(cluster) > 0;
    }

    @Override
    public Cluster getClusterOne(Cluster cluster) {
        return flinkColonyConfigMapper.selectOne(cluster);
    }

    @Override
    public Cluster getClusterById(Integer flinkColonyConfigId) {
        return flinkColonyConfigMapper.selectByPrimaryKey(flinkColonyConfigId);
    }

    @Override
    public List<Cluster> getAllCluster() {
        Cluster cluster = new Cluster();
        cluster.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        return flinkColonyConfigMapper.select(cluster);
    }
}
