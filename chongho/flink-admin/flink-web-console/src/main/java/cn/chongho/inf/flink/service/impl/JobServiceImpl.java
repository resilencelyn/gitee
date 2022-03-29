package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.mapper.JobMapper;
import cn.chongho.inf.flink.model.CheckPointInfo;
import cn.chongho.inf.flink.model.DbSource;
import cn.chongho.inf.flink.model.Job;
import cn.chongho.inf.flink.restapi.JarApi;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.CheckPointInfoService;
import cn.chongho.inf.flink.service.DbSourceService;
import cn.chongho.inf.flink.service.JobService;
import cn.chongho.inf.flink.service.tasks.SyncSavePointTask;
import cn.chongho.inf.flink.utils.DesUtils;
import cn.chongho.inf.flink.utils.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.*;

/**
 * @author ming
 */
@Service
public class JobServiceImpl implements JobService {

    /**
     * 数据库地址参数key
     */
    private static final String HOSTNAME_KEY = "--db.hostname";

    /**
     * 数据库参数key
     */
    private static final String DB_KEY = "--db.name";

    /**
     * 数据库用户名参数key
     */
    private static final String DB_USERNAME_KEY = "--db.username";

    /**
     * 数据库密码参数key
     */
    private static final String DB_PASSWORD_KEY = "--db.password";

    /**
     * 保存点路径
     */
    private static final String CHECK_POINT_PATH_KEY = "--checkpoint.path";

    @Value("${sys.checkpoint-path}")
    private String checkpointPath;

    @Value("${sys.des.secretkey}")
    private String desSecretkey;

    @Value("${sys.des.offset}")
    private String desOffset;

    @Resource
    private JobMapper jobMapper;

    @Resource
    private JarApi jarApi;

    @Resource
    private JobApi jobApi;

    @Autowired
    private CheckPointInfoService checkPointInfoService;

    @Autowired
    private DbSourceService dbSourceService;

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
    public List<Job> selectByPage(int page, int pageSize ,Job job) {
        int offset = (page-1) * pageSize;
        return jobMapper.selectByPage(offset, pageSize,job);
    }

    @Override
    public Job selectById(Integer id) {
        return jobMapper.selectByPrimaryKey(id);
    }

    @Override
    public boolean delete(Integer id , Integer loginUserId) {

        Job dbJob = jobMapper.selectByPrimaryKey(id);
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
       /* Job jobByEntryClass = jobMapper.findJobByEntryClass(job.getEntryClass());
        if(!(jobByEntryClass == null || jobByEntryClass.getId().equals(job.getId()))){
            return false;
        }*/
        job.setParallelism(job.getParallelism() == null ? 1 : job.getParallelism());
        if (job.getId() != null) {
            return jobMapper.updateJobById(job);
        } else  {
            job.setEnableFlag(Constant.EnableFlag.ENABLE.ordinal());
            job.setCreateTime(new Date());
            job.setCreateUserId(job.getUpdateUserId());
            job.setStatus(Constant.EnableFlag.NOT.ordinal());
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

        List<String> argsArray = getDbParameter(job.getTargetDbId());
        if(!StringUtils.isEmpty(job.getArgs())){
            argsArray.addAll(Arrays.asList(job.getArgs().split(" ")));
        }
        /**
         * 增加checkpoint路径
         */
        argsArray.add(CHECK_POINT_PATH_KEY);
        argsArray.add(checkpointPath);

        Map<String, Object> params= new HashMap<>(4);
        params.put("entryClass" ,job.getEntryClass());
        params.put("programArgsList" , argsArray);
        params.put("parallelism" ,job.getParallelism() == null ? 1 : job.getParallelism());
        if(!StringUtils.isEmpty(job.getSavepointPath())){
            params.put("savepointPath" ,job.getSavepointPath());
        }

        String jobid = jarApi.runJars(job.getFlinkColonyUrl(), params, job.getJobKey());
        Job updateJob = new Job();
        updateJob.setId(id);
        updateJob.setJobId(jobid);
        updateJob.setStatus(Constant.JobState.RUNNING.ordinal());
        updateJob.setUpdateTime(new Date());
        jobMapper.updateByPrimaryKeySelective(updateJob);
        return true;
    }

    /**
     * 根据所选的数据库增加数据库参数
     * @param dbId
     * @return
     */
    private List<String> getDbParameter(Integer dbId){
        if(dbId == null){
            return new ArrayList<>();
        }
        DbSource dbSource = dbSourceService.getDbSourceById(dbId);
        if(dbSource == null){
            return new ArrayList<>();
        }
        String url = dbSource.getUrl();
        url = url.split("//")[1];
        url = url.split(":")[0];

        String dbUsernameAfterDecrypt = DesUtils.decrypt(desSecretkey, desOffset, dbSource.getUsername());
        String dbPasswordAfterDecrypt = DesUtils.decrypt(desSecretkey, desOffset, dbSource.getPassword());

        List<String> dbParameters = new ArrayList<>(4);
        dbParameters.add(HOSTNAME_KEY);
        dbParameters.add(url);

        dbParameters.add(DB_KEY);
        dbParameters.add(dbSource.getName());

        dbParameters.add(DB_USERNAME_KEY);
        dbParameters.add(dbUsernameAfterDecrypt);

        dbParameters.add(DB_PASSWORD_KEY);
        dbParameters.add(dbPasswordAfterDecrypt);

        return dbParameters;
    }

    @Override
    public boolean savepoint(Integer id, Integer loginUserId) {
        Job job = jobMapper.findJobById(id);

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

}
