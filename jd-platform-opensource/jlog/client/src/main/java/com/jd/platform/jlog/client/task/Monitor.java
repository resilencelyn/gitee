package com.jd.platform.jlog.client.task;


import com.jd.platform.jlog.client.worker.WorkerInfoHolder;
import com.jd.platform.jlog.core.Configurator;
import com.jd.platform.jlog.core.ConfiguratorFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName Watchdog.java
 * @Description TODO
 * @createTime 2022年02月12日 10:20:00
 */
public class Monitor {

    private final static Logger LOGGER = LoggerFactory.getLogger(Monitor.class);

    /**
     * 开始获取workerIp地址并保存</>
     * 监听workerIp地址变化
     */
    public void start() {
        fetchWorkerInfo();
    }

    /**
     * 每隔30秒拉取worker信息
     */
    private void fetchWorkerInfo() {
        ScheduledExecutorService scheduledExecutorService = Executors.newSingleThreadScheduledExecutor();
        //开启拉取etcd的worker信息，如果拉取失败，则定时继续拉取
        scheduledExecutorService.scheduleAtFixedRate(() -> {
            LOGGER.info("trying to connect to config center and fetch worker info");
            try {
                fetch();
            } catch (Exception e) {
                e.printStackTrace();
            }

        }, 0, 30, TimeUnit.SECONDS);
    }

    /**
     * 从配置中心获取worker的ip集合
     */
    private void fetch() throws Exception {
        Configurator configurator = ConfiguratorFactory.getInstance();
        //获取所有worker的ip
        List<String> addresses;
        try {
            //如果设置了机房属性，则拉取同机房的worker。如果同机房没worker，则拉取所有
            addresses = configurator.getList("workers");
            //全是空，给个警告
            if (addresses == null || addresses.size() == 0) {
                LOGGER.warn("very important warn !!! workers ip info is null!!!");
                return;
            }
            //将对应的worker保存下来
            WorkerInfoHolder.mergeAndConnectNew(addresses);
        } catch (Exception ex) {
            LOGGER.error("config server connected fail. Check the config address!!!");
        }

    }

}
