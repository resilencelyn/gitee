package org.opencla.api.enumeration;

import lombok.Getter;

/**
 * 错误码枚举
 */
public enum ErrorCodeEnum {
    PARAMETER_INVALID("参数校验异常"),
    AGREEMENT_SIGNED("协议不能重复签署"),
    NOT_SUPPORTED_THIRD_PART_PLATFORM("不支持此第三方平台"),
    JSON_PARSE_EXCEPTION("json解析失败"),
    GET_TOKEN_REQUEST_PARAMETER_ERROR("请求参数错误，获取token失败"),
    ACCESS_TOKEN_DOES_NOT_EXIST("访问令牌错误或访问令牌不存在"),
    MAILBOX_ACQUISITION_FAILED("邮箱获取失败"),
    THIRD_PART_CONFIG_LOSE("找不到此第三方平台配置"),
    AUTHORIZATION_EMAIL_NOT_EXIST("授权邮箱不存在"),
    CHECK_CODE_ERROR("验证码错误"),
    CODE_EXCEED_SENDING_MAXIMUM("禁止频繁发送验证码"),
    IMAGE_CODE_NOT_EMPTY("图片验证码不能为空"),
    EMAIL_CONTENT_FILE_NOT_EXISTS("邮件内容文件未找到"),
    VALIDATION_IMAGE_CODE_FAILED("校验图片验证码失败"),
    UNKNOWN("未知错误"),
    IMAGE_CODE_EXPIRED("图片验证码已过期");

    @Getter
    private final String message;

    ErrorCodeEnum(String message) {
        this.message = message;
    }
}