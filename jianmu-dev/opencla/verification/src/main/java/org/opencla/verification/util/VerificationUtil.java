package org.opencla.verification.util;

import org.opencla.verification.service.email.IEmailService;
import org.opencla.verification.service.email.impl.EmailConfig;
import org.opencla.verification.service.email.impl.EmailService;

/**
 * 验证码工具类
 * Created by liamjung on 2018/1/22.
 */
public class VerificationUtil {

    /**
     * 创建邮箱服务
     *
     * @param config
     * @return
     */
    public static IEmailService createEmailService(EmailConfig config) {
        return new EmailService(config);
    }
}
