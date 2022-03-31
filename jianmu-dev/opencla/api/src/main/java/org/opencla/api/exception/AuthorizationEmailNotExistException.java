package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

/**
 * @author Daihw
 * @class AuthorizationEmailNotExistException
 * @description 授权邮箱不存在异常
 * @create 2022/2/9 4:51 下午
 */
public class AuthorizationEmailNotExistException extends BaseBusinessException {
    public AuthorizationEmailNotExistException() {
        super(ErrorCodeEnum.AUTHORIZATION_EMAIL_NOT_EXIST);
    }
}
