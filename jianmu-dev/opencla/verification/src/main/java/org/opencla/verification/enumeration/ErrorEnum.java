package org.opencla.verification.enumeration;

/**
 * 错误枚举
 * Created by liamjung on 2018/1/23.
 */
public enum ErrorEnum {

    //未知错误
    UNKNOWN("V_E_001", "未知错误"),

    //短信验证码相关
    CODE_NOT_EXISTED("V_E_010", "验证码不存在"),
    CODE_INCORRECT("V_E_011", "验证码有误"),
    CODE_EXPIRED("V_E_012", "验证码已过期"),
    CODE_EXCEED_SENDING_MAXIMUM("V_E_013", "禁止频繁发送验证码"),
    CODE_RESENDING_FAIL("V_E_014", "禁止频繁发送验证码"),
    CODE_EXCEED_VERIFYING_MAXIMUM("V_E_015", "验证码已过期"),

    //图片验证码相关
    IMAGE_CODE_INCORRECT("V_E_020", "图片验证码有误"),
    IMAGE_CODE_EXPIRED("V_E_021", "图片验证码已过期"),
    IMAGE_CODE_NOT_EMPTY("V_E_022", "图片验证码不能为空");

    /**
     * 错误码
     */
    private final String code;
    /**
     * 错误信息
     */
    private final String msg;

    ErrorEnum(String code, String msg) {

        this.code = code;
        this.msg = msg;
    }

    /**
     * 返回错误码
     *
     * @return 错误码
     */
    public String code() {
        return code;
    }

    /**
     * 返回错误信息
     *
     * @return 错误信息
     */
    public String msg() {
        return msg;
    }
}
