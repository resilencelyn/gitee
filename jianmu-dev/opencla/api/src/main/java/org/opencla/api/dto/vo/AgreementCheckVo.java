package org.opencla.api.dto.vo;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Getter;

/**
 * @author Daihw
 * @class AgreementCheckVo
 * @description 协议检查vo
 * @create 2022/1/26 10:46 上午
 */
@Getter
@Builder
public class AgreementCheckVo {
    /**
     * 是否签署
     */
    @Schema(required = true, description = "是否签署")
    private final Boolean isSign;
}
