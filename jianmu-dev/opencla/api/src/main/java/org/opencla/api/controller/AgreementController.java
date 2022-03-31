package org.opencla.api.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import org.opencla.api.application.AgreementService;
import org.opencla.api.dto.AgreementCheckingDto;
import org.opencla.api.dto.AuthorizationEmailSigningDto;
import org.opencla.api.dto.AuthorizationEmailViewingDto;
import org.opencla.api.dto.OtherEmailSigningDto;
import org.opencla.api.dto.vo.AgreementCheckVo;
import org.opencla.api.dto.vo.AgreementVo;
import org.opencla.api.dto.vo.AuthorizationEmailVo;
import org.opencla.api.dto.vo.EnterpriseMailVo;
import org.opencla.api.utils.EnterpriseMailUtil;
import org.springframework.web.bind.annotation.*;

import javax.validation.Valid;
import java.util.List;

/**
 * @author Daihw
 * @class AgreementController
 * @description AgreementController
 * @create 2022/1/10 5:30 下午
 */
@RestController
@RequestMapping("agreements")
@Tag(name = "协议API")
public class AgreementController {

    private final AgreementService agreementService;

    public AgreementController(AgreementService agreementService) {
        this.agreementService = agreementService;
    }

    /**
     * 查看协议
     *
     * @return
     */
    @GetMapping
    @Operation(summary = "查看协议")
    public AgreementVo viewAgreement() {
        return this.agreementService.viewAgreement();
    }

    /**
     * 查看企业邮件内容
     */
    @GetMapping("enterprise_mails")
    @Operation(summary = "查看企业邮件内容")
    public EnterpriseMailVo viewEnterpriseMail() {
        return EnterpriseMailVo.builder()
                .enterpriseMail(EnterpriseMailUtil.ENTERPRISE_MAIL)
                .build();
    }

    /**
     * 查询授权邮箱列表
     *
     * @return
     */
    @GetMapping("emails")
    @Operation(summary = "查询授权邮箱列表")
    public List<AuthorizationEmailVo> viewAuthorizationEmail(@Valid AuthorizationEmailViewingDto authorizationEmailViewingDto) {
        return this.agreementService.viewAuthorizationEmail(authorizationEmailViewingDto);
    }

    /**
     * 签署授权邮箱协议
     */
    @PostMapping("authorizations")
    @Operation(summary = "签署协议-授权邮箱")
    public void signAuthorizationAgreement(@RequestBody @Valid AuthorizationEmailSigningDto authorizationEmailSigningDto) {
        this.agreementService.signAuthorizationAgreement(authorizationEmailSigningDto);
    }

    /**
     * 签署其他邮箱协议
     */
    @PostMapping("others")
    @Operation(summary = "签署协议-其他邮箱")
    public void signOtherAgreement(@RequestBody @Valid OtherEmailSigningDto otherEmailSigningDto) {
        this.agreementService.signOtherAgreement(otherEmailSigningDto);
    }


    /**
     * 检查协议
     *
     * @return
     */
    @GetMapping("check")
    @Operation(summary = "检查协议")
    public AgreementCheckVo checkAgreement(@Valid AgreementCheckingDto agreementCheckingDto) {
        return this.agreementService.checkAgreement(agreementCheckingDto);
    }
}
