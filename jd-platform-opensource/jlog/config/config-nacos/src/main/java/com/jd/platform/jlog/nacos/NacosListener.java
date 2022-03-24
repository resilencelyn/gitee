package com.jd.platform.jlog.nacos;

import com.alibaba.nacos.api.NacosFactory;
import com.alibaba.nacos.api.config.ConfigService;
import com.alibaba.nacos.api.config.listener.AbstractSharedListener;
import com.alibaba.nacos.api.exception.NacosException;
import com.alibaba.nacos.api.naming.listener.Event;
import com.alibaba.nacos.api.naming.listener.EventListener;
import com.alibaba.nacos.common.utils.StringUtils;
import com.jd.platform.jlog.common.handler.JcProperties;
import com.jd.platform.jlog.common.utils.CollectionUtil;
import com.jd.platform.jlog.core.ClientHandlerBuilder;
import com.jd.platform.jlog.core.ConfigChangeEvent;
import com.jd.platform.jlog.core.ConfigChangeListener;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.*;
import java.util.Properties;
import java.util.Set;


import static com.jd.platform.jlog.core.Constant.*;
import static com.jd.platform.jlog.nacos.NacosConfigurator.*;


/**
 * @author tangbohu
 */
public class NacosListener extends AbstractSharedListener implements ConfigChangeListener, EventListener {


    private static final Logger LOGGER = LoggerFactory.getLogger(NacosListener.class);


    public NacosListener() {}

    @Override
    public void innerReceive(String dataId, String group, String configInfo) {
        LOGGER.debug("configInfo:{}", configInfo);
        ConfigChangeEvent event = new ConfigChangeEvent().setKey(dataId).setNewValue(configInfo).setNamespace(group);
        this.onProcessEvent(event);
    }



    @Override
    public void onChangeEvent(ConfigChangeEvent event) {
        LOGGER.debug("通用[配置]变更事件 event:{}",event);
        JcProperties props = new JcProperties();
        if (StringUtils.isNotBlank(event.getNewValue())) {
            try {
                props.load(new StringReader(event.getNewValue()));
            } catch (IOException e) {
                return;
            }
        }
        Set<String> diffKeys = CollectionUtil.diffKeys(props, PROPERTIES);
        if(!diffKeys.isEmpty()){
            PROPERTIES = props;
            for (String diffKey : diffKeys) {
                LOGGER.warn("NACOS {} 配置变更 key={} 变更事件:{}", DEFAULT_DATA_ID, diffKey,
                        String.valueOf(props.get(diffKey)),
                        String.valueOf(PROPERTIES.get(diffKey)));
            }
            ClientHandlerBuilder.refresh();
        }
    }


    @Override
    public void onEvent(Event event) {
        LOGGER.info("通用[服务]事件 event:{}",event);
    }

}
