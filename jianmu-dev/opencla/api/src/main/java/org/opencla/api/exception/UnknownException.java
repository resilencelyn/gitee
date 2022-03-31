package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

public class UnknownException extends BaseBusinessException{
    public UnknownException() {
        super(ErrorCodeEnum.UNKNOWN);
    }
}
