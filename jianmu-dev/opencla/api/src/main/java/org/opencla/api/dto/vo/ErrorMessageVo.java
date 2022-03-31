package org.opencla.api.dto.vo;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Getter;

/**
 * 错误信息值对象
 */
@Getter
@Builder
public class ErrorMessageVo {
    /**
     * 错误码
     */
    @Schema(required = true, description = "错误码")
    private final String errorCode;

    /**
     * 错误信息
     */
    @Schema(required = true, description = "错误信息")
    private final String message;
}
