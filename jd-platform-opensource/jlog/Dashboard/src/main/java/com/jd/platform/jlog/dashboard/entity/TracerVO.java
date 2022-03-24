package com.jd.platform.jlog.dashboard.entity;

import lombok.Data;

/**
 * @author shenkaiwen5
 * @version 1.0
 * @date 2021-09-02
 */
@Data
public class TracerVO {
    /**
     * 用户pin
     */
    private String pin;
    /**
     * 请求正文
     */
    private String requestContent;
    /**
     * 相应正文
     */
    private Object responseContent;
}
