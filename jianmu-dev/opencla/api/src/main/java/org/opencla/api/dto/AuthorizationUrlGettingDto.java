package org.opencla.api.dto;

import org.opencla.api.enumeration.ThirdPartyTypeEnum;
import org.opencla.validation.extension.annotation.Enum;
import lombok.Getter;
import lombok.Setter;

import javax.validation.constraints.NotBlank;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Pattern;

/**
 * 返回授权url的dto
 */
@Getter
@Setter
public class AuthorizationUrlGettingDto {
    /**
     * 第三方平台类型
     */
    @Enum(value = ThirdPartyTypeEnum.class, message = "请输入有效的登录平台")
    @NotNull
    private String thirdPartyType;

    /**
     * 回调地址
     */
    @NotBlank(message = "回调地址不能为空")
    @Pattern(regexp = "^$|(https?)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]", message = "请输入正确的地址")
    private String redirectUri;

    /**
     * 转换type为枚举类型
     *
     * @return
     */
    public ThirdPartyTypeEnum thirdPartyType() {
        return ThirdPartyTypeEnum.valueOf(this.thirdPartyType);
    }
}