package org.opencla.api.dto;


import lombok.Getter;
import lombok.Setter;
import org.opencla.api.config.valid_group.NotBlankGroup;
import org.opencla.api.config.valid_group.PatternGroup;
import org.opencla.api.utils.Constants;

import javax.validation.constraints.NotBlank;
import javax.validation.constraints.Pattern;

/**
 * 刷新图片验证码dto
 */
@Getter
@Setter
public class EmailImageCodeRefreshingDto {

    /**
     * 业务标识
     * 手机号更改：MOBILE_CHANGE
     * 手机号授权：MOBILE_AUTHENTICATION
     */
    @NotBlank
    private String emailFlag;

    /**
     * 手机号码
     */
    @NotBlank(groups = NotBlankGroup.class, message = "邮箱地址不能为空")
    @Pattern(regexp = Constants.EMAIL_PATTERN, message = "请输入正确的邮箱地址", groups = PatternGroup.class)
    private String emailAddress;
}
