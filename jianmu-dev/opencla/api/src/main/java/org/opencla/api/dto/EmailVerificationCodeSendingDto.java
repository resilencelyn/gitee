package org.opencla.api.dto;


import lombok.Getter;
import lombok.Setter;
import org.opencla.api.config.valid_group.NotBlankGroup;
import org.opencla.api.config.valid_group.PatternGroup;
import org.opencla.api.utils.Constants;

import javax.validation.constraints.NotBlank;
import javax.validation.constraints.Pattern;

/**
 * 发送短信验证码dto
 */
@Getter
@Setter
public class EmailVerificationCodeSendingDto {

    /**
     * 业务标识
     */
    @NotBlank
    private String emailFlag;

    /**
     * 邮箱地址
     */
    @NotBlank(groups = NotBlankGroup.class, message = "邮箱地址不能为空")
    @Pattern(regexp = Constants.EMAIL_PATTERN, message = "请输入正确的邮箱地址", groups = PatternGroup.class)
    private String emailAddress;

    /**
     * 图片验证码（可选）
     */
    private String imageCode;
}
