package org.opencla.api.api.impl;

import org.opencla.api.api.AuthApi;
import org.opencla.api.api.impl.config.GiteeConfigProperties;
import org.opencla.api.api.impl.vo.EmailVo;
import org.opencla.api.api.impl.vo.UserInfoVo;
import org.opencla.api.enumeration.ThirdPartyTypeEnum;
import org.opencla.api.exception.NotSupportedThirdPartPlatformException;
import org.opencla.api.exception.ThirdPartConfigLoseException;
import org.opencla.api.utils.ApplicationContextUtils;
import lombok.Builder;

import java.util.List;

/**
 * 第三方平台代理对象
 */
@Builder
public class AuthApiProxy implements AuthApi {
    private ThirdPartyTypeEnum thirdPartyType;

    private AuthApi getApi() {
        switch (this.thirdPartyType) {
            case GITEE:
                return ApplicationContextUtils.getBean(GiteeApi.class);
            // TODO 待扩展其他平台
            default:
                throw new NotSupportedThirdPartPlatformException();
        }
    }

    @Override
    public String getAuthUrl(String redirectUri) {
        this.beforeEnable();
        AuthApi api = getApi();
        return api.getAuthUrl(redirectUri);
    }

    @Override
    public String getAccessToken(String code, String redirectUri) {
        this.beforeEnable();
        AuthApi api = getApi();
        return api.getAccessToken(code, redirectUri);
    }

    @Override
    public UserInfoVo getUserInfo(String token) {
        this.beforeEnable();
        AuthApi api = getApi();
        return api.getUserInfo(token);
    }

    @Override
    public List<EmailVo> getEmail(String token) {
        this.beforeEnable();
        AuthApi api = getApi();
        return api.getEmail(token);
    }

    /**
     * 前置方法
     * 在方法调用之前，执行此方法
     */
    private void beforeEnable() {
        GiteeConfigProperties configProperties;
        switch (this.thirdPartyType) {
            case GITEE:
                configProperties = ApplicationContextUtils.getBean(GiteeConfigProperties.class);
                if (configProperties.getTokenUrl() == null || configProperties.getTokenUrl().isBlank()) {
                    throw new ThirdPartConfigLoseException();
                }
                break;
            // TODO 待扩展其他平台
            default:
                throw new NotSupportedThirdPartPlatformException();
        }
    }
}
