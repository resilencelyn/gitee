package org.opencla.verification.service.email;

import org.opencla.verification.service.IVerificationService;
import org.opencla.verification.service.Result;

/**
 * 邮件服务接口
 */
public interface IEmailService extends IVerificationService {

    /**
     * 发送验证码
     *
     * @param email     邮箱地址
     * @param code      短信验证码
     * @param content   短信内容
     * @param imageCode 图片验证码
     * @param subject   邮件主题
     * @param flag      业务标志
     * @return
     */
    Result<Void> send(String email, String code, String subject, String content, String imageCode, String flag, String signature);

    /**
     * 发送通知
     *
     * @param emailAddress 邮箱地址
     * @param text         通知
     */
    void send(String emailAddress, String subject, String text, String signature) throws Exception;
}
