package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

public class ValidationImageCodeFailedException extends BaseBusinessException{
    public ValidationImageCodeFailedException() {
        super(ErrorCodeEnum.VALIDATION_IMAGE_CODE_FAILED);
    }
}
