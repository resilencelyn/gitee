package org.opencla.api.application;

import lombok.extern.slf4j.Slf4j;
import org.opencla.api.config.EmailProperties;
import org.opencla.api.dto.EmailImageCodeRefreshingDto;
import org.opencla.api.dto.EmailVerificationCodeSendingDto;
import org.opencla.api.dto.vo.ImageCodeVo;
import org.opencla.api.exception.*;
import org.opencla.api.utils.EmailCodeUtil;
import org.opencla.api.utils.EmailContentUtil;
import org.opencla.verification.service.Result;
import org.opencla.verification.service.email.IEmailService;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.Objects;

/**
 * 邮件service
 */
@Service
@Transactional
@Slf4j
public class EmailService {
    private final EmailProperties emailProperties;
    private final IEmailService emailService;

    public EmailService(EmailProperties emailProperties, IEmailService emailService) {
        this.emailProperties = emailProperties;
        this.emailService = emailService;
    }

    /**
     * 发送邮箱验证码
     *
     * @param dto
     * @return
     */
    public void sendEmailCode(EmailVerificationCodeSendingDto dto) {
        // 邮箱验证码
        String flag = dto.getEmailFlag();
        String emailAddress = dto.getEmailAddress();
        String imageCode = dto.getImageCode();
        String code = EmailCodeUtil.generateRandomCode();
        String content = EmailContentUtil.setEmailCode(code, emailAddress, this.emailProperties.getTeam(), this.emailProperties.getSupportEmail());
        String subject = this.emailProperties.getSubject();
        String signature = this.emailProperties.getSignature();
        log.info("发送邮箱验证码 (emailAddress: {}; code: {}; flag: {})", emailAddress, code, flag);

        //发送短信验证码
        Result<Void> result = this.emailService.send(emailAddress, code, subject, content, imageCode, flag, signature);

        if (!result.getSuccess()) {
            if (Objects.equals(result.getErrorCode(), "V_E_013") || Objects.equals(result.getErrorCode(), "V_E_014")) {
                throw new CodeExceedSendingMaximumException();
            }
            if (Objects.equals(result.getErrorCode(), "V_E_022")) {
                throw new ImageCodeNotEmptyException();
            }
            if (Objects.equals(result.getErrorCode(), "V_E_020")) {
                throw new ValidationImageCodeFailedException();
            }
            if (Objects.equals(result.getErrorCode(), "V_E_001")) {
                throw new UnknownException();
            }
            if (Objects.equals(result.getErrorCode(), "V_E_021")) {
                throw new ImageCodeExpiredException();
            }
        }
    }

    /**
     * 刷新图片验证码
     *
     * @param dto
     * @retErn
     */
    public ImageCodeVo refreshImageCode(EmailImageCodeRefreshingDto dto) {
        //刷新图片验证码
        Result<String> result = this.emailService.refreshImageCode(dto.getEmailAddress(), dto.getEmailFlag());

        if (result.getSuccess()) {
            return ImageCodeVo.builder()
                    .base64(result.getData())
                    .build();
        }

        throw new UnknownException();
    }
}
