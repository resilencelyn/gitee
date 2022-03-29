package cn.chongho.inf.flink.service.tasks;

import cn.chongho.inf.flink.model.CheckPointInfo;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.CheckPointInfoService;
import cn.chongho.inf.flink.utils.ApplicationContextUtil;
import cn.chongho.inf.flink.utils.StringUtils;

import java.util.TimerTask;

/**
 * @author ming
 */
public class SyncSavePointTask extends TimerTask {

    private Integer checkpointId;

    private String  jobId;

    private String triggerId;

    private String flinkUrl;

    public SyncSavePointTask(Integer checkpointId, String jobId, String triggerId, String flinkUrl) {
        this.checkpointId = checkpointId;
        this.jobId = jobId;
        this.triggerId = triggerId;
        this.flinkUrl = flinkUrl;
    }

    @Override
    public void run() {

        JobApi jobApi = ApplicationContextUtil.getBean(JobApi.class);

        CheckPointInfoService checkPointInfoService = ApplicationContextUtil.getBean(CheckPointInfoService.class);

        String savepointLocation = jobApi.getSavepointLocation(flinkUrl, jobId, triggerId);

        if(StringUtils.isEmpty(savepointLocation)){
            return;
        }
        CheckPointInfo checkPointInfo = new CheckPointInfo();
        checkPointInfo.setId(checkpointId);
        checkPointInfo.setLocation(savepointLocation);
        checkPointInfoService.updateCheckPointInfo(checkPointInfo);
    }
}
