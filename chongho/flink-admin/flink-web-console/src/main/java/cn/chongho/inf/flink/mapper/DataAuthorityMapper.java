package cn.chongho.inf.flink.mapper;

import cn.chongho.inf.flink.model.DataAuthority;
import cn.chongho.inf.flink.model.UserDataAuthority;
import org.apache.ibatis.annotations.Param;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;


/**
 * @author ming
 */
public interface DataAuthorityMapper extends Mapper<DataAuthority> {

    List<UserDataAuthority> selectUserAuthority(@Param("dataId") Integer dataId, @Param("dataType") Integer dataType);
}
