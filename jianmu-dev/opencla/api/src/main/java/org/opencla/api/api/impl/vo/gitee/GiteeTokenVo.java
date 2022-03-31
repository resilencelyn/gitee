
package org.opencla.api.api.impl.vo.gitee;

import lombok.*;

/**
 * gitee的token的vo
 */
@Builder
@Getter
@AllArgsConstructor
@NoArgsConstructor
public class GiteeTokenVo {
    private String access_token;
    private String token_type;
    private long expires_in;
    private String refresh_token;
    private String scope;
    private long created_at;
}