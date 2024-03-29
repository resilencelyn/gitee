package com.dtstack.taier.scheduler.service;

import com.alibaba.fastjson.JSONObject;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.dtstack.taier.common.enums.Deleted;
import com.dtstack.taier.dao.domain.ScheduleJobExpand;
import com.dtstack.taier.dao.mapper.ScheduleJobExpandMapper;
import com.dtstack.taier.pluginapi.constrant.JobResultConstant;
import org.apache.commons.collections.CollectionUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

import java.util.Set;

/**
 * @Auther: dazhi
 * @Date: 2021/12/28 8:54 PM
 * @Email:dazhi@dtstack.com
 * @Description:
 */
@Service
public class ScheduleJobExpandService extends ServiceImpl<ScheduleJobExpandMapper, ScheduleJobExpand> {
    private static final Logger LOGGER = LoggerFactory.getLogger(ScheduleJobExpandService.class);

    /**
     * 清楚扩展表数据
     *
     * @param jobIds 需要清楚的实例id
     * @return 具体清楚的记录数
     */
    public Integer clearData(Set<String> jobIds) {
        if (CollectionUtils.isNotEmpty(jobIds)) {
            return this.baseMapper.updateLogByJobIds(jobIds, Deleted.NORMAL.getStatus(), "", "");
        }
        return 0;
    }

    public ScheduleJobExpand getByJobId(String jobId) {
        return getBaseMapper().selectOne(Wrappers.lambdaQuery(ScheduleJobExpand.class)
                .eq(ScheduleJobExpand::getJobId, jobId));
    }

    public void updateEngineLog(String jobId,String engineLog) {
        ScheduleJobExpand scheduleJobExpand = new ScheduleJobExpand();
        scheduleJobExpand.setJobId(jobId);
        scheduleJobExpand.setEngineLog(engineLog);
        getBaseMapper().update(scheduleJobExpand, Wrappers.lambdaQuery(ScheduleJobExpand.class)
                .eq(ScheduleJobExpand::getJobId, jobId));
    }

    public void updateExtraInfo(String jobId, String jobExtraInfo) {
        ScheduleJobExpand scheduleJobExpand = new ScheduleJobExpand();
        scheduleJobExpand.setJobId(jobId);
        scheduleJobExpand.setJobExtraInfo(jobExtraInfo);
        getBaseMapper().update(scheduleJobExpand, Wrappers.lambdaQuery(ScheduleJobExpand.class)
                .eq(ScheduleJobExpand::getJobId, jobId));
    }

    public void updateExtraInfoAndLog(String jobId, String jobExtraInfo, String logInfo, String engineLog) {
        ScheduleJobExpand scheduleJobExpand = new ScheduleJobExpand();
        scheduleJobExpand.setJobId(jobId);
        scheduleJobExpand.setJobExtraInfo(jobExtraInfo);
        scheduleJobExpand.setLogInfo(logInfo);
        scheduleJobExpand.setEngineLog(engineLog);
        getBaseMapper().update(scheduleJobExpand, Wrappers.lambdaQuery(ScheduleJobExpand.class)
                .eq(ScheduleJobExpand::getJobId, jobId));
    }

    public String getJobGraphJson(String jobId) {
        String jobExtraInfo =getByJobId(jobId).getJobExtraInfo();
        JSONObject jobExtraObj = JSONObject.parseObject(jobExtraInfo);
        if (null != jobExtraObj) {
            return jobExtraObj.getString(JobResultConstant.JOB_GRAPH);
        }
        return "";
    }


    public String getJobExtraInfo(String jobId) {
        return getByJobId(jobId).getJobExtraInfo();
    }
}
