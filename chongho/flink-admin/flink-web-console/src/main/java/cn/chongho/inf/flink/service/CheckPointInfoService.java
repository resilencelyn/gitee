package cn.chongho.inf.flink.service;



import cn.chongho.inf.flink.model.CheckPointInfo;

import java.util.List;

/**
 * @author ming
 */
public interface CheckPointInfoService {

    /**
     * 查询
     * @param pageNum
     * @param pageSize
     * @param checkPointInfo
     * @return
     */
    List<CheckPointInfo> getCheckPointInfoByPage(int pageNum, int pageSize, CheckPointInfo checkPointInfo);

    /**
     * 返回总条数
     * @param checkPointInfo
     * @return
     */
    int selectCount(CheckPointInfo checkPointInfo);

    /**
     * 查询
     * @param checkPointInfo
     * @return
     */
    List<CheckPointInfo> getAllCheckPointInfo(CheckPointInfo checkPointInfo);

    /**
     * 跟新
     * @param checkPointInfo
     * @return
     */
    boolean updateCheckPointInfo(CheckPointInfo checkPointInfo);

    /**
     * 新增
     * @param checkPointInfo
     * @return
     */
    boolean addCheckPointInfo(CheckPointInfo checkPointInfo);

    /**
     * 返回一个
     * @param checkPointInfo
     * @return
     */
    CheckPointInfo getCheckPointInfoOne(CheckPointInfo checkPointInfo);

    List<CheckPointInfo> getCheckPointInfoByJobId(Integer jobId , Integer jobForm);


    void addByBatch(List<CheckPointInfo> checkPointInfos);

    void updateByBatch(List<CheckPointInfo> checkPointInfos);
}
