package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.mapper.JobRunConfigMapper;
import cn.chongho.inf.flink.service.JobRunConfigService;
import cn.chongho.inf.flink.model.*;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.List;


/**
 * @author ming
 */
@Service
public class JobRunConfigServiceImpl implements JobRunConfigService {

    @Resource
    private JobRunConfigMapper jobRunConfigMapper;

    @Override
    public JobRunConfig selectByJobType(String jobType) {
        JobRunConfig jobRunConfig = new JobRunConfig();
        jobRunConfig.setJobType(jobType);
        return jobRunConfigMapper.selectOne(jobRunConfig);
    }

    @Override
    public JobRunConfig selectJobById(Integer id) {
        return jobRunConfigMapper.selectByPrimaryKey(id);
    }

    @Override
    public List<JobRunConfig> selectAll() {
        return jobRunConfigMapper.selectAllConfig();
    }

    @Override
    public boolean update(JobRunConfig jobRunConfig) {
        if(jobRunConfig.getId() == null){
            return jobRunConfigMapper.insertSelective(jobRunConfig) > 0;
        }else{
            return jobRunConfigMapper.updateByPrimaryKeySelective(jobRunConfig) > 0;
        }
    }
}
