package org.opencla.api.application;

import lombok.extern.slf4j.Slf4j;
import org.opencla.api.api.impl.AuthApiProxy;
import org.opencla.api.dto.AuthorizationUrlGettingDto;
import org.opencla.api.dto.LoggingDto;
import org.opencla.api.dto.vo.AuthorizationUrlVo;
import org.opencla.api.dto.vo.TokenVo;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

/**
 * 单点登录账号service
 */
@Service
@Transactional
@Slf4j
public class AuthService {

    /**
     * 获取url
     *
     * @param dto
     * @return
     */
    public AuthorizationUrlVo getAuthorizationUrl(AuthorizationUrlGettingDto dto) {
        AuthApiProxy authApiProxy = AuthApiProxy.builder()
                .thirdPartyType(dto.thirdPartyType())
                .build();
        return AuthorizationUrlVo.builder()
                .authorizationUrl(authApiProxy.getAuthUrl(dto.getRedirectUri()))
                .build();
    }

    /**
     * 获取token
     *
     * @param dto
     * @return
     */
    public TokenVo login(LoggingDto dto) {
        AuthApiProxy authApiProxy = AuthApiProxy.builder()
                .thirdPartyType(dto.thirdPartyType())
                .build();
        String accessToken = authApiProxy.getAccessToken(dto.getCode(), dto.getRedirectUri());
        return TokenVo.builder()
                .token(accessToken)
                .build();
    }


}
