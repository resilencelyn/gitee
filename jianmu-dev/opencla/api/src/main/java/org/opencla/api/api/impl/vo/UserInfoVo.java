package org.opencla.api.api.impl.vo;

import lombok.Builder;
import lombok.Getter;

/**
 * 全平台用以获取用户信息的vo
 */
@Getter
@Builder
public class UserInfoVo {
    /**
     * 头像
     */
    private String headUrl;

    /**
     * 用户名
     */
    private String username;
}
