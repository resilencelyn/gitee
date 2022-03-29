package cn.chongho.inf.flink.mapper;

import cn.chongho.inf.flink.model.Jar;
import org.apache.ibatis.annotations.Param;
import tk.mybatis.mapper.common.Mapper;

import java.util.Date;
import java.util.List;

/**
 * @author ming
 */
public interface JarMapper extends Mapper<Jar> {

    List<Jar> selectByPage(@Param("offset") int offset, @Param("limit") int limit,
                           @Param("jar") Jar jar, @Param("startTime")Date startTime, @Param("endTime")Date endTime);

    int countByPage(@Param("jar") Jar jar, @Param("startTime")Date startTime, @Param("endTime")Date endTime);
}
