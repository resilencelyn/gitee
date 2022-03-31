package org.opencla.api.config;

import org.opencla.verification.service.email.IEmailService;
import org.opencla.verification.service.email.impl.EmailConfig;
import org.opencla.verification.util.VerificationUtil;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

/**
 * bean配置
 */
@Configuration("emailBeanConfig")
public class BeanConfig {
    private final EmailProperties emailProperties;

    public BeanConfig(EmailProperties emailProperties) {
        this.emailProperties = emailProperties;
    }


    /**
     * 邮件验证码服务
     *
     * @return
     */
    @Bean("commonEmailService")
    public IEmailService emailService() {
        EmailConfig emailConfig = new EmailConfig();
        emailConfig.setSender(this.emailProperties.getSender());
        emailConfig.setHost(this.emailProperties.getHost());
        emailConfig.setAuthorizationCode(this.emailProperties.getAuthorizationCode());
        emailConfig.setPort(this.emailProperties.getPort() == null ? emailConfig.getPort() : this.emailProperties.getPort());
        emailConfig.setUseSsl(this.emailProperties.getUseSsl() == null ? emailConfig.getUseSsl() : this.emailProperties.getUseSsl());
        emailConfig.setDebug(this.emailProperties.getDebug() == null ? emailConfig.getDebug() : this.emailProperties.getDebug());
        return VerificationUtil.createEmailService(emailConfig);
    }
}
