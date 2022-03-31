package org.opencla.verification.model;

import lombok.Getter;

/**
 * 基础模型
 */
@Getter
public abstract class BaseModel {
    /**
     * 唯一身份验证码
     */
    protected String identifyCode;
    /**
     * 验证码过期时间
     */
    protected Long identifyCodeExpiration;
    /**
     * 验证码重发时间
     */
    protected Long identifyCodeResendingTime;
    /**
     * 图片验证码
     */
    protected String imageCode;
    /**
     * 图片验证码过期时间
     */
    protected Long imageCodeExpiration;
    /**
     * 已发次数
     */
    protected int sendingCount;
    /**
     * 已验次数
     */
    protected int verifyingCount;
    /**
     * 业务标志
     */
    protected String flag;

    /**
     * 自定义缓存key
     *
     * @param identify 标识
     * @param flag     业务标识
     * @return 缓存key
     */
    public static String id(String identify, String flag) {
        return flag + ":" + identify;
    }

    /**
     * 设置过期
     *
     * @param identifyCodeExpiration
     */
    public void expire(Long identifyCodeExpiration) {
        this.identifyCodeExpiration = identifyCodeExpiration;
    }

    /**
     * 增加验证次数
     *
     * @param verifyingCount
     */
    public void increase(int verifyingCount) {
        this.verifyingCount = verifyingCount;
    }

    /**
     * 设置图片验证码信息
     *
     * @param imageCode
     * @param imageCodeExpiration
     */
    public void setImageCodeInfo(String imageCode, Long imageCodeExpiration) {
        this.imageCode = imageCode;
        this.imageCodeExpiration = imageCodeExpiration;
    }

    /**
     * 生成缓存key
     *
     * @return 缓存key
     */
    public abstract String id();
}
