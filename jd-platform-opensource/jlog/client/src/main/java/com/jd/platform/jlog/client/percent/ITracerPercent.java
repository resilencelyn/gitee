package com.jd.platform.jlog.client.percent;

import java.util.Set;

/**
 * 常量类，控制日志的百分比
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-21
 */
public interface ITracerPercent {
    /**
     * 切量百分比
     */
    Integer percent();

    /**
     * 哪些接口要忽略，忽略后不搜集出入参
     */
    default Set<String> ignoreUriSet() {
        return null;
    }
}
