package org.opencla.api.dto.vo;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Getter;

/**
 * 返回token的vo
 */
@Getter
@Builder
public class TokenVo {
    /**
     * token
     */
    @Schema(required = true)
    private String token;
}
