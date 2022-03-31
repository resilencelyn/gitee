package org.opencla.api.api;

import org.opencla.api.api.impl.vo.EmailVo;
import org.opencla.api.api.impl.vo.UserInfoVo;

import java.util.List;

/**
 * 抽象第三方平台接口
 */
public interface AuthApi {
    /**
     * 获取授权地址
     *
     * @param redirectUri
     * @return
     */
    String getAuthUrl(String redirectUri);


    /**
     * 获取accessToken
     *
     * @param code
     * @return
     */
    default String getAccessToken(String code) {
        return this.getAccessToken(code, null);
    }

    /**
     * 获取accessToken
     *
     * @param code
     * @return
     */
    String getAccessToken(String code, String redirectUri);

    /**
     * 获取用户信息
     *
     * @param token
     * @return
     */
    UserInfoVo getUserInfo(String token);

    /**
     * 获取用户邮箱
     *
     * @return
     */
    List<EmailVo> getEmail(String token);
}
