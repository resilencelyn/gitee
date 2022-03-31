package org.opencla.api.enumeration;

import lombok.Getter;

/**
 * gitee请求类型枚举
 */
public enum GiteeGrantTypeEnum {
    AUTHORIZATION_CODE("authorization_code");

    @Getter
    private final String typeName;

    GiteeGrantTypeEnum(String typeName) {
        this.typeName = typeName;
    }
}
