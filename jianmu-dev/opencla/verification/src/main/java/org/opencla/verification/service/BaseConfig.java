package org.opencla.verification.service;

import lombok.Getter;
import lombok.Setter;

/**
 * 配置抽象类
 * Created by liamjung on 2018/1/19.
 */
@Getter
@Setter
public abstract class BaseConfig {

    /**
     * 唯一标识模型过期时间
     * 必须大于验证码过期时间和图片验证码过期时间
     * 单位：秒
     * 默认值：15分钟
     */
    private int identifyModelExpiration = 15 * 60;

    /**
     * 发送验证码最大次数，必须大于1
     * 表示唯一标识模型过期时间内，已发送验证码次数大于等于identifyMaxSendingCount时，不再发送短信，提示错误
     * 默认值：10次
     */
    private int identifyMaxSendingCount = 10;

    /**
     * 短信验证码过期时间
     * 单位：毫秒
     * 默认值：10分钟
     */
    private long identifyCodeExpiration = 10 * 60 * 1000;

    /**
     * 验证短信验证码最大次数，必须大于1
     * 表示短信模型过期时间内，已验证短信验证码次数大于等于identifyCodeMaxVerifyingCount时，立即过期，提示错误
     * 默认值：3次
     */
    private int identifyCodeMaxVerifyingCount = 3;

    /**
     * 短信验证码重发间隔
     * 单位：毫秒
     * 默认值：1分钟
     */
    private long identifyCodeResendingInterval = 60 * 1000;

    /**
     * 图片验证码过期时间
     * 单位：毫秒
     * 默认值：5分钟
     */
    private long imageCodeExpiration = 5 * 60 * 1000;

    /**
     * 图片验证码显示计数
     * 表示已发送短信验证码次数大于等于imageCodeDisplayCount时，需输入图片验证码
     * 默认值：2次
     */
    private int imageCodeDisplayCount = 2;
}
