package cn.chongho.inf.flink.service.tasks;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.model.CdcJob;
import cn.chongho.inf.flink.model.CheckPointInfo;
import cn.chongho.inf.flink.model.Job;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.CdcJobService;
import cn.chongho.inf.flink.service.CheckPointInfoService;
import cn.chongho.inf.flink.service.JobService;
import cn.chongho.inf.flink.utils.StringUtils;
import com.alibaba.fastjson.JSONObject;
import lombok.extern.slf4j.Slf4j;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.*;
import java.util.function.Function;
import java.util.stream.Collectors;


/**
 * @author ming
 */
@Component
@EnableScheduling
@Slf4j
public class SyncCheckPoint {

    @Resource
    private JobApi jobApi;

    @Resource
    private CdcJobService cdcJobService;

    @Resource
    private CheckPointInfoService checkPointInfoService;

    @Resource
    private JobService jobService;

    /**
     * 保存点状态为完整状态
     */
    private static final String ENABLE_STATUS = "COMPLETED";


    private static final String JOB_FROM = "jobFrom";

    private static final String IS_SAVEPOINT_KEY = "is_savepoint";

    private static final String STATUS_KEY = "status";

    private static final String EXTERNAL_PATH_KEY = "external_path";

    private static final String TRIGGER_TIMESTAMP = "trigger_timestamp";

//    @Scheduled(fixedRate = 60*1000L)
    public void doSync() {

        Map<Integer, Map<String, Object>> checkpointMap = new HashMap<>();

        Map<Integer, Map<String, Object>> jobPoint = getJobPoint();
        if(jobPoint != null){
            checkpointMap.putAll(jobPoint);
        }
        jobPoint = getCdcJobPoint();
        if(jobPoint != null){
            checkpointMap.putAll(jobPoint);
        }

        Map<String, Object> pointMap;
        CheckPointInfo newCheckpointInfo;

        CheckPointInfo checkPointInfo = new CheckPointInfo();
        checkPointInfo.setPointType(Constant.CheckPointType.CHECK.getValue());
        List<CheckPointInfo> allCheckPointInfo = checkPointInfoService.getAllCheckPointInfo(checkPointInfo);

        Map<String, Integer> dbCheckpointMap = allCheckPointInfo.stream().collect(Collectors.toMap(new Function<CheckPointInfo, String>() {
            @Override
            public String apply(CheckPointInfo checkPointInfo) {
                return checkPointInfo.getJobId() + ":" + checkPointInfo.getJobForm();
            }
            }, new Function<CheckPointInfo, Integer>() {
                @Override
                public Integer apply(CheckPointInfo checkPointInfo) {
                    return checkPointInfo.getId();
                }
        }));

        List<CheckPointInfo> addCheckpointInfo = new ArrayList<>();
        List<CheckPointInfo> updateCheckpointInfo = new ArrayList<>();

        Integer thisJobFrom;
        String thisKey;
        for(Integer jobId : checkpointMap.keySet()){
            pointMap = checkpointMap.get(jobId);
            newCheckpointInfo = new CheckPointInfo();

            thisJobFrom = (Integer) pointMap.get(JOB_FROM);
            newCheckpointInfo.setJobId(jobId);
            newCheckpointInfo.setCreateTime(new Date((long)pointMap.get(TRIGGER_TIMESTAMP)));
            newCheckpointInfo.setJobForm(thisJobFrom);
            newCheckpointInfo.setLocation((String) pointMap.get(EXTERNAL_PATH_KEY));
            newCheckpointInfo.setPointType(Constant.CheckPointType.CHECK.getValue());
            newCheckpointInfo.setTriggerId("");

            thisKey = jobId + ":" + thisJobFrom;
            if(dbCheckpointMap.containsKey(thisKey)){
                newCheckpointInfo.setId(dbCheckpointMap.get(thisKey));
                updateCheckpointInfo.add(newCheckpointInfo);
            }else{
                addCheckpointInfo.add(newCheckpointInfo);
            }
        }
        if(!addCheckpointInfo.isEmpty()){
            checkPointInfoService.addByBatch(addCheckpointInfo);
        }
        if(!updateCheckpointInfo.isEmpty()){
            checkPointInfoService.updateByBatch(updateCheckpointInfo);
        }

    }

    private Map<Integer, Map<String, Object>> getJobPoint(){
        List<Job> jobs = jobService.selectEnabledAndRunJob();
        if(jobs == null || jobs.isEmpty()){
            return null;
        }
        Map<Integer, Map<String, Object>> checkpointMap = new HashMap<>(jobs.size());

        for(Job job : jobs){
            if(StringUtils.isEmpty(job.getJobId())){
                continue;
            }
            try {
                List<JSONObject> checkpoint = jobApi.getCheckpoint(job.getFlinkColonyUrl(), job.getJobId());
                if(checkpoint == null || checkpoint.isEmpty()){
                    continue;
                }
                for(JSONObject point : checkpoint){
                    if(!point.getBoolean(IS_SAVEPOINT_KEY) && ENABLE_STATUS.equals(point.getString(STATUS_KEY))){
                        Map<String, Object> pointMap = new HashMap<>(2);
                        pointMap.put(JOB_FROM, Constant.CheckJobForm.JOB.getValue());
                        pointMap.put(EXTERNAL_PATH_KEY, point.getString(EXTERNAL_PATH_KEY));
                        pointMap.put(TRIGGER_TIMESTAMP, point.get(TRIGGER_TIMESTAMP));
                        checkpointMap.put(job.getId() , pointMap);
                        break;
                    }
                }
            }catch (Exception e){
                log.error("同步任务checkPoint失败{},{}",job.getJobName(), e.getMessage());
            }
        }
        return checkpointMap;
    }


    private Map<Integer, Map<String, Object>> getCdcJobPoint(){

        List<CdcJob> cdcJobList = cdcJobService.selectEnabledAndRunJob();
        if(cdcJobList == null || cdcJobList.isEmpty()){
            return null;
        }
        Map<Integer, Map<String, Object>> checkpointMap = new HashMap<>(cdcJobList.size());

        for(CdcJob cdcJob : cdcJobList){
            if(StringUtils.isEmpty(cdcJob.getJobId())){
                continue;
            }
            try {
                List<JSONObject> checkpoint = jobApi.getCheckpoint(cdcJob.getFlinkColonyUrl(), cdcJob.getJobId());
                if(checkpoint == null || checkpoint.isEmpty()){
                    continue;
                }
                for(JSONObject point : checkpoint){
                    if(!point.getBoolean(IS_SAVEPOINT_KEY) && ENABLE_STATUS.equals(point.getString(STATUS_KEY))){
                        Map<String, Object> pointMap = new HashMap<>(2);
                        pointMap.put(JOB_FROM, Constant.CheckJobForm.CDCJOB.getValue());
                        pointMap.put(EXTERNAL_PATH_KEY, point.getString(EXTERNAL_PATH_KEY));
                        pointMap.put(TRIGGER_TIMESTAMP, point.get(TRIGGER_TIMESTAMP));
                        checkpointMap.put(cdcJob.getId() , pointMap);
                        break;
                    }
                }
            }catch (Exception e){
                log.error("同步任务checkPoint失败{},{}",cdcJob.getJobName(), e.getMessage());
            }
        }
        return checkpointMap;
    }
}
