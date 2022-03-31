package org.opencla.verification.service;

public interface IVerificationService {
    /**
     * 验证验证码
     *
     * @param identify 手机号码
     * @param smsCode  短信验证码
     * @param flag     业务标志
     * @return
     */
    Result<Void> verify(String identify, String smsCode, String flag);

    /**
     * 刷新图片验证码
     *
     * @param phoneNo 手机号码
     * @param flag    业务标志
     * @return 返回值中的data为null或验证码图片base64
     */
    Result<String> refreshImageCode(String phoneNo, String flag);
}
