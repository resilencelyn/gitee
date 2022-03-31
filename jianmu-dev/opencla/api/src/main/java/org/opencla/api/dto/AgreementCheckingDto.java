package org.opencla.api.dto;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Getter;
import lombok.Setter;

import javax.validation.constraints.NotBlank;

/**
 * @author Daihw
 * @class AgreementCheckingDto
 * @description 检查是否签署协议dto
 * @create 2022/1/26 9:51 上午
 */
@Setter
@Getter
public class AgreementCheckingDto {
    /**
     * 邮箱
     */
    @NotBlank(message = "邮箱不能为空")
    @Schema(required = true, description = "邮箱")
    private String email;
}
