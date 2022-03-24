package com.jd.platform.jlog.etcd;


import com.jd.platform.jlog.core.Configurator;
import com.jd.platform.jlog.core.ConfiguratorProvider;

/**
 * @author tangbohu
 */
public class EtcdConfigurationProvider implements ConfiguratorProvider {
    @Override
    public Configurator build() {
        return EtcdConfigurator.getInstance();
    }
}
