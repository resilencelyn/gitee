package com.jd.platform.jlog.common.model;

import java.io.Serializable;
import java.util.List;
import java.util.Map;

/**
 * 承载传输信息的对象
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-12
 */
public class TracerBean implements Serializable {
    /**
     * 时间戳
     */
    private long createTime;
    /**
     * 耗时（毫秒）
     */
    private int costTime;
    /**
     * 唯一id，代表一条链路
     */
    private String tracerId;
    /**
     * tracer对象，里面放的是List<Map<String, Object>>
     * 第一个元素是request对象，key为request，value为完整request入参，从中可找到uuid
     * 最后一个元素是response响应，key为response，value为响应值byte[]，可转为String入库
     */
    private List<Map<String, Object>> tracerObject;


    @Override
    public String toString() {
        return "TracerBean{" +
                "createTime=" + createTime +
                ", costTime=" + costTime +
                ", tracerId='" + tracerId + '\'' +
                ", tracerObject=" + tracerObject +
                '}';
    }

    public String getTracerId() {
        return tracerId;
    }

    public void setTracerId(String tracerId) {
        this.tracerId = tracerId;
    }

    public int getCostTime() {
        return costTime;
    }

    public void setCostTime(int costTime) {
        this.costTime = costTime;
    }

    public long getCreateTime() {
        return createTime;
    }

    public void setCreateTime(long createTime) {
        this.createTime = createTime;
    }

    public List<Map<String, Object>> getTracerObject() {
        return tracerObject;
    }

    public void setTracerObject(List<Map<String, Object>> tracerObject) {
        this.tracerObject = tracerObject;
    }
}
