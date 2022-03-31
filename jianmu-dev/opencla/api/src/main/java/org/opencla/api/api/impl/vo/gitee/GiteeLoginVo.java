package org.opencla.api.api.impl.vo.gitee;

import lombok.*;

/**
 * gitee获取令牌vo，获取access_token
 */
@Builder
@Getter
public class GiteeLoginVo {
    private String grant_type;
    private String code;
    private String client_id;
    private String redirect_uri;
    private String client_secret;
}
