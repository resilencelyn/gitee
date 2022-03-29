package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.constants.Constant;
import cn.chongho.inf.flink.mapper.CheckPointInfoMapper;
import cn.chongho.inf.flink.model.CheckPointInfo;
import cn.chongho.inf.flink.service.CheckPointInfoService;
import org.apache.ibatis.session.RowBounds;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

/**
 * @author ming
 */
@Service
public class CheckPointInfoServiceImpl implements CheckPointInfoService {

    @Resource
    private CheckPointInfoMapper checkPointInfoMapper;

    @Override
    public List<CheckPointInfo> getCheckPointInfoByPage(int pageNum, int pageSize, CheckPointInfo checkPointInfo) {
        int offset = (pageNum-1) * pageSize;
        return checkPointInfoMapper.selectByRowBounds(checkPointInfo,new RowBounds(offset, pageSize));
    }

    @Override
    public int selectCount(CheckPointInfo checkPointInfo) {
        return checkPointInfoMapper.selectCount(checkPointInfo);
    }

    @Override
    public List<CheckPointInfo> getAllCheckPointInfo(CheckPointInfo checkPointInfo) {
        return checkPointInfoMapper.select(checkPointInfo);
    }

    @Override
    public boolean updateCheckPointInfo(CheckPointInfo checkPointInfo) {
        return checkPointInfoMapper.updateByPrimaryKeySelective(checkPointInfo) > 0;
    }

    @Override
    public boolean addCheckPointInfo(CheckPointInfo checkPointInfo) {
        return checkPointInfoMapper.insert(checkPointInfo) > 1;
    }

    @Override
    public CheckPointInfo getCheckPointInfoOne(CheckPointInfo checkPointInfo) {
        return checkPointInfoMapper.selectOne(checkPointInfo);
    }

    @Override
    public List<CheckPointInfo> getCheckPointInfoByJobId(Integer jobId, Integer jobForm) {
        //手动保存点返回最近5个
        CheckPointInfo checkPointInfo = new CheckPointInfo();
        checkPointInfo.setJobId(jobId);
        checkPointInfo.setJobForm(jobForm);
        checkPointInfo.setPointType(Constant.CheckPointType.SAVE.getValue());
        List<CheckPointInfo> checkPointInfos = checkPointInfoMapper.selectCheckPointInfoByJobId(checkPointInfo, 5);

        //自动保存点返回最新一个
        checkPointInfo.setPointType(Constant.CheckPointType.CHECK.getValue());
        checkPointInfos.addAll(checkPointInfoMapper.selectCheckPointInfoByJobId(checkPointInfo, 1));

        return checkPointInfos.stream().sorted(Comparator.comparing(CheckPointInfo::getId).reversed()).collect(Collectors.toList());
    }

    @Override
    public void addByBatch(List<CheckPointInfo> checkPointInfos) {
        checkPointInfoMapper.insertByBatch(checkPointInfos);
    }
}
