package com.jd.platform.jlog.dashboard.model;

import lombok.Data;

/**
 * 单个查询条件
 * @author wuweifeng
 * @version 1.0
 * @date 2021-09-01
 */
@Data
public class QuerySingleModel {
    /**
     * 追踪事件id
     */
    private Long tracerId;
    /**
     * 用户pin
     */
    private String pin;
}
