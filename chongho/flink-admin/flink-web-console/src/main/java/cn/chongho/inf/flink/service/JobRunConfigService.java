package cn.chongho.inf.flink.service;


import cn.chongho.inf.flink.model.JobRunConfig;

import java.util.List;


/**
 * @author ming
 */
public interface JobRunConfigService {

    JobRunConfig selectByJobType(String jobType);

    JobRunConfig selectJobById(Integer id);

    List<JobRunConfig> selectAll();

    boolean update(JobRunConfig jobRunConfig);
}
