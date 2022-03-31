package org.opencla.api.exception;

import org.opencla.api.enumeration.ErrorCodeEnum;

/**
 * 非支持的第三方平台异常
 */
public class NotSupportedThirdPartPlatformException extends BaseBusinessException {
    public NotSupportedThirdPartPlatformException() {
        super(ErrorCodeEnum.NOT_SUPPORTED_THIRD_PART_PLATFORM);
    }
}
