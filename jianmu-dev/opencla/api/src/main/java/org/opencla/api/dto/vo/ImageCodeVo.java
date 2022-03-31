package org.opencla.api.dto.vo;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Getter;

@Getter
@Builder
public class ImageCodeVo {
    /**
     * 图片验证码base64
     */
    @Schema(required = true)
    private String base64;
}
