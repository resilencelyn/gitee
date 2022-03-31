package org.opencla.api.config;

import lombok.Getter;
import lombok.Setter;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

@Getter
@Setter
@Component
@ConfigurationProperties(prefix = "opencla.email")
public class EmailProperties {
    public EmailProperties(){}
    /**
     * 发送者邮箱
     */
    private String sender;
    /**
     * 邮箱服务器的smtp地址
     */
    private String host;
    /**
     * 授权码
     */
    private String authorizationCode;
    /**
     * 签名
     */
    private String signature;
    /**
     * 邮件主题
     */
    private String subject;
    /**
     * 邮箱服务的端口
     */
    private Integer port;
    /**
     * 是否使用ssl连接邮箱服务器
     */
    private Boolean useSsl;
    /**
     * 团队
     */
    private String team;
    /**
     * 支持邮箱
     */
    private String supportEmail;
    /**
     * 是否开启debug
     */
    private Boolean debug;
}
