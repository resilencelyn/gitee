package com.jd.platform.jlog.core;

import java.util.List;

/**
 * @author tangbohu
 * @version 1.0.0
 * @desc 参考log4J
 * @ClassName Configurator.java
 * @createTime 2022年02月15日 17:06:00
 */
public interface Configurator {

    /**
     * 获取string配置
     * @param key key
     * @return val
     */
    String getString(String key);


    /**
     * 获取LONG配置
     * @param key key
     * @return val
     */
    Long getLong(String key);

    /**
     * 获取LIST类型配置
     * @param key key
     * @return val
     */
    List<String> getList(String key);

    /**
     * 获取实体类型配置
     * @param key key
     * @return val
     */
    <T> T getObject(String key, Class<T> clz);


    /**
     * 设置配置
     * @param key key
     * @param content val
     * @return content val
     */
    boolean putConfig(String key, String content);

    /**
     * 设置配置
     * @param key key
     * @return content val
     * @param timeoutMills timeoutMills
     * @param content content
     */
    boolean putConfig(String key, String content, long timeoutMills);


    /**
     * 获取配置器类型
     * @return string example:apollo
     */
    String getType();


}
