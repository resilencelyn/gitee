package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

public class ImageCodeExpiredException extends BaseBusinessException{
    public ImageCodeExpiredException(){
        super(ErrorCodeEnum.IMAGE_CODE_EXPIRED);
    }
}
