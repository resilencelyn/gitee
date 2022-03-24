package com.jd.platform.jlog.nacos;


import com.jd.platform.jlog.core.Configurator;
import com.jd.platform.jlog.core.ConfiguratorProvider;

public class NacosConfiguratorProvider implements ConfiguratorProvider {
    @Override
    public Configurator build() {
        return NacosConfigurator.getInstance();
    }
}
