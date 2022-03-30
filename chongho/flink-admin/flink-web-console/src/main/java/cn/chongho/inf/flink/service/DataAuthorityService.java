package cn.chongho.inf.flink.service;


import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.model.BaseJob;
import cn.chongho.inf.flink.model.DataAuthority;
import cn.chongho.inf.flink.model.UserDataAuthority;

import java.util.List;

/**
 * @author ming
 */
public interface DataAuthorityService {

    int selectCount(DataAuthority dataAuthority);

    List<DataAuthority> selectByPage(int page, int pageSize, DataAuthority dataAuthority);

    boolean delete(Integer id, Integer loginUserId);

    boolean update(Integer dataType, Integer dataId, String userIdsStr);

    List<UserDataAuthority> selectAllUser(Integer jobId, Integer dataType);

    void checkDataAuthority(BaseJob baseJob, Constant.DataType dataType, Integer loginUserId);
}
