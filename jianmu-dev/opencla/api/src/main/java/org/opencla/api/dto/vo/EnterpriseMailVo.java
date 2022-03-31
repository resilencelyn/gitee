package org.opencla.api.dto.vo;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Getter;

/**
 * @class EnterpriseMailVo
 * @description EnterpriseMailVo
 * @author Daihw
 * @create 2022/3/29 10:47 上午
 */
@Getter
@Builder
public class EnterpriseMailVo {
    /**
     * 企业邮件内容
     */
    @Schema(required = true, description = "企业邮件内容")
    private String enterpriseMail;
}
