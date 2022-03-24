package com.jd.platform.jlog.worker;

import com.jd.platform.jlog.worker.config.CenterStarter;
import com.jd.platform.jlog.worker.store.TracerLogToDbStore;
import com.jd.platform.jlog.worker.store.TracerModelToDbStore;
import com.jd.platform.jlog.worker.udp.UdpServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import javax.annotation.Resource;

/**
 * 设置组件启动及初始化
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-12
 */
@Component
public class Starter {
    /**
     * 日志
     */
    private Logger logger = LoggerFactory.getLogger(getClass());
    /**
     * 数据暂存和入库
     */
    @Resource
    private TracerModelToDbStore tracerModelToDbStore;
    /**
     * 日志暂存和入库
     */
    @Resource
    private TracerLogToDbStore tracerLogToDbStore;
    /**
     * udp server
     */
    @Resource
    private UdpServer udpServer;
    /**
     * etcd启动器
     */
    @Resource
    private CenterStarter centerStarter;

    @PostConstruct
    public void start() {

        logger.info("netty server监听已开启");
        new Thread(() -> {
            //开启服务监听
            udpServer.startServer();
        }).start();

        //入库
        tracerModelToDbStore.beginIntoDb();
        tracerLogToDbStore.beginIntoDb();

        //上报自己ip到配置中心
        centerStarter.uploadSelfInfo();
    }

}
