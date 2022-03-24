package com.jd.platform.jlog.client.percent;

/**
 * 切量百分比默认
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-21
 */
public class DefaultTracerPercentImpl implements ITracerPercent {
    @Override
    public Integer percent() {
        return 100;
    }
}
