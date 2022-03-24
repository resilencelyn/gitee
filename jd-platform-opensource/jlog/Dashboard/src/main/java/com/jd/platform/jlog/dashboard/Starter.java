package com.jd.platform.jlog.dashboard;

import com.jd.platform.jlog.dashboard.utils.TaskUtil;
import lombok.Data;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Configuration;

import javax.annotation.PostConstruct;
import java.util.concurrent.*;

/**
 * 设置组件启动及初始化
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-11-09
 */
@Configuration
@Data
public class Starter {
    /**
     * 日志
     */
    private Logger logger = LoggerFactory.getLogger(getClass());
    /**
     * fake
     */
    public static DelayQueue queue = new DelayQueue();

//    /**
//     * 打cpu开关
//     */
//    @Value("${fake.interval}")
//    private String interval;
//
//    /**
//     * 重复次数
//     */
//    @Value("${fake.repeat}")
//    private String repeat;
//
//    /**
//     * 打cpu开关
//     */
//    @Value("${fake.cpuon}")
//    private String cpuOn;

    /**
     * 定时调度线程池
     */
    private ScheduledExecutorService service = Executors.newScheduledThreadPool(8);

    @PostConstruct
    public void start() {
//        logger.info("interval", interval);
//        logger.info("repeat", repeat);
//        logger.info("cpuOn", cpuOn);

        TaskUtil taskUtil = new TaskUtil();
//        taskUtil.setRepeat(Integer.valueOf(repeat));
//        taskUtil.setCpuOn(Integer.valueOf(cpuOn));

        for (int i = 0; i < 8; i++) {
            ScheduledFuture future = service.scheduleAtFixedRate(taskUtil, 0, Integer.valueOf(1000), TimeUnit.MILLISECONDS);
            try {
                Thread.sleep(125);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        new Thread(()->{
            while (true) {
                try {
                    queue.poll(125, TimeUnit.MILLISECONDS);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

}
