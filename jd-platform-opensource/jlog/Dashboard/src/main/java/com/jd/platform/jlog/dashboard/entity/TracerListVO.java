package com.jd.platform.jlog.dashboard.entity;

import lombok.Data;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * TracerListVO
 *
 * @author wuweifeng
 * @version 1.0
 * @date 2021-09-01
 */
@Data
public class TracerListVO {
    /**
     * 总记录数
     */
    private Long total;
    /**
     * 列表数据
     */
    private List<Map<String, Object>> rows = new ArrayList<>(16);

    /**
     * 消息状态码
     */
    private Integer code;

    /**
     * 消息内容
     */
    private Integer msg;
}
