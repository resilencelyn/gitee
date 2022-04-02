package cn.chongho.inf.flink.service.tasks;

import cn.chongho.inf.flink.model.Cluster;
import cn.chongho.inf.flink.model.DingTalkMsg;
import cn.chongho.inf.flink.restapi.JobApi;
import cn.chongho.inf.flink.service.ClusterService;
import cn.chongho.inf.flink.service.impl.AlertEventService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;

/**
 * 定时拉取任务更新状态
 * @author ming
 */
@Component
@EnableScheduling
@Slf4j
@ConditionalOnProperty(prefix = "alert", value = "enable", havingValue = "true")
public class ClusterStatusCheck {

    @Autowired
    private ClusterService clusterService;

    @Autowired
    private AlertEventService alertEventService;

    @Autowired
    private JobApi jobApi;

    private static Map<Integer, Long> pushTime = new HashMap<>();

    /**
     * 过期时间
     */
    private static final int EXPIRE_TIME = 15 * 60;

    @Value("${alert.pushRobotId}")
    private String robotId;

    @Scheduled(fixedRate = 60 * 1000L)
    public void doCheck(){
        log.info("sync cluster status... ");

        List<Cluster> allFlinkColonyConfig = clusterService.getAllCluster();

        for(Cluster flinkColonyConfig : allFlinkColonyConfig){
            try {
                jobApi.getOverviewInfo(flinkColonyConfig.getUrl());
            }catch (Exception e){
                String msg ="flink集群[" + flinkColonyConfig.getName() + "]状态不可用，请注意排查。";
                log.info(msg);
                DingTalkMsg dingTalkMsg = new DingTalkMsg();
                dingTalkMsg.setMessage(msg);
                dingTalkMsg.setGroupId(robotId);

                long thisTime = System.currentTimeMillis();
                Long lastTime = pushTime.get(flinkColonyConfig.getId());
                if(lastTime == null){
                    if(alertEventService.eventUpload(dingTalkMsg)){
                        pushTime.put(flinkColonyConfig.getId(), thisTime);
                    }
                }else{
                    long seconds = TimeUnit.MILLISECONDS.toSeconds(thisTime - lastTime);
                    if(seconds > EXPIRE_TIME){
                        if(alertEventService.eventUpload(dingTalkMsg)){
                            pushTime.put(flinkColonyConfig.getId(), thisTime);
                        }
                    }
                }
            }
        }
    }
}
