package org.opencla.verification.model.Impl;

import org.opencla.verification.model.BaseModel;
import lombok.Getter;

/**
 * 邮箱模型
 */
@Getter
public class EmailModel extends BaseModel {

    /**
     * 邮箱地址
     */
    private String emailAddress;


    /**
     * 构造SmsModel
     *
     * @param emailAddress
     * @param identifyCode
     * @param identifyCodeExpiration
     * @param identifyCodeResendingTime
     * @param sendingCount
     * @param flag
     */
    public EmailModel(String emailAddress, String identifyCode, Long identifyCodeExpiration, Long identifyCodeResendingTime, int sendingCount, String flag) {
        this.emailAddress = emailAddress;
        super.identifyCode = identifyCode;
        super.identifyCodeExpiration = identifyCodeExpiration;
        super.identifyCodeResendingTime = identifyCodeResendingTime;
        super.sendingCount = sendingCount;
        super.flag = flag;
    }

    /**
     * 刷新短信数据
     *
     * @param identifyCode
     * @param identifyCodeExpiration
     * @param identifyCodeResendingTime
     * @param verifyingCount
     */
    public void refreshSmsCodeData(String identifyCode, Long identifyCodeExpiration, Long identifyCodeResendingTime, int verifyingCount) {
        super.identifyCode = identifyCode;
        super.identifyCodeExpiration = identifyCodeExpiration;
        super.identifyCodeResendingTime = identifyCodeResendingTime;
        super.verifyingCount = verifyingCount;
    }

    /**
     * 增加发送次数
     *
     * @param sendingCount
     */
    public void increaseSendingNumber(int sendingCount) {
        super.sendingCount = sendingCount;
    }


    @Override
    public String id() {
        return super.flag + ":" + this.emailAddress;
    }
}
