package com.jd.platform.jlog.common.model;

import java.io.Serializable;
import java.util.List;

/**
 * 客户端-服务端彼此传输的数据
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-17
 */
public class TracerData implements Serializable {
    /**
     * 多个tracer批量打包后
     */
    private List<TracerBean> tracerBeanList;

    @Override
    public String toString() {
        return "TracerData{" +
                "tracerBeanList=" + tracerBeanList +
                '}';
    }

    public List<TracerBean> getTracerBeanList() {
        return tracerBeanList;
    }

    public void setTracerBeanList(List<TracerBean> tracerBeanList) {
        this.tracerBeanList = tracerBeanList;
    }
}
