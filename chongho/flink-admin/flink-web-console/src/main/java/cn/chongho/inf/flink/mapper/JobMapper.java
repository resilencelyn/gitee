package cn.chongho.inf.flink.mapper;

import cn.chongho.inf.flink.model.Job;
import org.apache.ibatis.annotations.*;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;

/**
 * @author ming
 */
public interface JobMapper extends Mapper<Job> {

    /**
     * 关联查询
     * @param offset
     * @param limit
     * @return
     */
    List<Job> selectByPage(@Param("offset") int offset, @Param("limit") int limit,@Param("job") Job job,@Param("loginUserId") Integer loginUserId);

    int countByPage(@Param("job") Job job);

    @Select("SELECT job.id ,job.job_id jobId,job.create_user_id createUserId,job.flink_colony_id flinkColonyId, fc.url flinkColonyUrl, entry_class entryClass, target_db_id targetDbId, savepoint_path savepointPath ,args,parallelism ,jar.job_key jobKey" +
            " FROM job  LEFT JOIN jar ON job.jar_id = jar.id  LEFT JOIN cluster fc ON fc.id = job.flink_colony_id " +
            "WHERE job.enable_flag = 1 AND job.id = #{ id } ")
    Job findJobById(@Param("id") Integer id);

    @Select("SELECT job.id ,job.job_id jobId, fc.url flinkColonyUrl" +
            " FROM job  LEFT JOIN cluster fc ON job.flink_colony_id = fc.id " +
            "WHERE job.enable_flag = 1 AND job.status != 0 ")
    List<Job> selectAllRuningJob();


    @Select("SELECT id  FROM job WHERE job.enable_flag = 1 AND job.entry_class = #{ entryClass } ")
    Job findJobByEntryClass(@Param("entryClass") String entryClass);

    @Update("UPDATE job  SET STATUS =#{ status }  WHERE job_id = #{ jobId } ")
    boolean updateJobStatusByJobId(@Param("jobId") String jobId , @Param("status") Integer status);

    boolean updateJobById(@Param("job") Job job);
}
