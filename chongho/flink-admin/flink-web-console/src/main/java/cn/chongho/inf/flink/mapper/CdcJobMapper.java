package cn.chongho.inf.flink.mapper;

import cn.chongho.inf.flink.model.CdcJob;
import org.apache.ibatis.annotations.*;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;

/**
 * @author ming
 */
public interface CdcJobMapper extends Mapper<CdcJob> {

    List<CdcJob> selectByPage(@Param("offset") int offset, @Param("limit") int limit,@Param("cdcJob") CdcJob cdcJob, @Param("loginUserId") Integer loginUserId);

    int countByPage(@Param("cdcJob") CdcJob cdcJob);


    CdcJob selectById(@Param("jobId") int jobId);

    @Select("SELECT cdc_job.id ,cdc_job.job_id jobId, c.url flinkColonyUrl" +
            " FROM cdc_job  LEFT JOIN cluster c ON cdc_job.flink_colony_id = c.id " +
            "WHERE cdc_job.enable_flag = 1 AND cdc_job.status != 0 ")
    List<CdcJob> selectAllRunningJob();

    @Update("UPDATE cdc_job  SET status =#{ status }  WHERE job_id = #{ jobId } ")
    boolean updateJobStatusByJobId(@Param("jobId") String jobId , @Param("status") Integer status);

    boolean updateCdcJobById(@Param("cdcJob") CdcJob cdcJob);
}
