package com.orion.ops.consts.app;

import lombok.AllArgsConstructor;
import lombok.Getter;

/**
 * 仓库认证方式
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2022/2/19 22:37
 */
@AllArgsConstructor
@Getter
public enum VcsAuthType {

    /**
     * 密码
     */
    PASSWORD(10),

    /**
     * 私人令牌
     */
    TOKEN(20),

    ;

    private final Integer type;

    public static VcsAuthType of(Integer type) {
        if (type == null) {
            return null;
        }
        for (VcsAuthType value : values()) {
            if (value.type.equals(type)) {
                return value;
            }
        }
        return null;
    }

}
