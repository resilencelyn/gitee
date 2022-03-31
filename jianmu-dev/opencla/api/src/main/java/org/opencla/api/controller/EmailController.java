package org.opencla.api.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import org.opencla.api.application.EmailService;
import org.opencla.api.config.valid_group.SequenceGroup;
import org.opencla.api.dto.EmailImageCodeRefreshingDto;
import org.opencla.api.dto.EmailVerificationCodeSendingDto;
import org.opencla.api.dto.vo.ImageCodeVo;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.*;

/**
 * auth控制器
 */
@Validated
@RestController
@RequestMapping("emails")
@Tag(name = "邮箱相关控制器", description = "邮箱相关控制器")
public class EmailController {
    private final EmailService emailService;

    public EmailController(EmailService emailService) {
        this.emailService = emailService;
    }

    /**
     * 发送邮箱验证码，返回值中，success为true时，data为null或验证码图片base64，非null时，下次调用api时，需指定imageCode
     *
     * @param dto
     * @return
     */
    @PostMapping("/send")
    @Operation(summary = "发送邮箱验证码", description = "发送邮箱验证码")
    public void sendSmsCode(@Validated({SequenceGroup.class}) @RequestBody EmailVerificationCodeSendingDto dto) {
        this.emailService.sendEmailCode(dto);
    }

    /**
     * 刷新图片验证码，返回值中，success为true时，data为验证码图片base64
     *
     * @param dto
     * @return
     */
    @PatchMapping("/image_code")
    @Operation(summary = "刷新图片验证码", description = "刷新图片验证码")
    public ImageCodeVo refreshImageCode(@Validated({SequenceGroup.class}) @RequestBody EmailImageCodeRefreshingDto dto) {
        return this.emailService.refreshImageCode(dto);
    }
}
