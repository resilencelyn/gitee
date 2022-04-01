package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.mapper.JobMapper;
import cn.chongho.inf.flink.model.JobRunConfig;
import cn.chongho.inf.flink.model.CheckPointInfo;
import cn.chongho.inf.flink.model.Jar;
import cn.chongho.inf.flink.model.Job;
import cn.chongho.inf.flink.restapi.JarApi;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.*;
import cn.chongho.inf.flink.service.tasks.SyncSavePointTask;
import cn.chongho.inf.flink.utils.StringUtils;
import com.alibaba.fastjson.JSON;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.*;

/**
 * @author ming
 */
@Service
@Slf4j
public class SqlJobServiceImpl implements SqlJobService {

    /**
     * 保存点路径
     */
    private static final String CHECK_POINT_PATH_KEY = "--checkpoint.path";

    @Value("${sys.checkpoint-path}")
    private String checkpointPath;

    @Resource
    private JobMapper jobMapper;

    @Resource
    private JarApi jarApi;

    @Resource
    private JobApi jobApi;

    @Autowired
    private CheckPointInfoService checkPointInfoService;

    @Autowired
    private DataAuthorityService dataAuthorityService;

    @Resource
    private JobRunConfigService jobRunConfigService;

    @Resource
    private JarService jarService;

    @Override
    public List<Job> selectAll(Job job) {
        return jobMapper.select(job);
    }

    @Override
    public List<Job> selectEnabledAndRunJob() {
        return jobMapper.selectAllRuningJob();
    }

    @Override
    public int selectCount(Job job) {
        return jobMapper.countByPage(job);
    }

    @Override
    public List<Job> selectByPage(int page, int pageSize ,Job job, Integer loginUserId) {
        int offset = (page-1) * pageSize;
        return jobMapper.selectByPage(offset, pageSize, job, loginUserId);
    }

    @Override
    public Job selectById(Integer id) {
        return jobMapper.selectByPrimaryKey(id);
    }

    @Override
    public boolean delete(Integer id , Integer loginUserId) {

        Job dbJob = jobMapper.selectByPrimaryKey(id);
        dataAuthorityService.checkDataAuthority(dbJob, Constant.DataType.JOB, loginUserId);
        if(dbJob.getStatus() != Constant.JobState.CANCELED.ordinal()){
            return false;
        }

        Job updateJob = new Job();
        updateJob.setId(id);
        updateJob.setEnableFlag(Constant.EnableFlag.NOT.ordinal());
        updateJob.setUpdateTime(new Date());
        return jobMapper.updateByPrimaryKeySelective(updateJob) > 0;
    }

    @Override
    public boolean update(Job job) {
        int ret = 0;
        job.setUpdateTime(new Date());
        job.setParallelism(job.getParallelism() == null ? 1 : job.getParallelism());
        if (job.getId() != null) {
            Job dbJob = jobMapper.selectByPrimaryKey(job.getId());
            dataAuthorityService.checkDataAuthority(dbJob, Constant.DataType.JOB, job.getUpdateUserId());
            return jobMapper.updateJobById(job);
        } else  {
            job.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
            job.setCreateTime(new Date());
            job.setCreateUserId(job.getUpdateUserId());
            job.setStatus(Constant.EnableFlag.NOT.ordinal());
            job.setJobType(Constant.JobType.SQL_JOB.getValue());
            ret = jobMapper.insertSelective(job);
        }
        return ret > 0;
    }

