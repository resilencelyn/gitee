package com.jd.platform.jlog.core;

import java.util.HashSet;
import java.util.Set;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName Constant.java
 * @createTime 2022年02月26日 10:19:00
 */
public class Constant {

    /**
     * 监听重读配置文件间隔 单位ms
     */
    static final long LISTENER_CONFIG_INTERVAL = 10000;

    static final String CONFIG_FILE_PROPERTIES = "/application.properties";

    static final String CONFIG_FILE_YML = "/application.yml";

    static final String JLOG_CONFIG_FILE = "/jLog.properties";


    static final String ENV = "env";

    static final String YML = "yml";

    public static final long DEFAULT_TIMEOUT = 2 * 1000;

    static final long AWAIT_TIME = 3 * 1000;

    public static final String SERVER_ADDR_KEY = "serverAddr";

    public static final String NAMESPACE_KEY = "namespace";

    public static final String DEFAULT_NAMESPACE = "jLog";


    /**
     * 配置文件集合
     */
    public static final Set<String> CONFIG_FILES = new HashSet<String>() {
        {
            add(CONFIG_FILE_PROPERTIES);
            add(CONFIG_FILE_YML);
            add(JLOG_CONFIG_FILE);
        }
    };
}
