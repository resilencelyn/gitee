package com.jd.platform.jlog.core;


/**
 * @author tangbohu
 */
public interface ConfiguratorProvider {

    /**
     * 根据实现类构建配置器
     * @return 配置器
     */
    Configurator build();
}
