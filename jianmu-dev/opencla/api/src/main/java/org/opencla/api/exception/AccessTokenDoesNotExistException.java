package org.opencla.api.exception;

import org.checkerframework.checker.units.qual.A;
import org.opencla.api.enumeration.ErrorCodeEnum;

/**
 * accessToken不存在异常
 */
public class AccessTokenDoesNotExistException extends BaseBusinessException {
    public AccessTokenDoesNotExistException() {
        super(ErrorCodeEnum.ACCESS_TOKEN_DOES_NOT_EXIST);
    }
    public AccessTokenDoesNotExistException(String msg){
        super(ErrorCodeEnum.ACCESS_TOKEN_DOES_NOT_EXIST, msg);
    }
}
