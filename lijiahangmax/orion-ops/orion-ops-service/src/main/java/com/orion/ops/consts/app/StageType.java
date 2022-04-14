package com.orion.ops.consts.app;

import lombok.AllArgsConstructor;
import lombok.Getter;

/**
 * 操作阶段类型
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/12/1 15:48
 */
@AllArgsConstructor
@Getter
public enum StageType {

    /**
     * 构建
     */
    BUILD(10),

    /**
     * 发布
     */
    RELEASE(20),

    ;

    private final Integer type;

    public static StageType of(Integer type) {
        if (type == null) {
            return null;
        }
        for (StageType value : values()) {
            if (value.type.equals(type)) {
                return value;
            }
        }
        return null;
    }

}
