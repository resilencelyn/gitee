package org.opencla.api.dto;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Getter;
import lombok.Setter;
import org.opencla.api.enumeration.ThirdPartyTypeEnum;
import org.opencla.validation.extension.annotation.Enum;

import javax.validation.constraints.NotBlank;

/**
 * @author Daihw
 * @class AuthorizationEmailSigningDto
 * @description 授权邮箱签署协议Dto
 * @create 2022/2/9 4:42 下午
 */
@Getter
@Setter
public class AuthorizationEmailSigningDto {
    /**
     * 第三方平台类型
     */
    @Enum(value = ThirdPartyTypeEnum.class, message = "请输入有效的登录平台")
    @NotBlank(message = "第三方平台类型不能为空")
    @Schema(required = true, description = "第三方平台类型")
    private String thirdPartyType;

    /**
     * 邮箱
     */
    @NotBlank(message = "邮箱不能为空")
    @Schema(required = true, description = "邮箱")
    private String email;

    /**
     * 转换type为枚举类型
     *
     * @return
     */
    public ThirdPartyTypeEnum thirdPartyType() {
        return ThirdPartyTypeEnum.valueOf(this.thirdPartyType);
    }
}
