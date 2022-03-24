package com.jd.platform.jlog.client;


import com.alibaba.fastjson.JSON;
import com.jd.platform.jlog.client.mdc.Mdc;
import com.jd.platform.jlog.client.task.Monitor;
import com.jd.platform.jlog.client.udp.HttpSender;
import com.jd.platform.jlog.client.udp.UdpClient;
import com.jd.platform.jlog.client.udp.UdpSender;
import com.jd.platform.jlog.common.handler.TagConfig;
import com.jd.platform.jlog.core.ClientHandlerBuilder;
import com.jd.platform.jlog.core.Configurator;
import com.jd.platform.jlog.core.ConfiguratorFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


/**
 * TracerClientStarter
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-13
 */
public class TracerClientStarter {

    private final static Logger LOGGER = LoggerFactory.getLogger(TracerClientStarter.class);


    /**
     * 机房
     */
    private Mdc mdc;

    /**
     * 如果直接配置在app.properties/yaml等主配置文件
     * 可以用ConfigurationProperties直接获取有值对象
     */
    private TagConfig tagConfig;


    /**
     * TracerClientStarter
     */
    private TracerClientStarter(String appName) {
        if (appName == null) {
            throw new NullPointerException("APP_NAME cannot be null!");
        }
        Context.APP_NAME = appName;
    }

    public static class Builder {
        private String appName;
        private Mdc mdc;
        private TagConfig tagConfig;

        public Builder() {
        }

        public Builder setAppName(String appName) {
            this.appName = appName;
            return this;
        }

        public Builder setMdc(Mdc mdc) {
            this.mdc = mdc;
            return this;
        }

        public Builder setTagConfig(TagConfig tagConfig) {
            this.tagConfig = tagConfig;
            return this;
        }

        public TracerClientStarter build() {
            TracerClientStarter tracerClientStarter = new TracerClientStarter(appName);
            tracerClientStarter.tagConfig = tagConfig;
            tracerClientStarter.mdc = mdc;
            return tracerClientStarter;
        }
    }

    /**
     * 启动监听
     */
    public void startPipeline() {
        // 初始化配置
        initJLogConfig();

        Context.MDC = mdc;

        Monitor starter = new Monitor();
        starter.start();

        UdpClient udpClient = new UdpClient();
        udpClient.start();

        //开启发送
        UdpSender.uploadToWorker();

        //开启大对象http发送
        HttpSender.uploadToWorker();
    }


    /**
     * 如果未赋值，从配置器获取，底层是Properties
     */
    private void initJLogConfig(){
        LOGGER.info("从主配置获取的tagConfig:{}", JSON.toJSONString(tagConfig));
        Configurator configurator = ConfiguratorFactory.getInstance();
        ClientHandlerBuilder.buildHandler(tagConfig, configurator);
    }
}
