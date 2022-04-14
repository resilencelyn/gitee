package com.orion.ops.entity.domain;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.io.Serializable;
import java.util.Date;

/**
 * <p>
 * 应用依赖机器表
 * </p>
 *
 * @author Jiahang Li
 * @since 2021-07-02
 */
@Data
@EqualsAndHashCode(callSuper = false)
@TableName("application_machine")
public class ApplicationMachineDO implements Serializable {

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
     * 机器id
     */
    @TableField("machine_id")
    private Long machineId;

    /**
     * 当前版本发布id
     */
    @TableField("release_id")
    private Long releaseId;

    /**
     * 当前版本构建id
     */
    @TableField("build_id")
    private Long buildId;

    /**
     * 当前版本构建序列
     */
    @TableField("build_seq")
    private Integer buildSeq;

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
