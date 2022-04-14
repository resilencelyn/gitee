package com.orion.ops.entity.request;

import com.orion.lang.wrapper.PageRequest;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;

/**
 * 应用环境变量
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/7/4 11:25
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class ApplicationEnvRequest extends PageRequest {

    /**
     * id
     */
    private Long id;

    /**
     * id集合
     */
    private List<Long> idList;

    /**
     * 应用id
     */
    private Long appId;

    /**
     * 环境id
     */
    private Long profileId;

    /**
     * key
     */
    private String key;

    /**
     * value
     */
    private String value;

    /**
     * 描述
     */
    private String description;

    /**
     * 视图类型
     *
     * @see com.orion.ops.consts.env.EnvViewType
     */
    private Integer viewType;

    /**
     * 目标环境id
     */
    private List<Long> targetProfileIdList;

}
