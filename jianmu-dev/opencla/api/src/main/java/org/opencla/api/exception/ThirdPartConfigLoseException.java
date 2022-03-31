package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

public class ThirdPartConfigLoseException extends BaseBusinessException{

    public ThirdPartConfigLoseException() {
        super(ErrorCodeEnum.THIRD_PART_CONFIG_LOSE);
    }
}
