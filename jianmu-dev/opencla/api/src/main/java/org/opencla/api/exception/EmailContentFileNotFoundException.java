package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

public class EmailContentFileNotFoundException extends BaseBusinessException{
    public EmailContentFileNotFoundException() {
        super(ErrorCodeEnum.EMAIL_CONTENT_FILE_NOT_EXISTS);
    }
}
