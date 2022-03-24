package com.jd.platform.jlog.dashboard.model;

import lombok.Data;

/**
 * 查询条件对象
 * @author wuweifeng
 * @version 1.0
 * @date 2021-08-31
 */
@Data
public class QueryListModel {
    /**
     * 追踪事件id
     */
    private Long tracerId;
    /**
     * 用户pin
     */
    private String pin;
    /**
     * 接入应用名
     */
    private String appName;
    /**
     * uuid
     */
    private String uuid;
    /**
     * 开始时间
     */
    private String beginTime;
    /**
     * 结束时间
     */
    private String endTime;
    /**
     * 客户端种类 1：安卓 2：苹果 0：其他
     */
    private String clientType;
    /**
     * 客户端版本号
     */
    private String clientVersion;
    /**
     * 页码
     */
    private Long pageNum;
    /**
     * 接口名
     */
    private String uri;
    /**
     * serverIp
     */
    private String serverIp;
    /**
     * userIp
     */
    private String userIp;
}