    /**
     * 运行任务
     * @param id
     * @return
     */
    @Override
    public boolean runJob(Integer id, Integer loginUserId) {
        Job job = jobMapper.findJobById(id);
        dataAuthorityService.checkDataAuthority(job, Constant.DataType.JOB, loginUserId);

        JobRunConfig jobRunConfig = jobRunConfigService.selectByJobType(Constant.JobConfigType.SQL_JOB.name());
        log.info("JobRunConfig{}", JSON.toJSONString(jobRunConfig));
        if(jobRunConfig == null){
            return false;
        }

        Jar jar = jarService.getJarOne(new Jar(jobRunConfig.getJarId() ,Constant.EnableFlag.ENABLE.ordinal()));
        log.info("jar{}", JSON.toJSONString(jar));
        if(jar == null){
            return false;
        }

        List<String> argsArray = new ArrayList<>(2);
        argsArray.add("--jobSql");
        argsArray.add(analysisJobSql(job.getArgs()));
        /**
         * 增加checkpoint路径
         */
        argsArray.add(CHECK_POINT_PATH_KEY);
        argsArray.add(checkpointPath);

        Map<String, Object> params= new HashMap<>(4);
        params.put("entryClass" , jobRunConfig.getEntryClass());
        params.put("programArgsList" , argsArray);
        params.put("parallelism" ,job.getParallelism() == null ? 1 : job.getParallelism());
        if(!StringUtils.isEmpty(job.getSavepointPath())){
            params.put("savepointPath" ,job.getSavepointPath());
        }

        String jobid = jarApi.runJars(job.getFlinkColonyUrl(), params, jar.getJobKey());
        Job updateJob = new Job();
        updateJob.setId(id);
        updateJob.setJobId(jobid);
        updateJob.setStatus(Constant.JobState.RUNNING.ordinal());
        updateJob.setUpdateTime(new Date());
        jobMapper.updateByPrimaryKeySelective(updateJob);
        return true;
    }

    private String analysisJobSql(String args){
        BufferedReader br = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(args.getBytes(StandardCharsets.UTF_8)), StandardCharsets.UTF_8));
        try {
            String line;
            StringBuilder sqlBuilder=new StringBuilder();
            while ( (line = br.readLine()) != null ) {
                line = line.trim();
                if(!line.equals("")&&!line.startsWith("--")){
                    sqlBuilder.append(line);
                }
            }
            return sqlBuilder.toString();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }


    @Override
    public boolean savepoint(Integer id, Integer loginUserId) {
        Job job = jobMapper.findJobById(id);
        dataAuthorityService.checkDataAuthority(job, Constant.DataType.JOB, loginUserId);
        String triggerId = jobApi.savepoint(job.getFlinkColonyUrl(), job.getJobId());

        CheckPointInfo checkPointInfo = new CheckPointInfo(id ,triggerId);
        checkPointInfo.setJobForm(Constant.CheckJobForm.JOB.getValue());
        checkPointInfo.setPointType(Constant.CheckPointType.SAVE.getValue());
        checkPointInfo.setCreateUserId(loginUserId);
        checkPointInfo.setLocation("");
        checkPointInfoService.addCheckPointInfo(checkPointInfo);

        SyncSavePointTask syncSavePointTask = new SyncSavePointTask(checkPointInfo.getId(), job.getJobId(), triggerId, job.getFlinkColonyUrl());
        new Timer().schedule(syncSavePointTask ,Constant.DELAY_TIME);
        return true;
    }

    @Override
    public boolean stopJob(Integer id, Integer loginUserId){
        Job job = jobMapper.findJobById(id);
        dataAuthorityService.checkDataAuthority(job, Constant.DataType.JOB, loginUserId);
        String triggerId = jobApi.stopJob(job.getFlinkColonyUrl(), job.getJobId());

        CheckPointInfo checkPointInfo = new CheckPointInfo(id ,triggerId);
        checkPointInfo.setJobForm(Constant.CheckJobForm.JOB.getValue());
        checkPointInfo.setPointType(Constant.CheckPointType.SAVE.getValue());
        checkPointInfo.setCreateUserId(loginUserId);
        checkPointInfo.setLocation("");
        checkPointInfoService.addCheckPointInfo(checkPointInfo);

        jobMapper.updateJobStatusByJobId(job.getJobId(), Constant.JobState.CANCELED.ordinal());

        SyncSavePointTask syncSavePointTask = new SyncSavePointTask(checkPointInfo.getId(), job.getJobId(), triggerId, job.getFlinkColonyUrl());
        new Timer().schedule(syncSavePointTask ,Constant.DELAY_TIME);
        return true;
    }
}
