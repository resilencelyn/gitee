package cn.chongho.inf.flink.service.tasks;

import cn.chongho.inf.flink.mapper.CdcJobMapper;
import cn.chongho.inf.flink.mapper.JobMapper;
import cn.chongho.inf.flink.model.CdcJob;
import cn.chongho.inf.flink.model.Cluster;
import cn.chongho.inf.flink.model.Job;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.ClusterService;
import com.alibaba.fastjson.JSONObject;
import cn.chongho.inf.flink.constants.Constant;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;
import tk.mybatis.mapper.entity.Example;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 定时拉取任务更新状态
 * @author ming
 */
@Component
@EnableScheduling
@Slf4j
public class SyncJobStatusTask {

    @Autowired
    private JobApi jobApi;

    @Resource
    private JobMapper jobMapper;

    @Resource
    private CdcJobMapper cdcJobMapper;

    @Autowired
    private ClusterService clusterService;


    @Scheduled(fixedRate = 30000)
    public void doSync(){

        List<Cluster> allCluster = clusterService.getAllCluster();

        for(Cluster cluster : allCluster){
            try {
                doSyncCluster(cluster);
            } catch (Exception e) {
                log.warn("sync cluster info error", e);
            }
        }
    }

    private void doSyncCluster(Cluster cluster){
        Job job = new Job();
        job.setFlinkColonyId(cluster.getId());
        List<String> jobIdList = jobMapper.select(job).stream().map(Job::getJobId).collect(Collectors.toList());

        List<JSONObject> allJobs = jobApi.getAllJobs(cluster.getUrl());
        List<String> allJobIdList = new ArrayList<>(allJobs == null ? 0:allJobs.size());
        if(allJobs != null) {
            allJobs.forEach(jsonObject -> {
                String state = jsonObject.getString("state");
                Constant.JobState jobState = Enum.valueOf(Constant.JobState.class, state);
                String jid = jsonObject.getString("jid");
                Integer status = jobState == null ? 2 : jobState.ordinal();
                if(jobIdList.contains(jid)){
                    jobMapper.updateJobStatusByJobId(jid, status);
                }else{
                    cdcJobMapper.updateJobStatusByJobId(jid, status);
                }
                allJobIdList.add(jsonObject.getString("jid"));
            });
        }

        /**
         * 查询不到的任务修改状态为取消
         */
        Example jobExample = new Example(Job.class);
        Example.Criteria jobCriteria = jobExample.createCriteria();
        jobCriteria.andEqualTo("flinkColonyId", cluster.getId());

        Example cdcJobExample = new Example(CdcJob.class);
        Example.Criteria cdcJobCriteria = cdcJobExample.createCriteria();
        cdcJobCriteria.andEqualTo("flinkColonyId", cluster.getId());

        if(allJobIdList.isEmpty()){

            jobCriteria.andNotEqualTo("status",Constant.JobState.CANCELED.ordinal());

            cdcJobCriteria.andNotEqualTo("status",Constant.JobState.CANCELED.ordinal());

        }else{

            jobCriteria.andNotIn("jobId",allJobIdList);

            cdcJobCriteria.andNotIn("jobId",allJobIdList);
        }

        Job updateJob = new Job();
        updateJob.setStatus(Constant.JobState.CANCELED.ordinal());
        jobMapper.updateByExampleSelective(updateJob, jobExample);

        CdcJob updateCdcJob = new CdcJob();
        updateCdcJob.setStatus(Constant.JobState.CANCELED.ordinal());
        cdcJobMapper.updateByExampleSelective(updateCdcJob, cdcJobExample);
    }
}
