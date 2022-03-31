package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

/**
 * 获取token的请求参数错误异常
 */
public class GetTokenRequestParameterErrorException extends BaseBusinessException {
    public GetTokenRequestParameterErrorException() {
        super(ErrorCodeEnum.GET_TOKEN_REQUEST_PARAMETER_ERROR);
    }

    public GetTokenRequestParameterErrorException(String msg) {
        super(ErrorCodeEnum.GET_TOKEN_REQUEST_PARAMETER_ERROR, msg);
    }

}
