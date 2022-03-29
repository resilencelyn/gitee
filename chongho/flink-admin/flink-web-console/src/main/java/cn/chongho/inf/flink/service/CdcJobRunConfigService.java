package cn.chongho.inf.flink.service;


import cn.chongho.inf.flink.model.CdcJobRunConfig;


/**
 * @author ming
 */
public interface CdcJobRunConfigService {

    CdcJobRunConfig selectByJobType(String jobType);
}
