package com.jd.platform.jlog.config.apollo;

import com.jd.platform.jlog.core.Configurator;
import com.jd.platform.jlog.core.ConfiguratorProvider;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName ApolloConfiguratorProvider.java
 * @createTime 2022年02月21日 21:26:00
 */
public class ApolloConfiguratorProvider implements ConfiguratorProvider {
    @Override
    public Configurator build() {
        return ApolloConfigurator.getInstance();
    }
}

