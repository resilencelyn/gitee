package org.opencla.api.dto.vo;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Getter;

/**
 * @author Daihw
 * @class ViewAgreementVo
 * @description 查看协议值对象
 * @create 2022/1/11 9:43 上午
 */
@Getter
@Builder
public class AgreementVo {
    /**
     * 协议内容
     */
    @Schema(required = true, description = "协议内容")
    private final String agreementContent;

    /**
     * 项目名称
     */
    @Schema(required = true, description = "项目名称")
    private final String projectName;
}
