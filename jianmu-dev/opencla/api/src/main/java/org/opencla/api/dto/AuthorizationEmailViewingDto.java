package org.opencla.api.dto;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Getter;
import lombok.Setter;
import org.opencla.api.enumeration.ThirdPartyTypeEnum;
import org.opencla.validation.extension.annotation.Enum;

import javax.validation.constraints.NotBlank;

/**
 * @author Daihw
 * @class AuthorizationEmailViewingDto
 * @description 查询授权邮箱Dto
 * @create 2022/2/14 2:32 下午
 */
@Getter
@Setter
public class AuthorizationEmailViewingDto {
    /**
     * 第三方平台类型
     */
    @NotBlank(message = "第三方平台类型不能为空")
    @Enum(value = ThirdPartyTypeEnum.class, message = "请输入有效的登录平台")
    @Schema(required = true, description = "第三方平台类型")
    private String thirdPartyType;

    /**
     * 转换type为枚举类型
     *
     * @return
     */
    public ThirdPartyTypeEnum thirdPartyType() {
        return ThirdPartyTypeEnum.valueOf(this.thirdPartyType);
    }
}
