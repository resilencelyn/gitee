package com.jd.platform.jlog.nacos;

import java.io.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

import com.alibaba.fastjson.JSON;
import com.alibaba.nacos.api.NacosFactory;
import com.alibaba.nacos.api.config.ConfigService;
import com.alibaba.nacos.api.exception.NacosException;

import com.alibaba.nacos.common.utils.StringUtils;
import com.jd.platform.jlog.common.handler.JcProperties;
import com.jd.platform.jlog.common.utils.StringUtil;
import com.jd.platform.jlog.core.Configurator;
import com.jd.platform.jlog.core.ConfiguratorFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static com.jd.platform.jlog.common.utils.ConfigUtil.formatConfigStr;
import static com.jd.platform.jlog.core.Constant.*;



/**
 * @author tangbohu
 */
public class NacosConfigurator implements Configurator {

    private static final Logger LOGGER = LoggerFactory.getLogger(NacosConfigurator.class);

    private static final Configurator FILE_CONFIG = ConfiguratorFactory.base;

    private static volatile ConfigService configService;

    static volatile JcProperties PROPERTIES = new JcProperties();

    static final String JLOG_GROUP = "JLOG_GROUP";

    static final String DEFAULT_DATA_ID = "jLog.properties";

    static NacosListener NACOSLISTENER = new NacosListener();

    private static volatile NacosConfigurator instance;


    public static NacosConfigurator getInstance() {
        if (instance == null) {
            synchronized (NacosConfigurator.class) {
                if (instance == null) {
                    instance = new NacosConfigurator();
                }
            }
        }
        return instance;
    }


    private NacosConfigurator() {
        if (configService == null) {
            try {
                configService = NacosFactory.createConfigService(getConfigProperties());
                String config = configService.getConfig(DEFAULT_DATA_ID, JLOG_GROUP, DEFAULT_TIMEOUT);
                LOGGER.info("从NaCos获取配置进行初始化 config = {}", config);
                if (StringUtils.isNotBlank(config)) {
                    PROPERTIES.load(new StringReader(config));
                    LOGGER.info("初始化本地缓存 props:{} ", JSON.toJSONString(PROPERTIES));
                    configService.addListener(DEFAULT_DATA_ID, JLOG_GROUP, NACOSLISTENER);
                }
            } catch (NacosException | IOException e) {
                throw new RuntimeException(e);
            }
        }
    }


    @Override
    public String getString(String key) {
        return PROPERTIES.getString(key);
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
        return  putConfig(key, content, DEFAULT_TIMEOUT);
    }


    @Override
    public boolean putConfig(String key, String content, long timeoutMills) {
        boolean result = false;
        if(StringUtil.isEmpty(key)){
            return false;
        }
        try {
            if (!PROPERTIES.isEmpty()) {
                PROPERTIES.setProperty(key, content);
                result = configService.publishConfig(DEFAULT_DATA_ID, JLOG_GROUP, formatConfigStr(PROPERTIES));
            } else {
                result = configService.publishConfig(DEFAULT_DATA_ID, JLOG_GROUP, content);
            }
        } catch (NacosException ex) {
            LOGGER.error(ex.getErrMsg());
        }
        return result;
    }



    private static Properties getConfigProperties() {
        Properties properties = new Properties();
        String address = FILE_CONFIG.getString(SERVER_ADDR_KEY);
        if (address != null) {
            properties.setProperty(SERVER_ADDR_KEY, address);
        }

        properties.setProperty(NAMESPACE_KEY, DEFAULT_NAMESPACE);
        return properties;
    }



    @Override
    public String getType() {
        return "nacos";
    }


}
