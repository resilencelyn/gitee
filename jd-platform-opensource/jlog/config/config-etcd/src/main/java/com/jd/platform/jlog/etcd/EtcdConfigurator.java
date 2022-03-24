package com.jd.platform.jlog.etcd;


import java.io.IOException;
import java.io.StringReader;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

import com.alibaba.fastjson.JSON;
import com.google.protobuf.ByteString;
import com.ibm.etcd.api.KeyValue;
import com.ibm.etcd.api.RangeResponse;
import com.ibm.etcd.client.EtcdClient;
import com.jd.platform.jlog.common.handler.JcProperties;
import com.jd.platform.jlog.common.utils.CollectionUtil;
import com.jd.platform.jlog.common.utils.StringUtil;
import com.jd.platform.jlog.core.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static com.jd.platform.jlog.common.utils.ConfigUtil.formatConfigByte;
import static com.jd.platform.jlog.common.utils.ConfigUtil.formatConfigStr;
import static com.jd.platform.jlog.core.Constant.DEFAULT_TIMEOUT;
import static com.jd.platform.jlog.core.Constant.SERVER_ADDR_KEY;

/**
 * @author tangbohu
 * @version 1.0.0
 * @desc 参考log4J
 * @ClassName EtcdConfigurator.java
 * @createTime 2022年02月21日 21:46:00
 */
public class EtcdConfigurator implements Configurator {

    private static final Logger LOGGER = LoggerFactory.getLogger(EtcdConfigurator.class);

    private static volatile EtcdConfigurator instance;

    static volatile EtcdClient client;

    private static final Configurator FILE_CONFIG = ConfiguratorFactory.base;

    static final String ROOT = "/jLog";

    static final String PROPERTIES_PATH = "/jLog/jLog.properties";

    static JcProperties PROPERTIES = new JcProperties();

    private EtcdConfigurator() {
        LOGGER.info("开始构建etcd客户端, serverAddr:{}",FILE_CONFIG.getString(SERVER_ADDR_KEY));
        client = EtcdClient.forEndpoints(FILE_CONFIG.getString(SERVER_ADDR_KEY)).withPlainText().build();
        RangeResponse rangeResponse = client.getKvClient().get(ByteString.copyFromUtf8(PROPERTIES_PATH)).sync();
        List<KeyValue> keyValues = rangeResponse.getKvsList();
        if (CollectionUtil.isEmpty(keyValues)) {
            return;
        }
        String val = keyValues.get(0).getValue().toStringUtf8();
        if(StringUtil.isNotBlank(val)){
            try {
                PROPERTIES.load(new StringReader(val));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        LOGGER.info("初始化etcd配置", JSON.toJSONString(PROPERTIES));
        new EtcdListener().onProcessEvent(new ConfigChangeEvent());
    }


    public static EtcdConfigurator getInstance() {
        if (instance == null) {
            synchronized (EtcdConfigurator.class) {
                if (instance == null) {
                    instance = new EtcdConfigurator();
                }
            }
        }
        return instance;
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
        client.getKvClient().put(ByteString.copyFromUtf8(PROPERTIES_PATH), ByteString.copyFromUtf8(formatConfigStr(PROPERTIES))).sync();
        return true;
    }


    @Override
    public String getType() {
        return "etcd";
    }

}
