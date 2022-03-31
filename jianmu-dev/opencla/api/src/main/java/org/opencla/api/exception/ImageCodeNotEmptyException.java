package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

public class ImageCodeNotEmptyException extends BaseBusinessException{
    public ImageCodeNotEmptyException() {
        super(ErrorCodeEnum.IMAGE_CODE_NOT_EMPTY);
    }
}
