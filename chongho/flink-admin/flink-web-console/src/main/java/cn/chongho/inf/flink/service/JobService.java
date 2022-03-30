package cn.chongho.inf.flink.service;

import cn.chongho.inf.flink.model.Job;

import java.util.List;

public interface JobService {

    List<Job> selectAll(Job job);

    List<Job> selectEnabledAndRunJob();

    int selectCount(Job job);

    List<Job> selectByPage(int page, int pageSize,Job job, Integer loginUserId);

    Job selectById(Integer id);

    boolean delete(Integer id, Integer loginUserId);

    boolean update(Job job);

    boolean runJob(Integer id, Integer loginUserId);

    boolean savepoint(Integer id, Integer loginUserId);

    boolean stopJob(Integer id, Integer loginUserId);
}
