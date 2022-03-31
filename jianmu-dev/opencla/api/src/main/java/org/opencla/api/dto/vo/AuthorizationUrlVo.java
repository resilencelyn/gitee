package org.opencla.api.dto.vo;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Getter;

/**
 * 授权url的vo
 */
@Getter
@Builder
public class AuthorizationUrlVo {
    /**
     * 授权url
     */
    @Schema(required = true)
    private String authorizationUrl;

}
