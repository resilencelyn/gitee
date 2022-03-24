package com.jd.platform.jlog.worker.config;

import com.alibaba.fastjson.JSON;
import com.jd.platform.jlog.common.constant.Constant;
import com.jd.platform.jlog.common.utils.IpUtils;
import com.jd.platform.jlog.core.Configurator;
import com.jd.platform.jlog.core.ConfiguratorFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * 跟配置中心的通信
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-12
 */
@Component
public class CenterStarter {

    /**
     * 上报自己的ip到配置中心
     */
    public void uploadSelfInfo() {
        //开启上传worker信息
        Configurator config = ConfiguratorFactory.getInstance();
        ScheduledExecutorService scheduledExecutorService = Executors.newSingleThreadScheduledExecutor();
        scheduledExecutorService.scheduleAtFixedRate(() -> {

            try {
                List<String> list = config.getList("workers");
                if(!list.contains(buildKey())){
                    list.add(buildValue());
                    config.putConfig("workers", JSON.toJSONString(list));
                }
            } catch (Exception e) {
                //do nothing
                e.printStackTrace();
            }

        }, 0, 5, TimeUnit.SECONDS);
    }

    /**
     * 在配置中心存放的key
     */
    private String buildKey() {
        return IpUtils.getHostName();
    }

    /**
     * 在配置中心存放的value
     */
    private String buildValue() {
        String ip = IpUtils.getIp();
        return ip + Constant.SPLITER + Constant.NETTY_PORT;
    }

}
