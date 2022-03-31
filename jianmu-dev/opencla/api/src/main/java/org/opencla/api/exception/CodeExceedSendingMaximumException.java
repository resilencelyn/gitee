package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

public class CodeExceedSendingMaximumException extends BaseBusinessException{
    public CodeExceedSendingMaximumException() {
        super(ErrorCodeEnum.CODE_EXCEED_SENDING_MAXIMUM);
    }
}
