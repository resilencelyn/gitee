package org.opencla.api.application;

import org.opencla.api.api.impl.AuthApiProxy;
import org.opencla.api.api.impl.vo.EmailVo;
import org.opencla.api.config.AgreementProperties;
import org.opencla.api.dto.AgreementCheckingDto;
import org.opencla.api.dto.AuthorizationEmailSigningDto;
import org.opencla.api.dto.AuthorizationEmailViewingDto;
import org.opencla.api.dto.OtherEmailSigningDto;
import org.opencla.api.dto.vo.AgreementCheckVo;
import org.opencla.api.dto.vo.AgreementVo;
import org.opencla.api.dto.vo.AuthorizationEmailVo;
import org.opencla.api.exception.AccessTokenDoesNotExistException;
import org.opencla.api.exception.AgreementSignedException;
import org.opencla.api.exception.AuthorizationEmailNotExistException;
import org.opencla.api.exception.CheckCodeErrorException;
import org.opencla.api.utils.AgreementUtil;
import org.opencla.api.utils.UserContextHolder;
import org.opencla.domain.model.AgreementModel;
import org.opencla.domain.repository.AgreementRepository;
import org.opencla.verification.service.Result;
import org.opencla.verification.service.email.IEmailService;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.stream.Collectors;

/**
 * @author Daihw
 * @class AgreementService
 * @description AgreementService
 * @create 2022/1/10 5:30 下午
 */
@Service
public class AgreementService {

    private final AgreementRepository agreementRepository;
    private final AgreementProperties agreementProperties;
    private final IEmailService emailService;

    public AgreementService(AgreementRepository agreementRepository, AgreementProperties agreementProperties, IEmailService emailService) {
        this.agreementRepository = agreementRepository;
        this.agreementProperties = agreementProperties;
        this.emailService = emailService;
    }

    /**
     * 查看协议
     *
     * @return
     */
    public AgreementVo viewAgreement() {
        return AgreementVo.builder()
                .agreementContent(AgreementUtil.AGREEMENT_CONTENT)
                .projectName(this.agreementProperties.getProjectName())
                .build();
    }

    /**
     * 查询授权邮箱列表
     *
     * @return
     */
    public List<AuthorizationEmailVo> viewAuthorizationEmail(AuthorizationEmailViewingDto dto) {
        var authApiProxy = AuthApiProxy.builder()
                .thirdPartyType(dto.thirdPartyType())
                .build();
        List<EmailVo> emailVos;
        try {
            emailVos = authApiProxy.getEmail(UserContextHolder.getAccessToken());
        } catch (AccessTokenDoesNotExistException e) {
            throw new AccessTokenDoesNotExistException("授权失败，请到gitee设置-第三方应用中取消授权");
        }
        var agreements = this.agreementRepository.findAllById(emailVos.stream()
                .map(EmailVo::getEmail)
                .collect(Collectors.toList()));
        return emailVos.stream()
                .map(emailVo -> AuthorizationEmailVo.builder()
                        .email(emailVo.getEmail())
                        .labels(emailVo.getLabels())
                        .isSigned(agreements.stream().
                                anyMatch(agreementModel -> agreementModel.getEmail().equals(emailVo.getEmail())))
                        .build())
                .collect(Collectors.toList());
    }

    /**
     * 签署授权邮箱协议
     */
    @Transactional
    public void signAuthorizationAgreement(AuthorizationEmailSigningDto dto) {
        var authApiProxy = AuthApiProxy.builder()
                .thirdPartyType(dto.thirdPartyType())
                .build();
        var emailVos = authApiProxy.getEmail(UserContextHolder.getAccessToken());
        if (emailVos.stream().noneMatch(emailVo -> dto.getEmail().equals(emailVo.getEmail()))) {
            throw new AuthorizationEmailNotExistException();
        }
        if (this.agreementRepository.existsById(dto.getEmail())) {
            throw new AgreementSignedException();
        }
        var agreement = new AgreementModel(dto.getEmail(), this.agreementProperties.getVersion());
        this.agreementRepository.save(agreement);
    }

    /**
     * 签署其他邮箱协议
     */
    @Transactional
    public void signOtherAgreement(OtherEmailSigningDto dto) {
        Result<Void> result = this.emailService.verify(dto.getEmail(), dto.getCheckCode(), dto.getEmailFlag());
        if (!result.getSuccess()) {
            throw new CheckCodeErrorException(result.getErrorMsg());
        }
        if (this.agreementRepository.existsById(dto.getEmail())) {
            throw new AgreementSignedException();
        }
        var agreement = new AgreementModel(dto.getEmail(), this.agreementProperties.getVersion());
        this.agreementRepository.save(agreement);
    }

    /**
     * 检查协议是否签署
     *
     * @param agreementCheckingDto
     * @return
     */
    public AgreementCheckVo checkAgreement(AgreementCheckingDto agreementCheckingDto) {
        return AgreementCheckVo.builder()
                .isSign(this.agreementRepository.existsById(agreementCheckingDto.getEmail()))
                .build();
    }
}
