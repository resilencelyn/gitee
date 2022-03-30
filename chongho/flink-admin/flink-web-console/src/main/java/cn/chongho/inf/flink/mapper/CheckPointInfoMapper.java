package cn.chongho.inf.flink.mapper;

import cn.chongho.inf.flink.model.CheckPointInfo;
import org.apache.ibatis.annotations.Param;
import tk.mybatis.mapper.common.Mapper;

import java.util.List;

/**
 * @author ming
 */
public interface CheckPointInfoMapper extends Mapper<CheckPointInfo> {

    List<CheckPointInfo> selectCheckPointInfoByJobId(@Param("checkPointInfo") CheckPointInfo checkPointInfo, @Param("limit") Integer limit);

    void insertByBatch(List<CheckPointInfo> checkPointInfos);

    void updateBatch(List<CheckPointInfo> checkPointInfos);
}
