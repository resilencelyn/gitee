package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.mapper.ClusterMapper;
import cn.chongho.inf.flink.model.CdcJob;
import cn.chongho.inf.flink.model.Cluster;
import cn.chongho.inf.flink.model.Job;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.CdcJobService;
import cn.chongho.inf.flink.service.ClusterService;
import cn.chongho.inf.flink.service.JobService;
import com.alibaba.fastjson.JSONObject;
import lombok.extern.slf4j.Slf4j;
import org.apache.ibatis.session.RowBounds;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;


/**
 * @author ming
 */
@Service
@Slf4j
public class ClusterServiceImpl implements ClusterService {

    @Resource
    private ClusterMapper clusterMapper;

    @Autowired
    private JobApi jobApi;

    @Autowired
    private JobService jobService;

    @Autowired
    private CdcJobService cdcJobService;


    private List<Map<String, String>> flinkListCache;

    private long lastTimestamp = 0L;

    @Override
    public List<Cluster> getClusterByPage(int pageNum, int pageSize, Cluster cluster) {
        int offset = (pageNum-1) * pageSize;
        return clusterMapper.selectByRowBounds(cluster,new RowBounds(offset, pageSize));
    }

    @Override
    public int selectCount(Cluster cluster) {
        return clusterMapper.selectCount(cluster);
    }

    @Override
    public boolean update(Cluster cluster) {
        int ret = 0;
        if (cluster.getId() != null) {
            ret = clusterMapper.updateByPrimaryKeySelective(cluster);
        } else  {
            cluster.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
            ret = clusterMapper.insertSelective(cluster);
        }
        return ret > 0;
    }


    @Override
    public boolean delete(Integer flinkColonyConfigId) {
        Cluster cluster = new Cluster();
        cluster.setId(flinkColonyConfigId);
        cluster.setEnableFlag(Constant.EnableFlag.NOT.ordinal());
        return clusterMapper.updateByPrimaryKeySelective(cluster) > 0;
    }

    @Override
    public Cluster getClusterOne(Cluster cluster) {
        return clusterMapper.selectOne(cluster);
    }

    @Override
    public Cluster getClusterById(Integer flinkColonyConfigId) {
        return clusterMapper.selectByPrimaryKey(flinkColonyConfigId);
    }

    @Override
    public List<Cluster> getAllCluster() {
        Cluster cluster = new Cluster();
        cluster.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        return clusterMapper.select(cluster);
    }

    @Override
    public List<Map<String, String>> getColonyInfo() {
        long thisTime = System.currentTimeMillis();
        long seconds = TimeUnit.MILLISECONDS.toSeconds(thisTime - lastTimestamp);
        if(flinkListCache != null && seconds < 60L){
            return flinkListCache;
        }

        List<Cluster> allFlinkColonyConfig = getAllCluster();

        List<Map<String, String>> flinkList = new ArrayList<>(allFlinkColonyConfig.size());


        Job job = new Job();
        job.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        job.setStatus(Constant.JobState.RUNNING.ordinal());
        Map<Integer, List<Job>> jobGroupByFlink = jobService.selectAll(job).stream().collect(Collectors.groupingBy(Job::getFlinkColonyId));

        CdcJob cdcJob = new CdcJob();
        cdcJob.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
        cdcJob.setStatus(Constant.JobState.RUNNING.ordinal());
        Map<Integer, List<CdcJob>> cdcJobGroupByFlink = cdcJobService.selectAll(cdcJob).stream().collect(Collectors.groupingBy(CdcJob::getFlinkColonyId));


        Map<String, String> thisFlinkInfo;
        for(Cluster flinkColonyConfig : allFlinkColonyConfig){
            try {
                thisFlinkInfo = new HashMap<>();
                JSONObject overviewInfo = jobApi.getOverviewInfo(flinkColonyConfig.getUrl());
                for(String key : overviewInfo.keySet()){
                    thisFlinkInfo.put(key.replace("-", ""), overviewInfo.getString(key));
                }
                if(jobGroupByFlink.containsKey(flinkColonyConfig.getId())){
                    thisFlinkInfo.put("jobRuningCount", String.valueOf(jobGroupByFlink.get(flinkColonyConfig.getId()).size()));
                }else{
                    thisFlinkInfo.put("jobRuningCount", "0");
                }

                if(cdcJobGroupByFlink.containsKey(flinkColonyConfig.getId())){
                    thisFlinkInfo.put("cdcjobRuningCount", String.valueOf(cdcJobGroupByFlink.get(flinkColonyConfig.getId()).size()));
                }else{
                    thisFlinkInfo.put("cdcjobRuningCount", "0");
                }
                thisFlinkInfo.put("name", flinkColonyConfig.getName());
                thisFlinkInfo.put("url", flinkColonyConfig.getUrl());
                flinkList.add(thisFlinkInfo);
            }catch (Exception e){
                log.error(e.getMessage());
            }
        }
        flinkListCache = flinkList;
        lastTimestamp = thisTime;
        return flinkList;
    }
}
