package org.opencla.verification.service.email.impl;

import org.opencla.verification.service.BaseConfig;
import lombok.Getter;
import lombok.Setter;

/**
 * 所有邮件配置
 */
@Setter
@Getter
public class EmailConfig extends BaseConfig {
    /**
     * 发件人的邮箱
     */
    private String sender;
    /**
     * 邮箱授权码
     */
    private String authorizationCode;
    /**
     * 发件人SMTP服务器地址
     */
    private String host;
    /**
     * 默认使用邮箱服务的ssl端口
     */
    private Integer port = 465;
    /**
     * 是否使用ssl连接邮箱服务器
     */
    private Boolean useSsl = true;
    /**
     * 是否打印日志
     */
    private Boolean debug = false;
}
