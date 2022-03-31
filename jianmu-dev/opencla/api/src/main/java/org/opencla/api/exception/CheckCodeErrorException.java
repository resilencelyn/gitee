package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

/**
 * @class CheckCodeErrorException
 * @description 验证码错误异常
 * @author Daihw
 * @create 2022/2/9 5:02 下午
 */
public class CheckCodeErrorException extends BaseBusinessException {
    public CheckCodeErrorException(String message) {
        super(ErrorCodeEnum.CHECK_CODE_ERROR, message);
    }
}
