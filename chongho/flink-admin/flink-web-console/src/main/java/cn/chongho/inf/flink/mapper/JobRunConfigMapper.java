package cn.chongho.inf.flink.mapper;

import cn.chongho.inf.flink.model.JobRunConfig;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;


/**
 * @author ming
 */
public interface JobRunConfigMapper extends Mapper<JobRunConfig> {

    List<JobRunConfig> selectAllConfig();
}
