package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

/**
 * @author Daihw
 * @class AgreementSignedException
 * @description 协议重复签署异常
 * @create 2022/1/11 10:50 上午
 */
public class AgreementSignedException extends BaseBusinessException {
    public AgreementSignedException() {
        super(ErrorCodeEnum.AGREEMENT_SIGNED);
    }
}
