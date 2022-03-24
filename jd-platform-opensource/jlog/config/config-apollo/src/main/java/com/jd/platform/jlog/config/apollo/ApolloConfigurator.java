package com.jd.platform.jlog.config.apollo;

import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.*;

import com.alibaba.fastjson.JSON;
import com.ctrip.framework.apollo.ConfigFile;
import com.ctrip.framework.apollo.core.enums.ConfigFileFormat;
import com.ctrip.framework.apollo.core.utils.StringUtils;
import com.ctrip.framework.apollo.enums.PropertyChangeType;
import com.ctrip.framework.apollo.model.ConfigChange;
import com.jd.platform.jlog.common.handler.JcProperties;
import com.jd.platform.jlog.common.handler.TagConfig;
import com.jd.platform.jlog.common.utils.CollectionUtil;
import com.jd.platform.jlog.common.utils.FastJsonUtils;
import com.jd.platform.jlog.core.*;
import com.ctrip.framework.apollo.Config;
import com.ctrip.framework.apollo.ConfigService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


import static com.ctrip.framework.apollo.core.ApolloClientSystemConsts.APP_ID;
import static com.jd.platform.jlog.common.utils.ConfigUtil.invoke;
import static com.jd.platform.jlog.config.apollo.ApolloConstant.*;
import static com.jd.platform.jlog.core.Constant.DEFAULT_NAMESPACE;


/**
 * @author tangbohu
 * @version 1.0.0
 * @Description todo env and cluster
 * @ClassName ApolloConfigurator.java
 * @createTime 2022年02月21日 21:21:00
 */
public class ApolloConfigurator implements Configurator {


    private static final Logger LOGGER = LoggerFactory.getLogger(ApolloConfigurator.class);


    private static final Configurator FILE_CONFIG = ConfiguratorFactory.base;


    /**
     * 里面有resourceProperties 和 configProperties
     */
    private static volatile Config config;


    private static volatile ApolloConfigurator instance;

    private ApolloConfigurator() {
        loadApolloServerConfig();
        if (config == null) {
            synchronized (ApolloConfigurator.class) {
                if (config == null) {
                    // apollo的监听是按照namespace维度
                    config = ConfigService.getConfig(DEFAULT_NAMESPACE);
                    ApolloListener apolloListener = new ApolloListener();
                    config.addChangeListener(changeEvent -> {
                        LOGGER.info("Apollo收到事件变更, keys={}", changeEvent.changedKeys());
                        for (String key : changeEvent.changedKeys()) {
                            ConfigChange change = changeEvent.getChange(key);
                            ConfigChangeEvent event = new ConfigChangeEvent(key, change.getNamespace(), change.getOldValue(), change.getNewValue(), getChangeType(change.getChangeType()));
                            apolloListener.onProcessEvent(event);
                        }
                    });
                }
            }
        }
        LOGGER.info("Apollo配置器构建完成");
    }


    public static ApolloConfigurator getInstance() {
        if (instance == null) {
            synchronized (ApolloConfigurator.class) {
                if (instance == null) {
                    instance = new ApolloConfigurator();
                }
            }
        }
        return instance;
    }

    @Override
    public String getString(String key) {
        return config.getProperty(key, null);
    }

    @Override
    public Long getLong(String key) {
        return config.getLongProperty(key,null);
    }

    @Override
    public List<String> getList(String key) {
        return FastJsonUtils.toList(config.getProperty(key,""), String.class) ;
    }

    @Override
    public <T> T getObject(String key, Class<T> clz) {
        return FastJsonUtils.toBean(config.getProperty(key,""), clz);
    }

    @Override
    public boolean putConfig(String key, String content) {
        return false;
    }

    @Override
    public boolean putConfig(String dataId, String content, long timeoutMills) {
        return false;
    }


    @Override
    public String getType() {
        return "apollo";
    }


    private ConfigChangeType getChangeType(PropertyChangeType changeType) {
        switch (changeType) {
            case ADDED:
                return ConfigChangeType.ADD;
            case DELETED:
                return ConfigChangeType.DELETE;
            default:
                return ConfigChangeType.MODIFY;
        }
    }


    private void loadApolloServerConfig() {

        Properties properties = System.getProperties();
        if (!properties.containsKey(PROP_APP_ID)) {
            System.setProperty(PROP_APP_ID, FILE_CONFIG.getString(APP_ID));
        }
        if (!properties.containsKey(PROP_APOLLO_META)) {
            System.setProperty(PROP_APOLLO_META, FILE_CONFIG.getString(APOLLO_META));
        }
        if (!properties.containsKey(PROP_APOLLO_SECRET)) {
            String secretKey = FILE_CONFIG.getString(APOLLO_SECRET);
            if (!StringUtils.isBlank(secretKey)) {
                System.setProperty(PROP_APOLLO_SECRET, secretKey);
            }
        }
        if (!properties.containsKey(APOLLO_CLUSTER)) {
            if (!StringUtils.isBlank(FILE_CONFIG.getString(APOLLO_CLUSTER))) {
                System.setProperty(PROP_APOLLO_CLUSTER, FILE_CONFIG.getString(APOLLO_CLUSTER));
            }
        }
        if (!properties.containsKey(APOLLO_CONFIG_SERVICE)) {
            System.setProperty(PROP_APOLLO_CONFIG_SERVICE, FILE_CONFIG.getString(APOLLO_CONFIG_SERVICE));
        }
    }

}
