package cn.chongho.inf.flink.service.tasks;

import cn.chongho.inf.flink.mapper.CdcJobMapper;
import cn.chongho.inf.flink.mapper.JobMapper;
import cn.chongho.inf.flink.model.*;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.ClusterService;
import cn.chongho.inf.flink.service.impl.AlertEventService;
import com.alibaba.fastjson.JSONObject;
import cn.chongho.inf.flink.constants.Constant;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;
import tk.mybatis.mapper.entity.Example;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.TimeUnit;
import java.util.function.Function;
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

    private static Map<String, Long> jobRestartingTime = new ConcurrentHashMap<>();

    /**
     * 过期时间
     */
    private static final int EXPIRE_TIME = 5*60;

    @Autowired
    private AlertEventService alertEventService;

    @Value("${alert-event.pushRobotId}")
    private String robotId;

//    @Scheduled(fixedRate = 30*1000L)
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

    private void doSyncCluster(Cluster flinkColonyConfig){

        Integer flinkColonyId = flinkColonyConfig.getId();

        Map<String, Job> jobMap = findRunningJob(flinkColonyId).stream().collect(Collectors.toMap(Job::getJobId, Function.identity()));

        Map<String, CdcJob> cdcJobMap = findRunningCdcJob(flinkColonyId).stream().collect(Collectors.toMap(CdcJob::getJobId, Function.identity()));

        String flinkColonyUrl = flinkColonyConfig.getUrl();

        List<JSONObject> allJobs = jobApi.getAllJobs(flinkColonyUrl);
        List<String> allJobIdList = new ArrayList<>(allJobs == null ? 0:allJobs.size());
        if(allJobs != null){
            for(JSONObject jsonObject : allJobs){
                String state = jsonObject.getString("state");
                Constant.JobState jobState = Enum.valueOf(Constant.JobState.class, state);
                String jid = jsonObject.getString("jid");
                Integer status = jobState.ordinal();
                BaseJob baseJob;
                if(jobMap.containsKey(jid)){
                    baseJob = jobMap.get(jid);
                    //状态改变
                    if(!status.equals(jobMap.get(jid).getStatus())){
                        jobMapper.updateJobStatusByJobId(jid, status);
                    }
                    CompletableFuture.runAsync(() -> sendJobStatusChange(baseJob, jobState));
                    cancelJob(baseJob, jobState, flinkColonyUrl);
                }else if(cdcJobMap.containsKey(jid)){
                    baseJob = cdcJobMap.get(jid);
                    //状态改变
                    if(!status.equals(cdcJobMap.get(jid).getStatus())) {
                        cdcJobMapper.updateJobStatusByJobId(jid, status);
                    }
                    CompletableFuture.runAsync(() -> sendJobStatusChange(baseJob, jobState));
                    cancelJob(baseJob, jobState, flinkColonyUrl);
                }
                allJobIdList.add(jsonObject.getString("jid"));
            }
        }

        updateOtherJobStatus(allJobIdList, flinkColonyId);
    }


    private List<Job> findRunningJob(Integer flinkColonyId){
        Example jobExample = new Example(Job.class);
        Example.Criteria jobCriteria = jobExample.createCriteria();
        jobCriteria.andEqualTo("flinkColonyId", flinkColonyId);
        jobCriteria.andIsNotNull("jobId");
        return jobMapper.selectByExample(jobExample);
    }

    private List<CdcJob> findRunningCdcJob(Integer flinkColonyId){
        Example cdcjobExample = new Example(CdcJob.class);
        Example.Criteria cdcJobCriteria = cdcjobExample.createCriteria();
        cdcJobCriteria.andEqualTo("flinkColonyId", flinkColonyId);
        cdcJobCriteria.andIsNotNull("jobId");
        return cdcJobMapper.selectByExample(cdcjobExample);
    }

    /**
     * 查询不到的任务修改状态为取消
     * @param allJobIdList
     * @param flinkColonyId
     */
    private void updateOtherJobStatus(List<String> allJobIdList, Integer flinkColonyId){
        Example jobExample = new Example(Job.class);
        Example.Criteria jobCriteria = jobExample.createCriteria();
        jobCriteria.andEqualTo("flinkColonyId", flinkColonyId);

        Example cdcJobExample = new Example(CdcJob.class);
        Example.Criteria cdcJobCriteria = cdcJobExample.createCriteria();
        cdcJobCriteria.andEqualTo("flinkColonyId", flinkColonyId);

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

    /**
     * 重启状态发送
     * @param baseJob
     * @param jobState
     */
    private void sendJobStatusChange(BaseJob baseJob, Constant.JobState jobState){
        if(jobState.ordinal() != Constant.JobState.RESTARTING.ordinal()){
            return;
        }
        log.info("任务{}，状态{}", baseJob.getJobName(), jobState.name());
        String key = baseJob.getJobId() + ":" + baseJob.getFlinkColonyId();
        Long firstTime = jobRestartingTime.get(key);
        //首次
        long thisTime = System.currentTimeMillis();
        long reNotificationTime = EXPIRE_TIME + 120;
        if(firstTime == null || TimeUnit.MILLISECONDS.toSeconds(thisTime - firstTime) > reNotificationTime){
            String msg ="实时计算任务[" + baseJob.getJobName() + "]当前处于" + jobState.name() +" 状态，持续重启超过5分钟将被系统关闭,请注意排查。";

            log.info("{}", msg);

            DingTalkMsg dingTalkMsg = new DingTalkMsg();
            dingTalkMsg.setMessage(msg);
            dingTalkMsg.setGroupId(robotId);
            if(alertEventService.eventUpload(dingTalkMsg)){
                jobRestartingTime.put(key, System.currentTimeMillis());
            }
        }
    }

    /**
     * 重启状态取消任务
     * @param baseJob
     * @param jobState
     * @param flinkColonyUrl
     */
    private void cancelJob(BaseJob baseJob, Constant.JobState jobState, String flinkColonyUrl){
        if(jobState.ordinal() != Constant.JobState.RESTARTING.ordinal()){
            return;
        }
        String key = baseJob.getJobId() + ":" + baseJob.getFlinkColonyId();
        Long firstTime = jobRestartingTime.get(key);
        if(firstTime == null){
            return;
        }
        long thisTime = System.currentTimeMillis();
        long seconds = TimeUnit.MILLISECONDS.toSeconds(thisTime - firstTime);
        log.info("任务{}，状态{},持续时间{}", baseJob.getJobName(), jobState.name(), seconds);
        if(seconds >= EXPIRE_TIME){
            try {
                jobApi.cancelJob(flinkColonyUrl, baseJob.getJobId());
                String msg ="实时计算任务[" + baseJob.getJobName() + "]持续" + jobState.name() +" 状态超过5分，已被系统关闭，请注意！。";
                DingTalkMsg dingTalkMsg = new DingTalkMsg();
                dingTalkMsg.setMessage(msg);
                dingTalkMsg.setGroupId(robotId);
                if(alertEventService.eventUpload(dingTalkMsg)){
                    jobRestartingTime.remove(key);
                }
            }catch (Exception e){
                log.error(e.getMessage());
            }
        }
    }
}
