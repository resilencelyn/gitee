package org.opencla.api.api.impl.config;

import lombok.Getter;
import lombok.Setter;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

@Setter
@Getter
@Component
@ConfigurationProperties(prefix = "opencla.gitee")
public class GiteeConfigProperties {
    private String clientSecret;
    private String clientId;
    private String tokenUrl;
    private String codeUrl;
    private String responseType;
    private String userInfoUrl;
    private String emails;
}
