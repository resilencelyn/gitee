package org.opencla.api.dto;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Getter;
import lombok.Setter;

import javax.validation.constraints.NotBlank;

/**
 * @author Daihw
 * @class OtherEmailSigningDto
 * @description 其他邮箱签署协议Dto
 * @create 2022/2/9 4:58 下午
 */
@Getter
@Setter
public class OtherEmailSigningDto {
    /**
     * 邮箱
     */
    @NotBlank(message = "邮箱不能为空")
    @Schema(required = true, description = "邮箱")
    private String email;

    /**
     * 验证码
     */
    @NotBlank(message = "验证码不能为空")
    @Schema(required = true, description = "验证码")
    private String checkCode;

    /**
     * 业务标识
     */
    @NotBlank(message = "业务表示不能为空")
    @Schema(required = true, description = "业务标识")
    private String emailFlag;
}
