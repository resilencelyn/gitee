package cn.chongho.inf.flink.service.impl;

import cn.chongho.inf.flink.mapper.CdcJobRunConfigMapper;
import cn.chongho.inf.flink.service.CdcJobRunConfigService;
import cn.chongho.inf.flink.model.*;
import org.springframework.stereotype.Service;

import javax.annotation.Resource;


/**
 * @author ming
 */
@Service
public class CdcJobRunConfigServiceImpl implements CdcJobRunConfigService {

    @Resource
    private CdcJobRunConfigMapper cdcJobRunConfigMapper;

    @Override
    public CdcJobRunConfig selectByJobType(String jobType) {
        CdcJobRunConfig cdcJobRunConfig = new CdcJobRunConfig();
        cdcJobRunConfig.setJobType(jobType);
        return cdcJobRunConfigMapper.selectOne(cdcJobRunConfig);
    }
}
