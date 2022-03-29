package cn.chongho.inf.flink.mapper;

import cn.chongho.inf.flink.model.DbSource;
import org.apache.ibatis.annotations.Param;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;

/**
 * @author ming
 */
public interface DbSourceMapper extends Mapper<DbSource> {

    List<DbSource> selectByPage(@Param("offset") int offset, @Param("limit") int limit,@Param("dbSource") DbSource dbSource);

    int countByPage(@Param("dbSource") DbSource dbSource);
}
