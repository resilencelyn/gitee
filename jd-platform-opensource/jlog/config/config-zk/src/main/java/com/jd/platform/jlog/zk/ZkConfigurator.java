package com.jd.platform.jlog.zk;

import java.io.ByteArrayInputStream;
import java.util.List;
import com.alibaba.fastjson.JSON;
import com.jd.platform.jlog.common.handler.JcProperties;
import com.jd.platform.jlog.common.utils.StringUtil;
import com.jd.platform.jlog.core.*;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.retry.ExponentialBackoffRetry;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static com.jd.platform.jlog.common.utils.ConfigUtil.formatConfigByte;
import static com.jd.platform.jlog.core.Constant.DEFAULT_TIMEOUT;
import static com.jd.platform.jlog.core.Constant.DEFAULT_NAMESPACE;
import static com.jd.platform.jlog.core.Constant.SERVER_ADDR_KEY;


/**
 * @author tangbohu
 */
public class ZkConfigurator implements Configurator {

    private final static Logger LOGGER = LoggerFactory.getLogger(ZkConfigurator.class);

    private static final Configurator FILE_CONFIG = ConfiguratorFactory.base;

    static volatile CuratorFramework zkClient;

    static volatile JcProperties PROPERTIES = new JcProperties();

    private static final String DEFAULT_CONFIG_PATH = "/jLog.properties";


    public ZkConfigurator() throws Exception {
        if (zkClient == null) {
            synchronized (ZkConfigurator.class) {
                zkClient = CuratorFrameworkFactory.builder().connectString(FILE_CONFIG.getString(SERVER_ADDR_KEY))
                        // 连接超时时间
                        .sessionTimeoutMs(6000)
                        // 会话超时时间
                        .connectionTimeoutMs(2000)
                        .namespace(DEFAULT_NAMESPACE)
                        // 刚开始重试间隔为1秒，之后重试间隔逐渐增加，最多重试不超过三次
                        .retryPolicy(new ExponentialBackoffRetry(1000, 3))
                        .build();
                zkClient.start();
            }

            loadZkData();
            new ZkListener(DEFAULT_CONFIG_PATH);
            LOGGER.info("初始化ZK,载入ZK数据完成 props:{}", JSON.toJSONString(PROPERTIES));
        }
    }


    @Override
    public String getString(String key) {
        return  PROPERTIES.getString(key);
    }

    @Override
    public Long getLong(String key) {
        return PROPERTIES.getLong(key);
    }

    @Override
    public List<String> getList(String key) {
        return PROPERTIES.getStrList(key);
    }

    @Override
    public <T> T getObject(String key, Class<T> clz) {
        return PROPERTIES.getBean(key, clz);
    }


    @Override
    public boolean putConfig(String key, String content) {
        return putConfig(key, content, DEFAULT_TIMEOUT);
    }


    @Override
    public boolean putConfig(String key, String content, long timeoutMills) {
        if(StringUtil.isEmpty(key) || StringUtil.isEmpty(content)){
            return false;
        }
        PROPERTIES.setProperty(key, content);
        try {
            zkClient.setData().forPath(DEFAULT_CONFIG_PATH, formatConfigByte(PROPERTIES));
        } catch (Exception e) {
            return false;
        }
        return true;
    }



    @Override
    public String getType() {
        return "zk";
    }



    static void loadZkData() throws Exception {

        byte[] bt = zkClient.getData().forPath(DEFAULT_CONFIG_PATH);
        if (bt != null && bt.length > 0){
            ByteArrayInputStream bArray = new ByteArrayInputStream(bt);
            PROPERTIES.load(bArray);
        }
        LOGGER.info("# loadZkData # PROPERTIES:{}",JSON.toJSONString(PROPERTIES));
    }

}
