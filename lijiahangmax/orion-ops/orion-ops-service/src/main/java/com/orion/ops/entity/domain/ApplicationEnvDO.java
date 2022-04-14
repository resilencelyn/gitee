package com.orion.ops.entity.domain;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.io.Serializable;
import java.util.Date;

/**
 * <p>
 * 应用环境变量
 * </p>
 *
 * @author Jiahang Li
 * @since 2021-07-02
 */
@Data
@EqualsAndHashCode(callSuper = false)
@TableName("application_env")
public class ApplicationEnvDO implements Serializable {

    private static final long serialVersionUID = 1L;

    /**
     * id
     */
    @TableId(value = "id", type = IdType.AUTO)
    private Long id;

    /**
     * 应用id
     */
    @TableField("app_id")
    private Long appId;

    /**
     * 环境id
     */
    @TableField("profile_id")
    private Long profileId;

    /**
     * key
     */
    @TableField("attr_key")
    private String attrKey;

    /**
     * value
     */
    @TableField("attr_value")
    private String attrValue;

    /**
     * 是否为系统变量 1是 2否
     *
     * @see com.orion.ops.consts.Const#IS_SYSTEM
     * @see com.orion.ops.consts.Const#NOT_SYSTEM
     */
    @TableField("system_env")
    private Integer systemEnv;

    /**
     * 描述
     */
    @TableField("description")
    private String description;

    /**
     * 是否删除 1未删除 2已删除
     *
     * @see com.orion.ops.consts.Const#NOT_DELETED
     * @see com.orion.ops.consts.Const#IS_DELETED
     */
    @TableLogic
    private Integer deleted;

    /**
     * 创建时间
     */
    @TableField("create_time")
    private Date createTime;

    /**
     * 修改时间
     */
    @TableField("update_time")
    private Date updateTime;

}
