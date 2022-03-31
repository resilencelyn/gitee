package org.opencla.api.dto.vo;


import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Getter;

import java.util.List;

/**
 * @author Daihw
 * @class AuthorizationEmailVo
 * @description 授权邮箱值对象
 * @create 2022/2/9 4:20 下午
 */
@Getter
@Builder
public class AuthorizationEmailVo {
    /**
     * 邮箱
     */
    @Schema(required = true, description = "邮箱")
    private final String email;
    /**
     * 标签
     */
    @Schema(description = "标签")
    private final List<String> labels;
    /**
     * 是否签署协议
     */
    @Schema(required = true, description = "是否签署协议")
    private final Boolean isSigned;
}
